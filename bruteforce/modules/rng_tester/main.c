#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "include/types.h"

#include "bruteforce/framework/interface/interface.h"
#include "bruteforce/framework/interface/m64.h"
#include "bruteforce/framework/interface/debug_json_writer.h"

#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/misc_util.h"
#include "bruteforce/framework/states.h"
#include "bruteforce/framework/objects_interaction/object_utils.h"
#include "bruteforce/framework/objects_interaction/behavior_function_map.h"

#include "include/object_fields.h"

extern BehaviorScript bhvMario[];

u16 rng_index;
static u16 rngIndices[0x10000];
static u16 rngByIndex[0x10001];
extern u16 random_u16();

static BFDebugJsonWriter *debugWriter = NULL;

static void initGame()
{
    u32 i;
    gRandomSeed16 = 0;
    for (i = 0; i < 0x10000; i++) {
        u16 rng = random_u16();
        rngIndices[rng] = i + 1;
        rngByIndex[i + 1] = rng;
    }

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

    // srand(bfStaticState.rnd_seed);

    init_camera(gCamera);
    // Still required to initialize something?
    execute_mario_action(gMarioState->marioObj);
    update_camera(gCurrentArea->camera);

    if (bfStaticState.update_objects) {
        clear_objects();
        gMarioState->marioObj = gMarioObject = create_object(bhvMario);
        vec3f_copy(gMarioObject->header.gfx.pos, gMarioState->pos);
        bf_remap_behavior_scripts(bfStaticState.behavior_scripts, bfStaticState.dynamic_object_tris);
    }

    debugWriter = bf_create_debug_json_writer();

    if (bfStaticState.from_level_load)
        bfInitialDynamicState.rng_value = rngByIndex[bfStaticState.from_level_load_rng_index];
}

static void updateGame(OSContPad *input, UNUSED u32 frame_index)
{
    BFDynamicState lul;
    if (bfStaticState.update_objects && frame_index == 0) {
        bf_save_dynamic_state(&lul);
        bf_reset_objects(bfStaticState.behavior_scripts, bfStaticState.object_states);
        if (!bfStaticState.from_level_load)
            bf_load_dynamic_state(&lul);
    }

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
    gGlobalTimer++;
}

void main(int argc, char *argv[])
{
    BFDynamicState state;
    u32 i;
    u16 initialRng;

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

    bf_load_dynamic_state(&bfInitialDynamicState);
    initialRng = gRandomSeed16;

    if (bfStaticState.debug_desyncs) {
        for (i = 0; i < bfStaticState.m64_end - bfStaticState.m64_start; i++) {
            updateGame(&original_inputs->inputs[i], i);

            bf_save_dynamic_state(&state);
            state.rng_index = rngIndices[state.rng_value];
            
            bf_append_debug_json(debugWriter, &state);
        }
    }
    else {
        float min_thwomp_y = INFINITY;
        s32 min_thwomp_timer = 0x7FFFFFFF;
        do {
            bf_load_dynamic_state(&bfInitialDynamicState);

            // simulate spawning a single dust particle
            for (i = 0; i < 1; i++)
                random_u16();
            bf_save_dynamic_state(&bfInitialDynamicState);

            for (i = 0; i < bfStaticState.m64_end - bfStaticState.m64_start; i++) {
                updateGame(&original_inputs->inputs[i], i);
            }

            struct Object *thwomp = (struct Object *)gObjectListArray[9].next;
            if (thwomp->oAction == 1 && thwomp->oPosY <= min_thwomp_y) {
                min_thwomp_y = thwomp->oPosY;
                if (thwomp->oPosY < min_thwomp_y)
                    min_thwomp_timer = 0x7FFFFFFF;
                if (thwomp->oThwompRandomTimer - thwomp->oTimer <= min_thwomp_timer) {
                    min_thwomp_timer = thwomp->oThwompRandomTimer - thwomp->oTimer;
                    bf_safe_printf("RNG Index: %d; Score: %d, Thwomp Y: %f; Thwomp timer: %d; Thwomp random timer: %d\n",
                        rngIndices[bfInitialDynamicState.rng_value],
                        min_thwomp_timer,
                        min_thwomp_y,
                        thwomp->oTimer,
                        thwomp->oThwompRandomTimer);
                }
            }

        } while(bfInitialDynamicState.rng_value != initialRng);
        bf_safe_printf("Exhaustive search completed!\n");
    }
    bf_output_debug_json(debugWriter, "debug_output.json");
}