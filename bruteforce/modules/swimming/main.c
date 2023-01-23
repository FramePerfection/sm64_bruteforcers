#include <stdio.h>
#include <stdlib.h>

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

// Stubs that override __attribute__((weak)) functions to prevent null camera crashes
void set_submerged_cam_preset_and_spawn_bubbles(struct MarioState *m) { }
void update_mario_info_for_cam(struct MarioState *m) { }

static void initGame() {
	initCamera(); // Ideally this wouldn't be needed, but there are still functions that reference the camera currently
	initArea();
	initMario();
	
	safePrintf("Loading configuration...\n");
	if (!bf_init_states()) {
		safePrintf("Failed to load configuration! Exiting...\n");
		exit(-1);
	}
	
	clear_static_surfaces();
	init_static_surfaces(bfStaticState.static_tris);
}

static void updateGame(OSContPad *input) {
	updateController(input);
	adjust_analog_stick(gPlayer1Controller);
	execute_mario_action(gMarioState->marioObj);
}

static void perturbInput(OSContPad *input, u32 frame_idx) {
	if (bfStaticState.max_perturbation > 0 && randFloat() < bfStaticState.perturbation_chance) {
		u16 perturb = (u16)(bfStaticState.max_perturbation);
		u8 perturbation_x = (rand() % (2 * perturb) - perturb);
		u8 perturbation_y = (rand() % (2 * perturb) - perturb);
		input->stick_x = (s8)(input->stick_x + perturbation_x);
		input->stick_y = (s8)(input->stick_y + perturbation_y);
	}
}

static void updateScore(Candidate *candidate, u32 frame_idx, boolean *abort) {
	*abort = FALSE;
	if (frame_idx == bfStaticState.swimming_start_frame && !(gMarioState->action & 0x0C0)) {
		candidate->score = INFINITY;
		candidate->stats.hSpeed = INFINITY;
		*abort = TRUE;
		return;
	}
	if (frame_idx + 1 == bfStaticState.scoring_frame ) {
		f64 score = gMarioStates->forwardVel;
		s16 dYaw = gMarioState->faceAngle[1] - bfStaticState.target_angle;
		if (dYaw < 0)
			dYaw = -dYaw;
		if (dYaw > bfStaticState.target_angle_margin)
			score = -dYaw;
		candidate->stats.hSpeed = gMarioState->forwardVel;
		candidate->stats.angle = gMarioState->faceAngle[1];
		candidate->stats.x = gMarioState->pos[0];
		candidate->stats.y = gMarioState->pos[1];
		candidate->stats.z = gMarioState->pos[2];
		candidate->score = score;

		u8 best = gMarioStates->forwardVel > programState->bestScore;
		if (score > 0 && best) {
			programState->bestScore = gMarioState->forwardVel;
			output_input_sequence(candidate->sequence);
			safePrintf("New best: %f\n", gMarioState->forwardVel);
		}
	}
}

void main(int argc, char *argv[]) {
	parse_command_line_args(argc, argv);

	safePrintf("Running Bruteforcer...\n");
	initGame();

	safePrintf("Loading m64...\n");
	InputSequence *original_inputs;
	if (!read_m64_from_file(bfStaticState.m64_input, bfStaticState.m64_start, bfStaticState.m64_count, &original_inputs))
	{
		safePrintf("Failed to load m64! Exiting...\n");
		exit(-1);
	}

	Candidate *survivors;
	Candidate *best;
	initCandidates(original_inputs, &survivors);
	initCandidates(original_inputs, &best);

	initializeMultiProcess(original_inputs);
	
	bruteforce_loop_genetic(original_inputs, &updateGame, &perturbInput, &updateScore);
}