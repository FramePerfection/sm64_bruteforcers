#include "bruteforce/common_states/bf_state_definitions_common.inc.c"
#include "bruteforce/common_states/bf_state_definitions_mario.inc.c"
#include "bruteforce/common_states/bf_state_definitions_camera.inc.c"

#ifdef BF_STATE_INCLUDE

#include "types.h"
#include "bruteforce/framework/objects_interaction/object_utils.h"
extern struct MarioState *gMarioState;
extern s16 gCurrLevelNum;
extern s16 gCurrCourseNum;
extern u16 gRandomSeed16;

#else

// Options
BF_STATIC_STATE(boolean, update_objects, bfStaticState.update_objects, "Enables logic to update objects.")
BF_STATIC_STATE(boolean, relative_frame_numbers, bfStaticState.relative_frame_numbers, "If set, frame numbers will be interpreted as relative to 'm64_start'.")
__NL__

BF_STATIC_STATE(s16, level_num, gCurrLevelNum, "TODO: lol")
BF_STATIC_STATE(s16, course_num, gCurrCourseNum, "TODO: lel")

// World triangles
BF_STATIC_STATE(Triangles, static_tris, bfStaticState.static_tris, "The set of triangles composing the level's static geometry.")
__NL__

// Object data
BF_STATIC_STATE(BehaviorScriptArrayArray, behavior_scripts, bfStaticState.behavior_scripts, "all behavior scripts required to do stuff")
BF_STATIC_STATE(ObjectTrianglesArray, dynamic_object_tris, bfStaticState.dynamic_object_tris, \
"A set mapping from 'original segmented collision pointer' to triangle data. __NL__ \
Used to map LOAD_COLLISION_DATA arguments.")
BF_STATIC_STATE(BfObjectStateArray, object_states, bfStaticState.object_states, "all object information")
__NL__

// Misc data
BF_STATIC_STATE(u16, rng_value, gRandomSeed16, "The initial rng value.")

#endif