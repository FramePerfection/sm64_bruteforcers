#include "bruteforce/algorithms/genetic/algorithm.h"

#include <time.h>

#include "src/game/game_init.h"

#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/bf_states.h"

void bruteforce_loop_genetic(InputSequence *original_inputs, UpdateGameFunc updateGame, PerturbInputFunc perturbInput, ScoringFunc updateScore) {
	Candidate *survivors;
	Candidate *best;
	initCandidates(original_inputs, &survivors);
	initCandidates(original_inputs, &best);
	
	clock_t lastClock = clock();

	u32 gen_merge_mod = bfStaticState.merge_interval;
	if (gen_merge_mod == 0)
		gen_merge_mod = 20;

	u32 frames = 0;
	u32 gen;
	for (gen = 0; gen < bfStaticState.max_generations; gen++) {
		clock_t curClock = clock();
		float seconds = (float)(curClock - lastClock) / CLOCKS_PER_SEC;
		if (seconds >= bfStaticState.print_interval)
		{
			float fps = frames / seconds;
			lastClock = curClock;
			safePrintf("Generation %d starting... (%f FPS) (Best score: %f)\n", gen, fps, programState->bestScore);
			frames = 0;
		}

		// perform all runs
		u32 candidate_idx;
		for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++) {

			u32 run_idx;
			for (run_idx = 0; run_idx < bfStaticState.runs_per_survivor; run_idx++) {
				Candidate *candidate = &survivors[candidate_idx].children[run_idx];
				Candidate *original = &survivors[candidate_idx];
				
				InputSequence *inputs = candidate->sequence;
				clone_m64_inputs(inputs, original->sequence);

				u8 keepOriginal = run_idx == 0 && (randFloat() > bfStaticState.forget_rate);

				gPlayer1Controller->buttonDown = inputs->originalInput.button;
				gPlayer1Controller->rawStickX = inputs->originalInput.stick_x;
				gPlayer1Controller->rawStickY = inputs->originalInput.stick_y;
				bf_load_dynamic_state(&bfInitialDynamicState);

				u32 frame_idx;
				for (frame_idx = 0; frame_idx < inputs->count; frame_idx++) {
					frames++;
					OSContPad *currentInput = &inputs->inputs[frame_idx];
					if (!keepOriginal)
						perturbInput(candidate, currentInput, frame_idx);
					updateGame(currentInput);
					boolean abort = FALSE;
					updateScore(candidate, frame_idx, &abort);
					if (abort) break;
				}
			}
		}
		
		// sort by scoring
		updateSurvivors(survivors);
		updateBest(best, survivors);
		if (!isParentProcess())
			childUpdateMessages(best);

		if (gen % gen_merge_mod == 0)
			if (isParentProcess())
				parentMergeCandidates(survivors);
	}
}