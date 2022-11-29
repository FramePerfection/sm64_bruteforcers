
//#define BF_ENABLE_INTERACTIONS

#include <stdio.h>
#include "src/engine/surface_collision.h"
#include "src/engine/surface_load.h"
#include "src/game/mario.h"
#include "src/game/mario_step.h"
#include "src/game/level_update.h"
#include "src/game/area.h"
#include "src/game/camera.h"
#include "src/game/game_init.h"
#include "sm64.h"
#include "misc_util.inc.c"
#include "bruteforce/bf_states.h"
#include "bruteforce/m64.h"
#include <stdlib.h>

typedef struct Candidate_s {
	InputSequence *sequence;
	struct Candidate_s *children;
	f64 score;
	f32 hSpeed;
} Candidate;

void print_vec3f(Vec3f *vec) {
	printf("\t%f;\t%f;\t%f", (*vec)[0], (*vec)[1], (*vec)[2]);
}

struct Controller testController;
extern Vec3f last_q_step, last_q_step2;

void insertSorted(Candidate **best, Candidate *new) {
	if (best[0] == NULL) {
		best[0] = new;
		return;
	}
	
	s32 rank;
	for (rank = bfStaticState.survivors_per_generation; rank > 0; rank--)
		if (best[rank-1] != NULL && best[rank-1]->score <= new->score)
			break;

	if (rank == bfStaticState.survivors_per_generation)
		return;

	s32 move_idx;
	for (move_idx = bfStaticState.survivors_per_generation - 1; move_idx > rank; move_idx--) {
		best[move_idx] = best[move_idx - 1];
	}
	best[rank] = new;
}

