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
#include "bruteforce/algorithms/genetic/algorithm.h"

#include "bruteforce/framework/interface.h"
#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/m64.h"
#include "bruteforce/framework/misc_util.h"
#include "bruteforce/framework/quarter_steps.h"
#include "bruteforce/framework/states.h"

f32 minSpeed;

InputSequence *original_inputs;
Candidate *survivors;
Candidate *best;

extern Vec3f last_q_step, last_q_step2;

void initGame()
{
    bf_init_camera();
    bf_init_area();
    bf_init_mario();

    bf_safe_printf("Loading configuration...\n");
    if (!bf_init_states())
    {
        bf_safe_printf("Failed to load configuration! Exiting...\n");
        exit(-1);
    }

    clear_static_surfaces();
    bf_init_static_surfaces(bfStaticState.static_tris);

    time_t t;
    srand((unsigned)time(&t));
}

void updateGame(OSContPad *input)
{
    bf_update_controller(input);
    adjust_analog_stick(gPlayer1Controller);
    execute_mario_action(gMarioState->marioObj);
    if (gCurrentArea != NULL)
    {
        update_camera(gCurrentArea->camera);
    }
}

void perturbInput(OSContPad *input)
{
    if (bfStaticState.max_perturbation > 0 && bf_random_float() <= bfStaticState.perturbation_chance)
    {
        u16 perturb = (u16)(bfStaticState.max_perturbation);
        u8 perturbation_x = (rand() % (2 * perturb) - perturb);
        u8 perturbation_y = (rand() % (2 * perturb) - perturb);
        input->stick_x = (s8)(input->stick_x + perturbation_x);
        input->stick_y = (s8)(input->stick_y + perturbation_y);
    }
}

u8 updateScore(Candidate *candidate, u32 frame_idx)
{
    if (frame_idx == bfStaticState.scoring_frame - 1)
    {
        Vec3f *last_q_step = bf_get_quarterstep(2, 0);
        Vec3f *last_q_step2 = bf_get_quarterstep(3, 0);
        f64 dist = (*last_q_step)[0] * bfStaticState.plane_nx + (*last_q_step)[2] * bfStaticState.plane_nz + bfStaticState.plane_d - 50.0;
        f64 dist2 = (*last_q_step2)[0] * bfStaticState.plane_nx + (*last_q_step2)[2] * bfStaticState.plane_nz + bfStaticState.plane_d - 50.0;
        f64 score = -(dist * dist + dist2 * dist2);
        u8 best = gMarioStates->forwardVel > programState->bestScore;
        if (!best)
            score = -INFINITY;
        else
            score /= powf(2.0, (gMarioState->forwardVel - programState->bestScore) * 15);

        if (gMarioState->faceAngle[1] == (s16)(bfStaticState.gwk_angle + 0x8000))
        {
            bf_safe_printf("Found one: %d, %f %s\n", gMarioState->faceAngle[1], gMarioState->forwardVel, best ? "\t(new best!)" : "");
            if (best)
            {
                bf_output_input_sequence(bfInitialDynamicState.global_timer, candidate->sequence);
                programState->bestScore = gMarioState->forwardVel;
            }
        }
        candidate->stats.hSpeed = gMarioStates->forwardVel;
        candidate->score = score;
    }

    return TRUE;
}

void bruteforceLoop()
{
    clock_t lastClock = clock();
    u32 gen_mod = bfControlState->print_interval;

    u32 gen;
    for (gen = 0; gen < bfStaticState.max_generations; gen++)
    {
        if (gen % gen_mod == 0)
        {
            clock_t curClock = clock();
            float seconds = (float)(curClock - lastClock) / CLOCKS_PER_SEC;
            float fps = gen_mod * original_inputs->count * bfStaticState.runs_per_survivor * bfStaticState.survivors_per_generation / seconds;
            lastClock = curClock;
            bf_safe_printf("Generation %d starting... (bestSpeed %f, %f FPS)\n", gen, programState->bestScore, fps);
        }

        // perform all runs
        u32 candidate_idx;
        for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
        {
            u32 run_idx;
            for (run_idx = 0; run_idx < bfStaticState.runs_per_survivor; run_idx++)
            {
                Candidate *candidate = &survivors[candidate_idx].children[run_idx];
                Candidate *original = &survivors[candidate_idx];

                InputSequence *inputs = candidate->sequence;
                bf_clone_m64_inputs(inputs, original->sequence);

                u8 keepOriginal = run_idx == 0 && (bf_random_float() > bfControlState->forget_rate);

                bf_load_dynamic_state(&bfInitialDynamicState);
                gPlayer1Controller->rawStickX = inputs->originalInput.stick_x;
                gPlayer1Controller->rawStickY = inputs->originalInput.stick_y;
                gPlayer1Controller->buttonDown = inputs->originalInput.button;

                u32 frame_idx;
                for (frame_idx = 0; frame_idx < inputs->count; frame_idx++)
                {
                    OSContPad *currentInput = &inputs->inputs[frame_idx];
                    if (!keepOriginal)
                        perturbInput(currentInput);
                    updateGame(currentInput);
                    updateScore(candidate, frame_idx);
                }

                if (candidate->stats.hSpeed < programState->bestScore - bfControlState->score_leniency)
                    bf_clone_m64_inputs(inputs, original->sequence);
            }
        }

        // sort by scoring
        bf_update_survivors(survivors);
        bf_update_best(best, survivors);

        bf_algorithm_genetic_update_interprocess(survivors);
    }
}

void main(int argc, char *argv[])
{
    bf_parse_command_line_args(argc, argv);
    bf_safe_printf("Running Bruteforcer...\n");

    bf_safe_printf("Initializing game state...\n");
    initGame();

    bf_safe_printf("Loading m64...\n");
    if (!bf_read_m64_from_file(bfStaticState.m64_input, bfStaticState.m64_start, bfStaticState.m64_end, &original_inputs))
    {
        bf_safe_printf("Failed to load m64! Exiting...\n");
        exit(-1);
    }
    bf_init_candidates(original_inputs, &survivors);
    bf_init_candidates(original_inputs, &best);

    bf_algorithm_genetic_init(original_inputs);
    bf_start_multiprocessing();
    
    programState->bestScore = minSpeed;

    bruteforceLoop();
}