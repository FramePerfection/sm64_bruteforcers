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

extern Vec3f last_q_step, last_q_step2;

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

	srand(bfStaticState.rnd_seed);
	
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
	if (bfStaticState.max_perturbation > 0) {
		u16 perturb = (u16)(bfStaticState.max_perturbation);
		u8 perturbation_x = (rand() % (2 * perturb) - perturb);
		u8 perturbation_y = (rand() % (2 * perturb) - perturb);
		input->stick_x = (s8)(input->stick_x + perturbation_x);
		input->stick_y = (s8)(input->stick_y + perturbation_y);
	}
}

void updateScore(Candidate *candidate, u32 frame_idx) {
	if (frame_idx == 8) {
		f64 dist = last_q_step[0] * bfStaticState.plane_nx + last_q_step[2] * bfStaticState.plane_nz + bfStaticState.plane_d - 50.0;
		f64 dist2 = last_q_step2[0] * bfStaticState.plane_nx + last_q_step2[2] * bfStaticState.plane_nz + bfStaticState.plane_d - 50.0;
		f64 score = (dist * dist + dist2 * dist2);
		u8 best = gMarioStates->forwardVel > minSpeed;
		if (!best)
			score += minSpeed - gMarioState->forwardVel;
		candidate->stats.hSpeed = gMarioStates->forwardVel;
		candidate->score = score;

		if (gMarioState->faceAngle[1] != bfStaticState.approach_angle && gMarioState->faceAngle[1] != -bfStaticState.approach_angle)
		{
			printf("Found one: %d, %f", gMarioState->faceAngle[1], gMarioState->forwardVel);
			if (best) {
				save_to_m64_file(bfStaticState.m64_input, bfStaticState.m64_output, candidate->sequence);
				printf("\t(new best!)");
				minSpeed = gMarioState->forwardVel;
			}
			printf("\n");
		}
	}
}

void main() {
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
	Candidate **best;
	initCandidates(original_inputs, &survivors, &best);

	clock_t lastClock = clock();
	u32 gen_mod = bfStaticState.print_interval;
	if (gen_mod == 0)
		gen_mod = 100;

	u32 gen;
	for (gen = 0; gen < bfStaticState.max_generations; gen++) {
		if (gen % gen_mod == 0)
		{
			clock_t curClock = clock();
			float seconds = (float)(curClock - lastClock) / CLOCKS_PER_SEC;
			float fps = gen_mod * original_inputs->count * bfStaticState.runs_per_survivor * bfStaticState.survivors_per_generation / seconds;
			lastClock = curClock;
			printf("Generation %d starting... (minSpeed %f, %f FPS)\n", gen, minSpeed, fps);
		}

		// perform all runs
		u32 candidate_idx;
		for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++) {
			best[candidate_idx] = NULL;

			u32 run_idx;
			for (run_idx = 0; run_idx < bfStaticState.runs_per_survivor; run_idx++) {
				Candidate *candidate = &survivors[candidate_idx].children[run_idx];
				Candidate *original = &survivors[candidate_idx];
				
				InputSequence *inputs = candidate->sequence;
				clone_m64_inputs(inputs, original->sequence);

				if (run_idx == 0 && (rand() % 100 <= 98)) {
					candidate->score = original->score;
					candidate->stats.hSpeed = original->stats.hSpeed;
					continue;
				}

				bf_load_dynamic_state(&bfInitialDynamicState);

				u32 frame_idx;
				for (frame_idx = 0; frame_idx < inputs->count; frame_idx++) {
					OSContPad *currentInput = &inputs->inputs[frame_idx];
					perturbInput(currentInput);
					updateGame(currentInput);
					updateScore(candidate, frame_idx);
				}
			}
		}
		
		// sort by scoring
		updateBestCandidates(survivors, best);

		if (gen % gen_mod == 0)
			for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
				printf("%d:\t%a;\t%f\n", candidate_idx, survivors[candidate_idx].score, survivors[candidate_idx].stats.hSpeed);
	}
	save_to_m64_file(bfStaticState.m64_input, bfStaticState.m64_output, best[0]->sequence);
}