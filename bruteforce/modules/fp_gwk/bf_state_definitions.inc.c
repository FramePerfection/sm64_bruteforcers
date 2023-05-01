#include "bruteforce/common_states/bf_state_definitions_common.inc.c"
#include "bruteforce/common_states/bf_state_definitions_mario.inc.c"
#include "bruteforce/common_states/bf_state_definitions_camera.inc.c"
#include "bruteforce/common_states/bf_state_definitions_simple_perturbate.inc.c"
#include "bruteforce/algorithms/genetic/bf_state_definitions_genetic.inc.c"

#ifdef BF_STATE_INCLUDE

#include "types.h"
extern struct MarioState *gMarioState;
extern f32 minSpeed;

#else

// Scoring parameters
BF_STATIC_STATE(u32, scoring_frame, bfStaticState.scoring_frame, "The frame on which the fpgwk should be performed")
BF_STATIC_STATE(f32, plane_nx, bfStaticState.plane_nx, "The target reference wall's normal x value")
BF_STATIC_STATE(f32, plane_nz, bfStaticState.plane_nz, "The target reference wall's normal z value")
BF_STATIC_STATE(f32, plane_d, bfStaticState.plane_d, "The target reference wall's plane offset")
BF_STATIC_STATE(s16, gwk_angle, bfStaticState.gwk_angle, "The angle at which Mario will bonk when successfully performing a glitchy bonk")
BF_STATIC_STATE(f32, minimum_h_speed, minSpeed, \
"The minimum speed at which to try to perform glitchy bonks.\n\
Attempts may go a little below, but will quickly approach this value if Mario's initial speed is far above.\n\
Should be a little less than the base run's h-speed to get started.")
__NL__

// World triangles
BF_STATIC_STATE(Triangles, static_tris, bfStaticState.static_tris, "The set of triangles composing the level's static geometry.")
__NL__

#endif