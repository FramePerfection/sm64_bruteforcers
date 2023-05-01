#include "bruteforce/common_states/bf_state_definitions_common.inc.c"
#include "bruteforce/common_states/bf_state_definitions_mario.inc.c"
#include "bruteforce/common_states/bf_state_definitions_camera.inc.c"
#include "bruteforce/algorithms/genetic/bf_state_definitions_genetic.inc.c"

#ifdef BF_STATE_INCLUDE

#include "types.h"
#include "scoring_method.h"
#include "perturbator.h"
extern f32 minSpeed;
extern s16 gCurrLevelNum;
extern s16 gCurrCourseNum;

#else

// scoring parameters
BF_STATIC_STATE(boolean, relative_frame_numbers, bfStaticState.relative_frame_numbers, "If set, frame numbers will be interpreted as relative to 'm64_start'.")
BF_STATIC_STATE(boolean, score_on_last_frame, bfStaticState.score_on_last_frame, "If set, scoring is only applied on the last frame. Should be set per default.")
BF_STATIC_STATE(ScoringMethods, scoring_methods, bfStaticState.scoring_methods, "A set of scoring methods that define the the bruteforcing goal.")
BF_STATIC_STATE(Perturbators, perturbators, bfStaticState.perturbators, "A set of perturbators that each can perturb the input in specific ways on a given range of frames.")
__NL__

BF_STATIC_STATE(s16, level_num, gCurrLevelNum, "This should not be declared where it is, and it's the current level number lol.")
BF_STATIC_STATE(s16, course_num, gCurrCourseNum, "This should not be declared where it is, and it's the current course number lol.")

// World triangles
BF_STATIC_STATE(Triangles, dynamic_tris, bfStaticState.dynamic_tris, "WIP: A set of triangles that represent some dynamic geometry relevant to the bruteforcing task at hand.")
BF_STATIC_STATE(Triangles, static_tris, bfStaticState.static_tris, "The set of triangles composing the level's static geometry.")
__NL__

BF_DYNAMIC_STATE(u16, area_terrain_type, gMarioState->area->terrainType, "The area's terrain type as defined by the level script, such as normal, snowy, slippery")

#endif