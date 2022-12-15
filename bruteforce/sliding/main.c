#include <stdio.h>
#include "src/engine/surface_collision.h"
#include "src/engine/surface_load.h"
#include "src/game/mario.h"
#include "src/game/mario_step.h"
#include "src/game/level_update.h"
#include "src/game/area.h"
#include "src/game/camera.h"
#include "src/game/game_init.h"
#include "bruteforce/misc_util.h"
#include "bruteforce/bf_states.h"
#include "bruteforce/m64.h"
#include "bruteforce/interprocess.h"
#include <stdlib.h>
#include "time.h"
#include "bruteforce/candidates.h"

#define STATE_INCLUDE <bruteforce/MODULE_PATH/state.h>
#include STATE_INCLUDE

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
	u32 i;
	for (i = 0; i < bfStaticState.static_tris.data_size; i++) {
		Triangle t = bfStaticState.static_tris.data[i];
		struct Surface *surface = gen_surface(t.x1, t.y1, t.z1, t.x2, t.y2, t.z2, t.x3, t.y3, t.z3);
		if (surface != NULL)
			add_surface(surface, FALSE);
		else
			printf("found degenerate triangle: (%d,%d,%d),(%d,%d,%d),(%d,%d,%d)\n", t.x1, t.y1, t.z1, t.x2, t.y2, t.z2, t.x3, t.y3, t.z3);
	}

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
		f64 score = -gMarioStates->forwardVel;
		s16 dYaw = gMarioState->faceAngle[1] - bfStaticState.target_angle;
		if (dYaw < 0)
			dYaw = -dYaw;
		if (dYaw > bfStaticState.target_angle_margin)
			score = dYaw;
		if (gMarioState->action != bfStaticState.target_action)
			score = INFINITY;
		candidate->stats.hSpeed = gMarioState->forwardVel;
		candidate->stats.angle = gMarioState->faceAngle[1];
		candidate->stats.x = gMarioState->pos[0];
		candidate->stats.y = gMarioState->pos[1];
		candidate->stats.z = gMarioState->pos[2];
		candidate->score = score;

		u8 best = gMarioStates->forwardVel > programState->bestSpeed;
		if (score < 0 && best) {
			programState->bestSpeed = gMarioState->forwardVel;
			save_to_m64_file(bfStaticState.m64_input, bfStaticState.m64_output, candidate->sequence);
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
	
	programState->bestSpeed = minSpeed;

	clock_t lastClock = clock();
	u32 gen_mod = bfStaticState.print_interval;
	if (gen_mod == 0)
		gen_mod = 100;

	u32 gen_merge_mod = bfStaticState.merge_interval;
	if (gen_merge_mod == 0)
		gen_merge_mod = gen_mod;

	u32 frames = 0;
	u32 gen;
	for (gen = 0; gen < bfStaticState.max_generations; gen++) {
		if (gen % gen_mod == 0)
		{
			clock_t curClock = clock();
			float seconds = (float)(curClock - lastClock) / CLOCKS_PER_SEC;
			float fps = frames / seconds;
			lastClock = curClock;
			printf("Generation %d starting... (%f FPS)\n", gen, fps);
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

				if (run_idx == 0 && (randFloat() <= bfStaticState.forget_rate)) {
					candidate->score = original->score;
					candidate->stats.hSpeed = original->stats.hSpeed;
					candidate->stats.angle = original->stats.angle;
					candidate->stats.x = original->stats.x;
					candidate->stats.y = original->stats.y;
					candidate->stats.z = original->stats.z;
					continue;
				}

				bf_load_dynamic_state(&bfInitialDynamicState);

				u32 frame_idx;
				for (frame_idx = 0; frame_idx < inputs->count; frame_idx++) {
					frames++;
					OSContPad *currentInput = &inputs->inputs[frame_idx];
					perturbInput(currentInput);
					updateGame(currentInput);
					if (!updateScore(candidate, frame_idx))
						break;
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

		if (gen % gen_mod == 0)
		{
			for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++) {
				printf("%d:\tscore: %f;\tangle: %d\tx: %f\ty: %f\tz: %f\thspeed: %f\n", 
					candidate_idx, survivors[candidate_idx].score, 
					survivors[candidate_idx].stats.angle, 
					survivors[candidate_idx].stats.x, 
					survivors[candidate_idx].stats.y, 
					survivors[candidate_idx].stats.z, 
					survivors[candidate_idx].stats.hSpeed);
			}
			printf("Best so far: %f\n", programState->bestSpeed);
		}
	}
}