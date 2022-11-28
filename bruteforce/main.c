
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

void print_vec3f(Vec3f *vec) {
	printf("\t%f;\t%f;\t%f", (*vec)[0], (*vec)[1], (*vec)[2]);
}

void main() {
	printf("Hello World\n");

	struct Object testObj;
	struct Controller testController;
	struct Area testArea;
	struct GraphNodeCamera testCamera;
	struct MarioBodyState testBodyState;
	struct Surface *testFloor = gen_surface(
			 100, 	-800, -100,
			-100, 	-800, -100,
			 0, 	-800,  100
			);
	struct Surface *testWall = gen_surface(
			 100, 	-800, 	100,
			-100, 	-800, 	100,
			 0, 	 800, 	100
			);

	gCurrentArea = &testArea;

	init_graph_node_object(NULL, &testObj, NULL, gVec3fZero, gVec3sZero, gVec3fOne);
	
	clear_static_surfaces();
	add_surface(testFloor, FALSE);
	add_surface(testWall, FALSE);

	testCamera.config.mode = CAMERA_MODE_8_DIRECTIONS;
	gLakituState.mode = testCamera.config.mode;
	create_camera(&testCamera, NULL);
	gCurrentArea->camera = testCamera.config.camera;

	vec3f_set(gLakituState.goalPos, 25.0f, 0.5f, 23.0f);

	gMarioState->marioObj = &testObj;
	gMarioState->marioBodyState = &testBodyState;
	gMarioState->statusForCamera = &gPlayerCameraState[0];

	gMarioState->controller = &testController;
	gMarioState->action = ACT_WALL_KICK_AIR;
	gMarioState->area = &testArea;
	gMarioState->vel[1] = 62.0f;
	gMarioState->forwardVel = 40.0f;
	
	testController.rawStickX = 127;
	adjust_analog_stick(&testController);

	u32 i;
	u32 prev_action = gMarioState->action;
	for (i = 0; i < 100; i++) {
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