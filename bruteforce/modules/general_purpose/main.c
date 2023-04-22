#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "src/engine/surface_collision.h"
#include "src/engine/surface_load.h"
#include "src/game/mario.h"
#include "src/game/mario_step.h"
#include "src/game/level_update.h"
#include "src/game/area.h"
#include "src/game/camera.h"
#include "src/game/game_init.h"

#include "bruteforce/framework/misc_util.h"
#include "bruteforce/framework/bf_states.h"
#include "bruteforce/framework/m64.h"
#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/interface.h"
#include "bruteforce/framework/candidates.h"

#include "bruteforce/algorithms/genetic/algorithm.h"

#include "perturbator.h"

static void initGame() {
	initCamera();
	initArea();
	initMario();
	
	safePrintf("Loading configuration...\n");
	if (!bf_init_states()) {
		safePrintf("Failed to load configuration! Exiting...\n");
		exit(-1);
	}
	safePrintf("m64 path is %s, %p, loaded from %s \n", bfStaticState.m64_input, &bfStaticState.m64_input[0], override_config_file);
	
	clear_static_surfaces();
	clear_dynamic_surfaces();
	init_static_surfaces(bfStaticState.static_tris);
	init_dynamic_surfaces(bfStaticState.dynamic_tris);

	// srand(bfStaticState.rnd_seed);
	
	init_camera(gCamera);
	// Still required to initialize something?
	execute_mario_action(gMarioState->marioObj);
	update_camera(gCurrentArea->camera);
}

static void updateGame(OSContPad *input) {
	updateController(input);
	adjust_analog_stick(gPlayer1Controller);
	execute_mario_action(gMarioState->marioObj);
	if (gCurrentArea != NULL) {
		update_camera(gCurrentArea->camera);
	}
}

static void perturbInput(UNUSED Candidate *candidate, OSContPad *input, u32 frame_idx) {
	if (!bfStaticState.relative_frame_numbers)
		frame_idx += bfStaticState.m64_start;

	u32 i;
	for (i = 0; i < bfStaticState.perturbators.nPerturbators; i++) {
		Perturbator *perturbator = &bfStaticState.perturbators.perturbators[i];
		if (frame_idx < perturbator->min_frame || frame_idx > perturbator->max_frame)
			continue;

		if (perturbator->max_perturbation > 0 && randFloat() < perturbator->perturbation_chance) {
			u16 perturb = (u16)(perturbator->max_perturbation);
			u8 perturbation_x = (rand() % (2 * perturb) - perturb);
			u8 perturbation_y = (rand() % (2 * perturb) - perturb);
			input->stick_x = (s8)(input->stick_x + perturbation_x);
			input->stick_y = (s8)(input->stick_y + perturbation_y);
		}
	}
}

static void updateScore(Candidate *candidate, u32 frame_idx, boolean *abort) {
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
		if (bfStaticState.scoring_methods.methods[i].frame == frame_idx + 1) {
			applyMethod(&bfStaticState.scoring_methods.methods[i], candidate, &success, abort);
			if (*abort) return;
		}

	u8 best = success && candidate->score > programState->bestScore;
	if (best && lastFrame) {
		programState->bestScore = candidate->score;
		output_input_sequence(bfInitialDynamicState.global_timer, candidate->sequence);
		safePrintf("New best: %f\n", candidate->score);
	}
}

void main(int argc, char *argv[]) {
	parse_command_line_args(argc, argv);

	safePrintf("Running Bruteforcer...\n");
	initGame();

	safePrintf("Loading m64...\n");
	InputSequence *original_inputs;
	if (!read_m64_from_file(bfStaticState.m64_input, bfStaticState.m64_start, bfStaticState.m64_end, &original_inputs))
	{
		safePrintf("Failed to load m64! (%s) Exiting...\n", bfStaticState.m64_input);
		exit(-1);
	}

	initializeMultiProcess(original_inputs);
	if (isParentProcess())
		programState->bestScore = -INFINITY;

	bruteforce_loop_genetic(original_inputs, &updateGame, &perturbInput, &updateScore);
}