
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

void print_vec3f(Vec3f *vec) {
	printf("\t%f;\t%f;\t%f", (*vec)[0], (*vec)[1], (*vec)[2]);
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
	InputSequence *inputs;
	if (!read_m64_from_file(bfStaticState.m64_input, bfStaticState.m64_start, bfStaticState.m64_count, &inputs))
	{
		printf("Failed to load m64! Exiting...\n");
		return;
	}

	struct Object testObj;
	struct Controller testController;
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
	
	u32 k;
	for (k = 0; k < 2; k++) {
		printf("spawning...\n");

		bf_load_dynamic_state(&bfInitialDynamicState);

		u32 prev_action = gMarioState->action;
		for (i = 0; i < inputs->count; i++) {
			printf("Frame %d:\t", inputs->offset + i);
			
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
			if (prev_action != gMarioState->action)
				printf("Action transition: %x -> %x\n", prev_action, gMarioState->action);
			prev_action = gMarioState->action;
			
			printf("Mario:");
			print_vec3f(&gMarioState->pos);
			printf("\tCamera:");
			print_vec3f(&gCurrentArea->camera->pos);
			printf("\n");
		}
	}
}