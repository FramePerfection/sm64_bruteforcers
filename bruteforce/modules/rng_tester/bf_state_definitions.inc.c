#include "bruteforce/common_states/bf_state_definitions_common.inc.c"
#include "bruteforce/common_states/bf_state_definitions_mario.inc.c"
#include "bruteforce/common_states/bf_state_definitions_camera.inc.c"

#ifdef BF_STATE_INCLUDE

#include "types.h"
#include "src/game/area.h"
#include "bruteforce/framework/objects_interaction/object_utils.h"
extern struct MarioState *gMarioState;
extern struct SpawnInfo *gMarioSpawnInfo;
extern s16 gCurrLevelNum;
extern s16 gCurrCourseNum;
extern u16 gRandomSeed16;
extern u16 rng_index;

#else

// Options
BF_STATIC_STATE(boolean, from_level_load, bfStaticState.from_level_load, "If set, rng will not be reset after initially updating loaded objects.")
BF_STATIC_STATE(u16, from_level_load_rng_index, bfStaticState.from_level_load_rng_index, "If from_level_load is set, this should be the rng index during the star select screen.")
BF_STATIC_STATE(boolean, debug_desyncs, bfStaticState.debug_desyncs, "If set, the input run is played back once and 'debug_output.json' is written, capturing the dynamic state on each frame.")
BF_STATIC_STATE(boolean, update_objects, bfStaticState.update_objects, "Enables logic to update objects.")
BF_STATIC_STATE(boolean, relative_frame_numbers, bfStaticState.relative_frame_numbers, "If set, frame numbers will be interpreted as relative to 'm64_start'.")
__NL__

BF_STATIC_STATE(s16, level_num, gCurrLevelNum, "TODO: lol")
BF_STATIC_STATE(s16, course_num, gCurrCourseNum, "TODO: lel")

// World triangles
BF_STATIC_STATE(Triangles, static_tris, bfStaticState.static_tris, "The set of triangles composing the level's static geometry.")
__NL__

// Object data
BF_STATIC_STATE(f32, mario_spawn_x, gMarioSpawnInfo->startPos[0], "gMarioSpawnInfo->startPos[0]")
BF_STATIC_STATE(f32, mario_spawn_y, gMarioSpawnInfo->startPos[1], "gMarioSpawnInfo->startPos[1]")
BF_STATIC_STATE(f32, mario_spawn_z, gMarioSpawnInfo->startPos[2], "gMarioSpawnInfo->startPos[2]")
BF_STATIC_STATE(BehaviorScriptArrayArray, behavior_scripts, bfStaticState.behavior_scripts, "all behavior scripts required to do stuff")
BF_STATIC_STATE(ObjectTrianglesArray, dynamic_object_tris, bfStaticState.dynamic_object_tris, \
"A set mapping from 'original segmented collision pointer' to triangle data. __NL__ \
Used to map LOAD_COLLISION_DATA arguments.")
BF_STATIC_STATE(BfObjectStateArray, object_states, bfStaticState.object_states, "all object information")
__NL__

// Misc data
BF_DYNAMIC_STATE(u16, rng_value, gRandomSeed16, "The initial rng value.")
BF_DYNAMIC_STATE(u16, rng_index, rng_index, "The computed rng index. Only used for debug output.")

#endif