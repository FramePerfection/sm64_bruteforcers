#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "src/engine/surface_collision.h"
#include "src/engine/surface_load.h"
#include "src/engine/math_util.h"
#include "src/game/area.h"
#include "src/game/camera.h"
#include "src/game/game_init.h"
#include "src/game/level_update.h"
#include "src/game/mario.h"
#include "src/game/mario_step.h"
#include "src/game/object_list_processor.h"
#include "src/game/spawn_object.h"

#include "bruteforce/framework/candidates.h"
#include "bruteforce/framework/interface.h"
#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/m64.h"
#include "bruteforce/framework/misc_util.h"
#include "bruteforce/framework/states.h"

#include "bruteforce/algorithms/genetic/algorithm.h"

#include "perturbator.h"
#include "behavior_function_map.h"

extern BehaviorScript bhvMario[];

static void remapBehaviorScript(BehaviorScriptWrapper script) {
    u32 i;
    for (i = 0; i < script.size; i++) {
        u8 command = script.data[i] >> 0x18; 
        if (command == 0x2A) { // LOAD_COLLISION_DATA
            s16* mappedValue;
            // TODO: use multiple objects m9
            if (bfStaticState.test_dynamic_object_tris.overrides == (BehaviorScript)script.data[i + 1]) {
                bf_safe_printf("This works.");
                mappedValue = bfStaticState.test_dynamic_object_tris.data;
            }
            script.data[i + 1] = (BehaviorScript)mappedValue;
        }
        else if (command == 0x0C) { // EXECUTE_NATIVE_FUNC
            script.data[i + 1] = (BehaviorScript)bf_map_native_behavior_func((u32)script.data[i + 1]);
        }
    }
}

static void initGame()
{
    // TODO: load and reset object state etc.
    clear_objects();
    bf_init_camera();
    bf_init_area();
    bf_init_mario();

    bf_safe_printf("Loading configuration...\n");
    if (!bf_init_states())
    {
        bf_safe_printf("Failed to load configuration! Exiting...\n");
        exit(-1);
    }
    bf_safe_printf("m64 path is %s, %p, loaded from %s \n", bfStaticState.m64_input, &bfStaticState.m64_input[0], override_config_file);

    clear_static_surfaces();
    clear_dynamic_surfaces();
    bf_init_static_surfaces(bfStaticState.static_tris);
    bf_init_dynamic_surfaces(bfStaticState.dynamic_tris);

    // srand(bfStaticState.rnd_seed);

    init_camera(gCamera);
    // Still required to initialize something?
    execute_mario_action(gMarioState->marioObj);
    update_camera(gCurrentArea->camera);

    if (bfStaticState.update_objects) {
        gMarioState->marioObj = gMarioObject = create_object(bhvMario);
        vec3f_copy(gMarioObject->header.gfx.pos, gMarioState->pos);
        remapBehaviorScript(bfStaticState.test_behavior_script);
        struct Object *testObj = create_object(bfStaticState.test_behavior_script.data);
        testObj->rawData.asF32[0x06] = -3111.0f;
        testObj->rawData.asF32[0x07] = -829;
        testObj->rawData.asF32[0x08] = 3497;
        testObj->rawData.asS32[0x13] = 63716;
    }
}

static void updateGame(OSContPad *input, UNUSED u32 frame_index)
{
    bf_update_controller(input);
    adjust_analog_stick(gPlayer1Controller);
    
    if (bfStaticState.update_objects)
        area_update_objects();
    else
        execute_mario_action(gMarioState->marioObj);
    
    if (gCurrentArea != NULL)
    {
        update_camera(gCurrentArea->camera);
    }
    // bf_safe_printf("%f; %f; %f\n", gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2]);
}

static void perturbInput(UNUSED Candidate *candidate, OSContPad *input, u32 frame_idx)
{
    if (!bfStaticState.relative_frame_numbers)
        frame_idx += bfStaticState.m64_start;

    u32 i;
    for (i = 0; i < bfStaticState.perturbators.nPerturbators; i++)
    {
        Perturbator *perturbator = &bfStaticState.perturbators.perturbators[i];
        if (frame_idx < perturbator->min_frame || frame_idx > perturbator->max_frame)
            continue;

        if (perturbator->max_perturbation > 0 && bf_random_float() < perturbator->perturbation_chance)
        {
            u16 perturb = (u16)(perturbator->max_perturbation);
            u8 perturbation_x = (rand() % (2 * perturb) - perturb);
            u8 perturbation_y = (rand() % (2 * perturb) - perturb);
            input->stick_x = (s8)(input->stick_x + perturbation_x);
            input->stick_y = (s8)(input->stick_y + perturbation_y);
        }
    }
}

static void updateScore(Candidate *candidate, u32 frame_idx, boolean *abort)
{
    *abort = FALSE;
    u8 success = TRUE;
    u32 i;
    u8 lastFrame = frame_idx + 1 == candidate->sequence->count;

    if (bfStaticState.score_on_last_frame || frame_idx == 0)
        candidate->score = 0.0;

    if (!bfStaticState.relative_frame_numbers)
        frame_idx += bfStaticState.m64_start;

    candidate->stats.frame_index = frame_idx;
    for (i = 0; i < bfStaticState.scoring_methods.n_methods; i++)
        if (bfStaticState.scoring_methods.methods[i].frame == frame_idx + 1)
        {
            applyMethod(&bfStaticState.scoring_methods.methods[i], candidate, &success, abort);
            if (*abort)
                return;
        }

    u8 best = success && candidate->score > programState->bestScore;
    if (best && lastFrame)
    {
        programState->bestScore = candidate->score;
        bf_output_input_sequence(bfInitialDynamicState.global_timer, candidate->sequence);
        bf_safe_printf("New best: %f\n", candidate->score);
    }
}

void main(int argc, char *argv[])
{
    bf_parse_command_line_args(argc, argv);

    bf_safe_printf("Running Bruteforcer...\n");
    initGame();

    bf_safe_printf("Loading m64...\n");
    InputSequence *original_inputs;
    if (!bf_read_m64_from_file(bfStaticState.m64_input, bfStaticState.m64_start, bfStaticState.m64_end, &original_inputs))
    {
        bf_safe_printf("Failed to load m64! (%s) Exiting...\n", bfStaticState.m64_input);
        exit(-1);
    }

    bf_initialize_multi_process(original_inputs);
    if (isParentProcess())
        programState->bestScore = -INFINITY;

    bf_algorithm_genetic_loop(original_inputs, &updateGame, &perturbInput, &updateScore);
}