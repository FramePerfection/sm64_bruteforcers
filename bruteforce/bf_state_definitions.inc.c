#include "bf_state_definitions_camera.inc.c"

#ifdef BF_STATE_INCLUDE

#include "types.h"
extern struct MarioState *gMarioState;

#else

BF_STATIC_STATE(Triangles, static_tris, bfStaticState.static_tris)

// Mario
BF_DYNAMIC_VEC3(mario, gMarioState->pos)
BF_DYNAMIC_VEC3(mario_vel, gMarioState->vel)
BF_DYNAMIC_STATE(f32, mario_h_speed, gMarioState->forwardVel)
BF_DYNAMIC_STATE(u32, mario_action, gMarioState->action)
BF_DYNAMIC_STATE(u32, mario_previous_action, gMarioState->prevAction)
BF_DYNAMIC_STATE(s16, mario_yaw_facing, gMarioState->faceAngle[1])

#endif