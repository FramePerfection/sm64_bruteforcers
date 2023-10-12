
#ifdef BF_STATE_INCLUDE

#include "types.h"
extern struct MarioState *gMarioState;
extern s16 sSwimStrength;
extern u8 gSpecialTripleJump;

#else

// Mario
BF_DYNAMIC_STATE(s16, mario_health, gMarioState->health, "Mario's health, between 0 and 0x880")
BF_DYNAMIC_VEC3(mario, gMarioState->pos, "Mario's position.")
BF_DYNAMIC_VEC3(mario_vel, gMarioState->vel, "Mario's velocity vector.")
BF_DYNAMIC_STATE(f32, mario_slide_vel_x, gMarioState->slideVelX, "The x component of Mario's sliding velocity vector")
BF_DYNAMIC_STATE(f32, mario_slide_vel_z, gMarioState->slideVelZ, "The z component of Mario's sliding velocity vector")
BF_DYNAMIC_STATE(f32, mario_h_speed, gMarioState->forwardVel, "Mario's forward speed.")
BF_DYNAMIC_STATE(u32, mario_action, gMarioState->action, "Mario's current action, represented as a hexadecimal 32 bit integer.")
BF_DYNAMIC_STATE(u16, mario_action_state, gMarioState->actionState, "Mario's action state.")
BF_DYNAMIC_STATE(u16, mario_action_timer, gMarioState->actionTimer, "Mario's action timer.")
BF_DYNAMIC_STATE(u8, mario_wallkick_timer, gMarioState->wallKickTimer, "Mario's wallkick timer.")
BF_DYNAMIC_STATE(u32, mario_previous_action, gMarioState->prevAction, "Mario's previous action, represented as a hexadecimal 32 bit integer.")
BF_DYNAMIC_STATE(s16, mario_yaw_facing, gMarioState->faceAngle[1], "Mario's facing angle.")
BF_DYNAMIC_STATE(s16, mario_pitch_facing, gMarioState->faceAngle[0], "Mario's facing pitch angle.")
BF_DYNAMIC_STATE(s16, mario_yaw_velocity, gMarioState->angleVel[1], "Mario's yaw velocity.")
BF_DYNAMIC_STATE(s16, mario_pitch_velocity, gMarioState->angleVel[0], "Mario's pitch velocity.")
BF_DYNAMIC_STATE(s16, mario_slide_yaw, gMarioState->slideYaw, "Mario's sliding angle.")
BF_DYNAMIC_STATE(u32, mario_flags, gMarioState->flags, "Mario flags, such as cap state. See sm64.h line 115 for details.")
BF_DYNAMIC_STATE(u16, mario_cap_timer, gMarioState->capTimer, "Mario's cap timer.")
BF_STATIC_STATE(boolean, special_triple_jump, gSpecialTripleJump, "Special triple jump enabled.")
__NL__

#endif