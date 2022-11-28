#ifdef BF_STATE_INCLUDE

#include "types.h"
extern struct MarioState *gMarioState;

#else

BF_STATIC_STATE(Triangles, static_tris, bfStaticState.static_tris)
BF_DYNAMIC_STATE(f32, mario_x, gMarioState->pos[0])
BF_DYNAMIC_STATE(f32, mario_y, gMarioState->pos[1])
BF_DYNAMIC_STATE(f32, mario_z, gMarioState->pos[2])
BF_DYNAMIC_STATE(f32, mario_h_speed, gMarioState->forwardVel)
BF_DYNAMIC_STATE(f32, mario_x_vel, gMarioState->vel[0])
BF_DYNAMIC_STATE(f32, mario_y_vel, gMarioState->vel[1])
BF_DYNAMIC_STATE(f32, mario_z_vel, gMarioState->vel[2])
BF_DYNAMIC_STATE(u32hex, mario_action, gMarioState->action)

#endif // _BF_STATE_DEFINITIONS_