void main() {
	u32 i;

	struct GraphNodeCamera testCamera;
	gCamera = &testCamera;

	printf("Running Bruteforcer...\n");
	printf("Loading configuration...\n");
	if (!bf_init_states()) {
		printf("Failed to load configuration! Exiting...\n");
		return;
	}

	printf("Loading m64...");
	InputSequence *original_inputs;
	if (!read_m64_from_file(bfStaticState.m64_input, bfStaticState.m64_start, bfStaticState.m64_count, &original_inputs))
	{
		printf("Failed to load m64! Exiting...\n");
		return;
	}

	Candidate *survivors = malloc(sizeof(Candidate) * bfStaticState.survivors_per_generation);
	Candidate **best = malloc(sizeof(Candidate*) * bfStaticState.survivors_per_generation);
	for (i = 0; i < bfStaticState.survivors_per_generation; i++) {
		survivors[i].score = INFINITY;
		survivors[i].children = malloc(sizeof(Candidate) * bfStaticState.runs_per_survivor);
		u32 k;
		for (k = 0; k < bfStaticState.runs_per_survivor; k++)
			survivors[i].children[k].sequence = clone_m64(original_inputs);
		survivors[i].sequence = clone_m64(original_inputs);
		best[i] = &survivors[i];
	}

	struct Object testObj;
	struct Area testArea;
	struct MarioBodyState testBodyState;

	gCurrentArea = &testArea;

	init_graph_node_object(NULL, &testObj, NULL, gVec3fZero, gVec3sZero, gVec3fOne);
	
	clear_static_surfaces();
	for (i = 0; i < bfStaticState.static_tris.data_size; i++) {
		Triangle t = bfStaticState.static_tris.data[i];
		struct Surface *testFloor = gen_surface(t.x1, t.y1, t.z1, t.x2, t.y2, t.z2, t.x3, t.y3, t.z3);
		if (testFloor != NULL)
			add_surface(testFloor, FALSE);
		else
			printf("found degenerate triangle: (%d,%d,%d),(%d,%d,%d),(%d,%d,%d)\n", t.x1, t.y1, t.z1, t.x2, t.y2, t.z2, t.x3, t.y3, t.z3);
	}

	testCamera.config.mode = CAMERA_MODE_8_DIRECTIONS;
	gLakituState.mode = testCamera.config.mode;
	create_camera(&testCamera, NULL);
	gCurrentArea->camera = testCamera.config.camera;

	vec3f_set(gLakituState.goalPos, 25.0f, 0.5f, 23.0f);

	gMarioState->marioObj = &testObj;
	gMarioState->marioBodyState = &testBodyState;
	gMarioState->statusForCamera = &gPlayerCameraState[0];

	gMarioState->controller = &testController;
	gMarioState->area = &testArea;
	
	// Still required to initialize something?
	execute_mario_action(gMarioState->marioObj);
	update_camera(gCurrentArea->camera);

	srand(bfStaticState.rnd_seed);
	
	u32 gen;
	for (gen = 0; gen < bfStaticState.max_generations; gen++) {
		printf("Generation %d starting...\n", gen);

		// perform all runs
		u32 candidate_idx;
		for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++) {
			best[candidate_idx] = NULL;

			u32 run_idx;
			for (run_idx = 0; run_idx < bfStaticState.runs_per_survivor; run_idx++) {
				bf_load_dynamic_state(&bfInitialDynamicState);
				u8 perturb_run = run_idx > 0 || (rand() % 100 > 98);
				
				InputSequence *inputs = survivors[candidate_idx].children[run_idx].sequence;
				clone_m64_inputs(inputs, survivors[candidate_idx].sequence);

				for (i = 0; i < inputs->count; i++) {
					if (i == 9) {
						f64 dist = last_q_step[0] * bfStaticState.plane_nx + last_q_step[2] * bfStaticState.plane_nz + bfStaticState.plane_d - 50.0;
						f64 dist2 = last_q_step2[0] * bfStaticState.plane_nx + last_q_step2[2] * bfStaticState.plane_nz + bfStaticState.plane_d - 50.0;
						f64 score = dist * dist + dist2 * dist2;
						if (gMarioStates->forwardVel < 40.0f)
							score = INFINITY;
						survivors[candidate_idx].children[run_idx].hSpeed = gMarioStates->forwardVel;
						survivors[candidate_idx].children[run_idx].score = score;

						if (gMarioState->faceAngle[1] != 5632 && gMarioState->faceAngle[1] != -5632)
						{
							printf("Found one: %d, %a", gMarioState->faceAngle[1], gMarioState->forwardVel);
							save_to_m64_file(bfStaticState.m64_input, bfStaticState.m64_output, inputs);
							return;
						}
					}

					if (perturb_run) {
						u16 perturb = (u16)(bfStaticState.max_perturbation);
						u8 perturbation_x = (rand() % (2 * perturb) - perturb);
						u8 perturbation_y = (rand() % (2 * perturb) - perturb);
						inputs->inputs[i].stick_x = (s8)(inputs->inputs[i].stick_x + perturbation_x);
						inputs->inputs[i].stick_y = (s8)(inputs->inputs[i].stick_y + perturbation_y);
					}

					testController.rawStickX = inputs->inputs[i].stick_x;
					testController.rawStickY = inputs->inputs[i].stick_y;
					testController.buttonPressed = inputs->inputs[i].button
												& (inputs->inputs[i].button ^ testController.buttonDown);
					// 0.5x A presses are a good meme
					testController.buttonDown = inputs->inputs[i].button;

					adjust_analog_stick(&testController);

					execute_mario_action(gMarioState->marioObj);

					if (gCurrentArea != NULL) {
						update_camera(gCurrentArea->camera);
					}
				}
			}
		}
		
		// sort by scoring
		for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++) {
			u32 run_idx = 0;
			for (run_idx = 0; run_idx < bfStaticState.runs_per_survivor; run_idx++)
				insertSorted(best, &(survivors[candidate_idx].children[run_idx]));
		}
		for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++) {
			clone_m64_inputs(survivors[candidate_idx].sequence, best[candidate_idx]->sequence);
			survivors[candidate_idx].score = best[candidate_idx]->score;
			survivors[candidate_idx].hSpeed = best[candidate_idx]->hSpeed;
			printf("%d:\t%a;\t%f\n", candidate_idx, survivors[candidate_idx].score, survivors[candidate_idx].hSpeed);
		}
	}
	save_to_m64_file(bfStaticState.m64_input, bfStaticState.m64_output, best[0]->sequence);
	printf("Random Seed: %d", bfStaticState.rnd_seed);
}