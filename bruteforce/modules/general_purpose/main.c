#include <stdio.h>
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
#include <stdlib.h>
#include "time.h"
#include "bruteforce/framework/candidates.h"

struct GraphNodeCamera camera;
struct Object marioObj;
struct Area area;
struct MarioBodyState marioBodyState;
struct Controller testController;
f32 minSpeed;

void initGame() {
	gCamera = &camera;
	gCurrentArea = &area;
	init_graph_node_object(NULL, &marioObj, NULL, gVec3fZero, gVec3sZero, gVec3fOne);
	create_camera(&camera, NULL);
	gCurrentArea->camera = camera.config.camera;

	gMarioState->marioObj = &marioObj;
	gMarioState->marioBodyState = &marioBodyState;
	gMarioState->statusForCamera = &gPlayerCameraState[0];

	gMarioState->controller = &testController;
	gMarioState->area = &area;
	
	printf("Loading configuration...\n");
	if (!bf_init_states()) {
		printf("Failed to load configuration! Exiting...\n");
		exit(-1);
	}
	printf("m64 path is %s, %p, loaded from %s \n", bfStaticState.m64_input, &bfStaticState.m64_input[0], override_config_file);
	
	clear_static_surfaces();
	init_static_surfaces(bfStaticState.static_tris);

	// srand(bfStaticState.rnd_seed);
	
	// Still required to initialize something?
	execute_mario_action(gMarioState->marioObj);
	update_camera(gCurrentArea->camera);
}

void updateGame(OSContPad *input) {
	testController.rawStickX = input->stick_x;
	testController.rawStickY = input->stick_y;
	testController.buttonPressed = input->button
								& (input->button ^ testController.buttonDown);
	testController.buttonDown = input->button;

	adjust_analog_stick(&testController);
	execute_mario_action(gMarioState->marioObj);
	if (gCurrentArea != NULL) {
		update_camera(gCurrentArea->camera);
	}
}

void perturbInput(OSContPad *input) {
	if (bfStaticState.max_perturbation > 0 && randFloat() < bfStaticState.perturbation_chance) {
		u16 perturb = (u16)(bfStaticState.max_perturbation);
		u8 perturbation_x = (rand() % (2 * perturb) - perturb);
		u8 perturbation_y = (rand() % (2 * perturb) - perturb);
		input->stick_x = (s8)(input->stick_x + perturbation_x);
		input->stick_y = (s8)(input->stick_y + perturbation_y);
	}
}

u8 updateScore(Candidate *candidate, u32 frame_idx) {
	u8 success = TRUE;
	u32 i;

	if (bfStaticState.score_on_last_frame && frame_idx + 1 != candidate->sequence->count)
		success = FALSE;

	candidate->stats.frame_index = frame_idx;
	candidate->score = 0.0;
	for (i = 0; i < bfStaticState.scoring_methods.n_methods; i++) 
		if (bfStaticState.scoring_methods.methods[i].frame == frame_idx + 1) {
			u8 abort = FALSE;
			applyMethod(&bfStaticState.scoring_methods.methods[i], candidate, &success, &abort);
			if (abort)
				return FALSE;
		}

	u8 best = success && candidate->score > programState->bestScore;
	if (best) {
		programState->bestScore = candidate->score;
		output_input_sequence(candidate->sequence);
		printf("New best: %f\n", candidate->score);
	}

	return TRUE;
}

#include "bruteforce/snippets/bruteforce_loop.inc.c"

void main(int argc, char *argv[]) {
	parse_command_line_args(argc, argv);

	printf("Running Bruteforcer...\n");
	initGame();

	printf("Loading m64...\n");
	InputSequence *original_inputs;
	if (!read_m64_from_file(bfStaticState.m64_input, bfStaticState.m64_start, bfStaticState.m64_count, &original_inputs))
	{
		printf("Failed to load m64! (%s) Exiting...\n", bfStaticState.m64_input);
		exit(-1);
	}

	initializeMultiProcess(original_inputs);
	if (isParentProcess())
		programState->bestScore = -INFINITY;

	bruteforce_loop(original_inputs);
}