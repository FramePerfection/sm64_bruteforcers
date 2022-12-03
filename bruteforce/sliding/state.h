#ifndef STATE_H
#define STATE_H

#include <PR/ultratypes.h>
#include "src/game/area.h"
#include "src/game/camera.h"
#include "src/game/game_init.h"
#include "src/game/mario.h"
#include "src/game/level_update.h"

extern struct GraphNodeCamera camera;
extern struct Object marioObj;
extern struct Area area;
extern struct MarioBodyState marioBodyState;
extern struct Controller testController;

extern f32 minSpeed;

typedef struct {
	f32 hSpeed;
	s16 angle;
	f32 x;
	f32 y;
	f32 z;
} CandidateStats;

#endif // STATE_H