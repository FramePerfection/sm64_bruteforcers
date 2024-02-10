#include "bruteforce/common_states/bf_state_definitions_common.inc.c"
#include "bruteforce/common_states/bf_state_definitions_mario.inc.c"
#include "bruteforce/common_states/bf_state_definitions_camera.inc.c"
#include "bruteforce/common_states/bf_state_definitions_simple_perturbate.inc.c"
#include "bruteforce/algorithms/genetic/bf_state_definitions_genetic.inc.c"

#ifdef BF_STATE_INCLUDE

#include "types.h"
extern struct MarioState *gMarioState;
extern s16 sSwimStrength;
extern s16 gCurrLevelNum;
extern s16 gCurrCourseNum;

#else

// options
BF_STATIC_STATE(boolean, auto_detect_reference_offset, bfStaticState.auto_detect_reference_offset, "If set, the last reference frame that scores the smallest difference to the initial state will be chosen as the reference start.")
BF_STATIC_STATE(boolean, relative_frame_numbers, bfStaticState.relative_frame_numbers, "If set, frame numbers will be interpreted as relative to 'm64_start'.")
BF_STATIC_STATE(boolean, match_reference_buttons, bfStaticState.match_reference_buttons, "If set, buttons will be pressed exactly as in the reference run, ignoring the base run's button inputs.")
BF_STATIC_STATE(u32, reference_offset, bfStaticState.reference_offset, "The offset into the reference run to start comparing at. TODO: example")
BF_STATIC_STATE(u32, num_fine_tuning_frames, bfStaticState.num_fine_tuning_frames, "The number of frames to fine-tune with a genetic algorithm at the end of the per-frame matched result.")

// scoring parameters
BF_CONTROL_STATE(f64, x_weight, "The weight of Mario's x-position matching.")
BF_CONTROL_STATE(f64, y_weight, "The weight of Mario's y-position matching.")
BF_CONTROL_STATE(f64, z_weight, "The weight of Mario's z-position matching.")
BF_CONTROL_STATE(f64, angle_weight, "The weight of Mario's angle difference in angular units. Should be drastically lower than x/y/z weights.")
BF_CONTROL_STATE(f64, hspeed_weight, "The weight of Mario's horizontal speed matching.")
__NL__

BF_STATIC_STATE(s16, level_num, gCurrLevelNum, "TODO: lol")
BF_STATIC_STATE(s16, course_num, gCurrCourseNum, "TODO: lel")

// World triangles
BF_STATIC_STATE(Triangles, dynamic_tris, bfStaticState.dynamic_tris, "WIP: A set of triangles that represent some dynamic geometry relevant to the bruteforcing task at hand.")
BF_STATIC_STATE(Triangles, static_tris, bfStaticState.static_tris, "The set of triangles composing the level's static geometry.")
__NL__

#endif