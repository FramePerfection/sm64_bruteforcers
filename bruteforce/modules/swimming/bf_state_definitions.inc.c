#include "bruteforce/common_states/bf_state_definitions_camera.inc.c"
#include "bruteforce/common_states/bf_state_definitions_common.inc.c"

#ifdef BF_STATE_INCLUDE

#include "types.h"
extern struct MarioState *gMarioState;
extern s16 sSwimStrength;
extern s16 *gEnvironmentRegions;
extern struct Controller testController;
extern f32 minSpeed;

#else

BF_STATIC_STATE(EnvironmentRegions, environment_regions, gEnvironmentRegions)
// scoring parameters
BF_STATIC_STATE(u32, swimming_action, bfStaticState.swimming_action)
BF_STATIC_STATE(u32, swimming_start_frame, bfStaticState.swimming_start_frame)
BF_STATIC_STATE(u32, target_action, bfStaticState.target_action)
BF_STATIC_STATE(u32, scoring_frame, bfStaticState.scoring_frame)
BF_STATIC_STATE(s16, target_angle, bfStaticState.target_angle)
BF_STATIC_STATE(s16, target_angle_margin, bfStaticState.target_angle_margin)
BF_STATIC_STATE(s16, min_speed, minSpeed)
__NL__

// World triangles
BF_STATIC_STATE(Triangles, static_tris, bfStaticState.static_tris)
__NL__

BF_DYNAMIC_STATE(u16, initial_button_state, testController.buttonDown)
__NL__

// Mario
BF_DYNAMIC_STATE(s16, mario_health, gMarioState->health)
BF_DYNAMIC_STATE(s16, swim_strength, sSwimStrength)
BF_DYNAMIC_VEC3(mario, gMarioState->pos)
BF_DYNAMIC_VEC3(mario_vel, gMarioState->vel)
BF_DYNAMIC_STATE(f32, mario_h_speed, gMarioState->forwardVel)
BF_DYNAMIC_STATE(u32, mario_action, gMarioState->action)
BF_DYNAMIC_STATE(u16, mario_action_timer, gMarioState->actionTimer)
BF_DYNAMIC_STATE(u8, mario_wallkick_timer, gMarioState->wallKickTimer)
BF_DYNAMIC_STATE(u32, mario_previous_action, gMarioState->prevAction)
BF_DYNAMIC_STATE(s16, mario_yaw_facing, gMarioState->faceAngle[1])
BF_DYNAMIC_STATE(s16, pitch, gMarioState->faceAngle[0])
BF_DYNAMIC_STATE(s16, mario_slide_yaw, gMarioState->slideYaw)
BF_DYNAMIC_STATE(s16, yaw_velocity, gMarioState->angleVel[1])
BF_DYNAMIC_STATE(s16, pitch_velocity, gMarioState->angleVel[0])

#endif