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
	if (frame_idx == bfStaticState.sliding_start_frame -1 && gMarioState->action != bfStaticState.sliding_action) {
		candidate->score = INFINITY;
		candidate->stats.hSpeed = INFINITY;
		return FALSE;
	}
	if (frame_idx == bfStaticState.scoring_frame - 1) {
		f64 score = gMarioStates->forwardVel;
		s16 dYaw = gMarioState->faceAngle[1] - bfStaticState.target_angle;
		if (dYaw < 0)
			dYaw = -dYaw;
		if (dYaw > bfStaticState.target_angle_margin)
			score = -dYaw;
		if (gMarioState->action != bfStaticState.target_action)
			score = -INFINITY;
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
			printf("New best: %f\n", gMarioState->forwardVel);
		}
	}
	return TRUE;
}

void printVec3f(Vec3f *v) {
	printf("(%f, %f, %f)", (*v)[0], (*v)[1], (*v)[2]);
}

void debug_print_run(s32 frame_idx) {
	printf("%d - Mario (%x); (%x): ", frame_idx, gMarioState->health, gMarioState->action);
	printVec3f(&gMarioState->pos);
	printf("\tSliding (%f, -,%f)\t", gMarioState->slideVelX, gMarioState->slideVelZ);
	printVec3f(&gMarioState->vel);
	printf("\n");

	if (frame_idx == bfStaticState.m64_count - 1) {
		printf("break;\n");
	}
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
		printf("Failed to load m64! Exiting...\n");
		exit(-1);
	}

	Candidate *survivors;
	Candidate *best;
	initCandidates(original_inputs, &survivors);
	initCandidates(original_inputs, &best);

	initializeMultiProcess(original_inputs);
	
	programState->bestScore = minSpeed;
	
	bruteforce_loop(original_inputs);
}