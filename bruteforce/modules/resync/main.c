#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "bruteforce/framework/states.h"
#include "bruteforce/framework/m64.h"
#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/interface.h"
#include "bruteforce/framework/candidates.h"

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
	const char *fileContents = read_file("reference_run.json");
	Json *root = Json_create(fileContents);
	if (!root)
	{
		safePrintf("error in reference_run.json starting at:\n%s\n", Json_getError());
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
	initCamera();
	initArea();
	initMario();

	safePrintf("Loading reference run...\n");
	if (!readReference())
	{
		safePrintf("Failed to read reference run!");
		exit(-1);
	}

	safePrintf("Loading configuration...\n");
	if (!bf_init_states())
	{
		safePrintf("Failed to load configuration! Exiting...\n");
		exit(-1);
	}

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

static void updateGame(OSContPad *input)
{
	updateController(input);
	adjust_analog_stick(gPlayer1Controller);
	execute_mario_action(gMarioState->marioObj);
	if (gCurrentArea != NULL)
	{
		update_camera(gCurrentArea->camera);
	}
}

static void perturbInput(UNUSED Candidate *candidate, OSContPad *input, u32 frame_idx)
{
	if (!bfStaticState.relative_frame_numbers)
		frame_idx += bfStaticState.m64_start;

	UNUSED f64 score_diff = candidate->stats.lastScore - candidate->score;
	f64 chance = 1.0; // 1.0 / (1.0 + exp(-score_diff));
	if (randFloat() < chance * bfStaticState.perturbation_chance)
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
	f64 x_diff = (gMarioState->pos[0] - reference->x) * bfStaticState.x_weight;
	f64 y_diff = (gMarioState->pos[1] - reference->y) * bfStaticState.y_weight;
	f64 z_diff = (gMarioState->pos[2] - reference->z) * bfStaticState.z_weight;
	f64 hspeed_diff = (gMarioState->forwardVel - reference->hspeed) * bfStaticState.hspeed_weight;
	f64 angle_diff = (gMarioState->faceAngle[1] - reference->angle) * bfStaticState.angle_weight;
	return (x_diff * x_diff) + (y_diff * y_diff) + (z_diff * z_diff) + (hspeed_diff * hspeed_diff) + (angle_diff * angle_diff);
}

static void updateScore(Candidate *candidate, u32 frame_idx, boolean *abort)
{
	*abort = gMarioState->action != referenceFrames[frame_idx + bfStaticState.reference_offset].action;

	if (frame_idx == 0)
		candidate->score = 0;
	candidate->stats.lastScore = candidate->score;

	f64 frame_diff = getError(&referenceFrames[frame_idx + bfStaticState.reference_offset]);
	candidate->score -= frame_diff * frame_diff;
	candidate->stats.frame_index = frame_idx;

	if (frame_idx == candidate->sequence->count - 1)
	{
		u8 best = candidate->score > programState->bestScore;
		if (best)
		{
			hasbest = TRUE;
			programState->bestScore = candidate->score;
			clone_m64_inputs(bestInputs, candidate->sequence);
		}
	}
	// Save at most 1 run per second
	clock_t newClock = clock();
	if (hasbest && newClock - lastSaveTime > 1000)
	{
		hasbest = FALSE;
		lastSaveTime = newClock;
		output_input_sequence(bfInitialDynamicState.global_timer, bestInputs);
		safePrintf("New best: %f\n", programState->bestScore);
	}
}

void main(int argc, char *argv[])
{
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

	if (bfStaticState.per_frame_matching)
	{
		// Initial runthrough, find best input to match on every frame one by one
		u32 i;
		s8 x, y;
		BFDynamicState state;
		u32 nFramesToMatch = bfStaticState.m64_end - bfStaticState.m64_start;
		if (nReferenceFrames < nFramesToMatch)
			nFramesToMatch = nReferenceFrames;

		bf_load_dynamic_state(&bfInitialDynamicState);
		for (i = 0; i < nFramesToMatch; i++)
		{
			u32 k = i + bfStaticState.reference_offset;
			safePrintf("Matching frame %d...\n", k);

			bf_save_dynamic_state(&state);
			OSContPad cont;
			memcpy(&cont, &original_inputs->inputs[i], sizeof(OSContPad));
			f64 minError = INFINITY;
			if (bfStaticState.match_reference_buttons)
				cont.button = (u16)referenceFrames[k].buttonDown;

			// Iterate the entire set of control stick options...
			for (y = -128; y != 127; y++)
			{
				for (x = -128; x != 127; x++)
				{
					bf_load_dynamic_state(&state);
					cont.stick_x = x;
					cont.stick_y = y;
					updateGame(&cont);

					// ... and store the best result in the original input sequence
					f64 newError = getError(&referenceFrames[k]);
					if (newError < minError)
					{
						memcpy(&original_inputs->inputs[i], &cont, sizeof(OSContPad));
						minError = newError;
					}
				}
			}

			// Then perform the best found input again and continue from there
			bf_load_dynamic_state(&state);
			updateGame(&original_inputs->inputs[i]);

			safePrintf("Matched frame %d with error %f!\n", i, minError);
		}
		safePrintf("Matching frames done!\n");
	}

	bestInputs = clone_m64(original_inputs);

	output_input_sequence(bfInitialDynamicState.global_timer, original_inputs);

	// 2nd phase: optimize inputs to reduce overall error across frames
	initializeMultiProcess(original_inputs);

	if (isParentProcess())
		programState->bestScore = -INFINITY;

	bruteforce_loop_genetic(original_inputs, &updateGame, &perturbInput, &updateScore);
}