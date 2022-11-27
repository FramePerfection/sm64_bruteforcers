
//#define BF_ENABLE_INTERACTIONS

#include <stdio.h>
#include "src/engine/surface_collision.h"
#include "src/engine/surface_load.h"
#include "src/game/mario.h"
#include "src/game/mario_step.h"
#include "src/game/level_update.h"
#include "src/game/area.h"
#include "src/game/camera.h"

extern void create_camera(struct GraphNodeCamera *gc, struct AllocOnlyPool *pool);

void main() {
	printf("Hello World");

	struct Object testObj;
	struct Controller testController;
	struct Area testArea;
	struct GraphNodeCamera testCamera;

	gCurrentArea = &testArea;

	create_camera(&testCamera, NULL);
	gCurrentArea->camera = testCamera.config.camera;

	gMarioState->marioObj = &testObj;
	gMarioState->controller = &testController;


    execute_mario_action(gMarioState->marioObj);

    if (gCurrentArea != NULL) {
        update_camera(gCurrentArea->camera);
    }
}