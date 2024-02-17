#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "src/engine/surface_collision.h"
#include "src/engine/surface_load.h"
#include "src/game/area.h"
#include "src/game/camera.h"
#include "src/game/game_init.h"
#include "src/game/level_update.h"
#include "src/game/mario.h"
#include "src/game/mario_step.h"

#include "bruteforce/algorithms/genetic/candidates.h"
#include "bruteforce/framework/interface/interface.h"
#include "bruteforce/framework/interface/m64.h"
#include "bruteforce/framework/misc_util.h"
#include "bruteforce/framework/states.h"
#include "bruteforce/framework/interprocess.h"

#include "bruteforce/algorithms/genetic/algorithm.h"

typedef struct ReferenceFrame_s
{
    f32 x;
    f32 y;
    f32 z;
    f32 hspeed;
    s16 angle;
    u32 action;
    s16 buttonDown;
} ReferenceFrame;

static ReferenceFrame *referenceFrames;
static u32 nReferenceFrames;

static InputSequence *bestInputs;
static u8 hasbest;
static clock_t lastSaveTime;

static boolean readReference()
{
    const char *fileContents = bf_read_file("reference_run.json");
    Json *root = Json_create(fileContents);
    if (!root)
    {
        bf_safe_printf("error in reference_run.json starting at:\n%s\n", Json_getError());
        free((char *)fileContents);
        return FALSE;
    }

    nReferenceFrames = root->size;
    referenceFrames = calloc(nReferenceFrames, sizeof(ReferenceFrame));
    ReferenceFrame *currentFrame = &referenceFrames[0];
    Json *frameNode = root->child;
    while (frameNode != NULL)
    {

        // TODO: this can be abstracted better
        // clang-format off

		#define JSON_SRC(NAME)                      \
			if (strcmp(dataNode->name, #NAME) == 0) \
			currentFrame->NAME = dataNode->
		Json *dataNode = frameNode->child;
		while (dataNode != NULL){
			JSON_SRC(x)valueFloat;
			JSON_SRC(y)valueFloat;
			JSON_SRC(z)valueFloat;
			JSON_SRC(angle)valueInt;
			JSON_SRC(hspeed)valueFloat;
			JSON_SRC(action)valueInt;
			JSON_SRC(buttonDown)valueInt;
			dataNode = dataNode->next;
		}
		#undef JSON_SRC
        // clang-format on
        frameNode = frameNode->next;
        currentFrame++;
    }
    free((char *)fileContents);
    Json_dispose(root);
    return TRUE;
}

static void initGame()
{
    bf_init_camera();
    bf_init_area();
    bf_init_mario();

    bf_safe_printf("Loading reference run...\n");
    if (!readReference())
    {
        bf_safe_printf("Failed to read reference run!");
        exit(-1);
    }

    bf_safe_printf("Loading configuration...\n");
    if (!bf_init_states())
    {
        bf_safe_printf("Failed to load configuration! Exiting...\n");
        exit(-1);
    }

    clear_static_surfaces();
    clear_dynamic_surfaces();
    bf_init_static_surfaces(bfStaticState.static_tris);
    bf_init_dynamic_surfaces(bfStaticState.dynamic_tris);

    init_camera(gCamera);
}

static void updateGame(OSContPad *input, UNUSED u32 frame_index)
{
    bf_update_controller(input);
    adjust_analog_stick(gPlayer1Controller);
    execute_mario_action(gMarioState->marioObj);
    if (gCurrentArea != NULL)
    {
        update_camera(gCurrentArea->camera);
    }
    gGlobalTimer++;
}

static void perturbInput(UNUSED Candidate *candidate, OSContPad *input, u32 frame_idx)
{
    if (!bfStaticState.relative_frame_numbers)
        frame_idx += bfStaticState.m64_start;

    UNUSED f64 score_diff = candidate->stats.lastScore - candidate->score;
    f64 chance = 1.0; // 1.0 / (1.0 + exp(-score_diff));
    if (bf_random_float() < chance * bfStaticState.perturbation_chance)
    {
        u16 perturb = (u16)(bfStaticState.max_perturbation);
        u8 perturbation_x = (rand() % (2 * perturb) - perturb);
        u8 perturbation_y = (rand() % (2 * perturb) - perturb);
        input->stick_x = (s8)(input->stick_x + perturbation_x);
        input->stick_y = (s8)(input->stick_y + perturbation_y);
    }
}

static f64 getError(ReferenceFrame *reference)
{
    f64 x_diff = (gMarioState->pos[0] - reference->x) * bfControlState->x_weight;
    f64 y_diff = (gMarioState->pos[1] - reference->y) * bfControlState->y_weight;
    f64 z_diff = (gMarioState->pos[2] - reference->z) * bfControlState->z_weight;
    f64 hspeed_diff = (gMarioState->forwardVel - reference->hspeed) * bfControlState->hspeed_weight;
    f64 angle_diff = (gMarioState->faceAngle[1] - reference->angle) * bfControlState->angle_weight;
    return (x_diff * x_diff) + (y_diff * y_diff) + (z_diff * z_diff) + (hspeed_diff * hspeed_diff) + (angle_diff * angle_diff);
}

static void updateScore(Candidate *candidate, u32 frame_idx, boolean *abort)
{
    if (programState->breakLoop) {
        *abort = TRUE;
        return;
    }

    if (frame_idx == 0)
        candidate->score = 0;

    ReferenceFrame* expected = &referenceFrames[frame_idx + bfStaticState.reference_offset + 1];
        
    boolean actionMismatch = gMarioState->action != expected->action;
    if (actionMismatch)
         *abort = TRUE;
    candidate->stats.lastScore = candidate->score;

    f64 frame_diff = getError(expected);
    candidate->score -= frame_diff * frame_diff * (frame_idx + 1);
    candidate->stats.frame_index = frame_idx;

    if (frame_idx >= programState->furthestActionMatch)
    {
        if (!actionMismatch && frame_idx > programState->furthestActionMatch) {
            candidate->score = 0;
            programState->furthestActionMatch = frame_idx;
            programState->breakLoop = TRUE;
        }

        u8 best = candidate->score > programState->bestScore;
        if (best)
        {
            hasbest = TRUE;
            programState->bestScore = candidate->score;
            bf_clone_m64_inputs(bestInputs, candidate->sequence);
        }
    }
}

static void printState(u32 currentGeneration, float fps) {
    bf_safe_printf("Generation %d starting... (%f FPS) (Best score: %f at %d frames)\n", currentGeneration, fps, programState->bestScore, programState->furthestActionMatch);
}

static u32 greedySync(InputSequence *sequence, BFDynamicState *savestate, u32 offset) {
    u32 i;
    s8 x, y;
    BFDynamicState state;
    u32 nFramesToMatch = bfStaticState.m64_end - bfStaticState.m64_start;
    if (nReferenceFrames < nFramesToMatch)
        nFramesToMatch = nReferenceFrames;

    bf_load_dynamic_state(savestate);
    for (i = offset; i < nFramesToMatch; i++)
    {
        u32 k = i + bfStaticState.reference_offset;
        bf_safe_printf("Matching frame %d...\n", k);

        bf_save_dynamic_state(&state);
        OSContPad cont;
        memcpy(&cont, &sequence->inputs[i], sizeof(OSContPad));
        f64 minError = INFINITY;
        if (bfStaticState.match_reference_buttons) {
            cont.button = (u16)referenceFrames[k + 1].buttonDown;
            // cont.button &= ~0x1F | U_CBUTTONS; // do not sync C-Buttons and R-Button
        }

        // We expect to match one frame after the current frame in the sequence after updating
        ReferenceFrame* expected = &referenceFrames[k + 1];

        // Iterate the entire set of control stick options...
        for (y = -128; y != 127; y++)
        {
            for (x = -128; x != 127; x++)
            {
                bf_load_dynamic_state(&state);
                cont.stick_x = x;
                cont.stick_y = y;
                updateGame(&cont, i);

                // ... and store the best result in the original input sequence
                f64 newError = getError(expected);
                if (gMarioState->action == expected->action && newError < minError)
                {
                    memcpy(&sequence->inputs[i], &cont, sizeof(OSContPad));
                    minError = newError;
                }
            }
        }

        // Then perform the best found input again and continue from there
        bf_load_dynamic_state(&state);
        updateGame(&sequence->inputs[i], i);

        if (expected->action != gMarioState->action) {
            bf_safe_printf("Failed to match action for frame %d (%d / %d)!\n", i + bfStaticState.m64_start, i, nFramesToMatch);
            return i;
        }

        bf_save_dynamic_state(&state);

        u32 mupenFrame = i + bfStaticState.m64_start + 1;
        bf_safe_printf("Matched frame %d (%d) with error %f!\n", i, mupenFrame, minError);
        bf_safe_printf("Expected: x: %f; y: %f; z: %f; angle: %d; hspeed: %f; action: %8X\n", expected->x, expected->y, expected->z, expected->angle, expected->hspeed, expected->action);
        bf_safe_printf("Actual: x: %f; y: %f; z: %f; angle: %d; hspeed: %f; action: %8X;\n", gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2], gMarioState->faceAngle[1], gMarioState->forwardVel, gMarioState->action);
    }
    bf_safe_printf("Matching frames done!\n");
    return i - 1;
}

static u8 breakLoop() {
    // Save at most 1 run per second
    clock_t newClock = clock();
    if (hasbest && newClock - lastSaveTime > 50)
    {
        hasbest = FALSE;
        lastSaveTime = newClock;
        bf_output_input_sequence(bfInitialDynamicState.global_timer, bestInputs);
        bf_safe_printf("New best: %f\n", programState->bestScore);
    }

    return programState->breakLoop;
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

    if (bfStaticState.auto_detect_reference_offset) {
        bf_load_dynamic_state(&bfInitialDynamicState);
        unsigned int i;
        f64 minError = INFINITY;
        for (i = 0; i < nReferenceFrames; i++) {
            f64 error = getError(&referenceFrames[i]);
            if (referenceFrames[i].action == bfInitialDynamicState.mario_action && error <= minError) {
                minError = error;
                bfStaticState.reference_offset = i;
            }
        }
        bf_safe_printf("Automatically set reference offset to %d frames with error %f. (buttons: %4X)\n", bfStaticState.reference_offset, minError, referenceFrames[bfStaticState.reference_offset].buttonDown);
    
        ReferenceFrame *f = &referenceFrames[bfStaticState.reference_offset];
        bf_safe_printf("Expected: x: %f; y: %f; z: %f; angle: %d; hspeed: %f; action: %8X\n", f->x, f->y, f->z, f->angle, f->hspeed, f->action);
        bf_safe_printf("Actual: x: %f; y: %f; z: %f; angle: %d; hspeed: %f; action: %8X; vspeed: %f\n", gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2], gMarioState->faceAngle[1], gMarioState->forwardVel, gMarioState->action, gMarioState->vel[1]);
    }

    bf_algorithm_genetic_init(original_inputs);

    bf_start_multiprocessing();
    if (bf_is_parent_process())
        programState->bestScore = -INFINITY;

    // Initial runthrough, find best input to match on every frame one by one until Mario's action fails to synchronize
    programState->furthestActionMatch = greedySync(original_inputs, &bfInitialDynamicState, 0);
    
    bestInputs = bf_clone_m64(original_inputs);

    bf_output_input_sequence(bfInitialDynamicState.global_timer, original_inputs);

    // 2nd phase: optimize inputs to reduce overall error across frames

    while (TRUE) {
        static BFDynamicState savestate;

        bf_load_dynamic_state(&bfInitialDynamicState);
        gPlayer1Controller->buttonDown = referenceFrames[bfStaticState.reference_offset].buttonDown;
        u32 i;
        for (i = 0; (s32)i < ((s32)programState->furthestActionMatch - (s32)bfStaticState.num_fine_tuning_frames); i++) {
            updateGame(&bestInputs->inputs[i], i);
        }
        bf_save_dynamic_state(&savestate);
        
        // run the genetic algorithm until at least one more frame matches the reference run's action
        bf_algorithm_genetic_loop(bestInputs, &savestate, &updateGame, &perturbInput, &updateScore, &printState, &breakLoop);
        
        bf_load_dynamic_state(&bfInitialDynamicState);
        // rerun new best inputs
        for (i = 0; i < programState->furthestActionMatch; i++) {
            updateGame(&bestInputs->inputs[i], i);
        }
        bf_save_dynamic_state(&savestate);

        // greedy sync again until we fail to match the reference action
        programState->furthestActionMatch = greedySync(bestInputs, &savestate, programState->furthestActionMatch);
        programState->breakLoop = FALSE;
        programState->bestScore = -INFINITY;
    }
}