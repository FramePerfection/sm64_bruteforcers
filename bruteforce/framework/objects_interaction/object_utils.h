#ifndef OBJECT_UTILS_H
#define OBJECT_UTILS_H

#include "include/types.h"
#include "bruteforce/framework/readers.h"
#include "src/game/spawn_object.h"
#include "src/game/object_list_processor.h"
#include "src/engine/math_util.h"
#include "src/engine/surface_load.h"

extern BehaviorScript bhvMario[];
extern struct MarioState *gMarioState;
extern struct Object *gMarioObject;

typedef struct ObjectTriangles_s {
	u32 overrides;
	s16 *data;
} ObjectTriangles;

#include "json_structs.inc.h"

void bf_remap_behavior_scripts(BehaviorScriptArrayArray behaviour_scripts, ObjectTrianglesArray object_triangles);
void bf_reset_objects(BehaviorScriptArrayArray behavior_sciprts, BfObjectStateArray object_states);

#endif