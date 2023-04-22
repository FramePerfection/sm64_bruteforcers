#include "bruteforce/common_states/bf_state_definitions_common.inc.c"
#include "bruteforce/common_states/bf_state_definitions_simple_perturbate.inc.c"
#include "bruteforce/algorithms/genetic/bf_state_definitions_genetic.inc.c"

#ifdef BF_STATE_INCLUDE

#include "types.h"
extern struct MarioState *gMarioState;
extern s16 sSwimStrength;
extern s16 *gEnvironmentRegions;

#else

// TODO: Verify this documentation
BF_STATIC_STATE(EnvironmentRegions, environment_regions, gEnvironmentRegions,\
"The environment regions as a list of s16 values.\n\
[0]: number of environment boxes 'n'\n\
[n+1]: n-th low x coordinate\n\
[n+2]: n-th low z coordinate\n\
[n+3]: n-th high x coordinate\n\
[n+4]: n-th high z coordinate\n\
[n+5]: n-th y coordinate\n\
[n+6]: n-th type??")


// scoring parameters
BF_STATIC_STATE(u32, swimming_action, bfStaticState.swimming_action, "TODO: document this")
BF_STATIC_STATE(u32, swimming_start_frame, bfStaticState.swimming_start_frame, "TODO: document this")
BF_STATIC_STATE(u32, target_action, bfStaticState.target_action, "TODO: document this")
BF_STATIC_STATE(u32, scoring_frame, bfStaticState.scoring_frame, "TODO: document this")
BF_STATIC_STATE(s16, target_angle, bfStaticState.target_angle, "TODO: document this")
BF_STATIC_STATE(s16, target_angle_margin, bfStaticState.target_angle_margin, "TODO: document this")
__NL__

// World triangles and configuration
BF_STATIC_STATE(Triangles, static_tris, bfStaticState.static_tris, "TODO: document this")
BF_STATIC_STATE(HitBoxes, hitboxes, bfStaticState.hitboxes, "TODO: document this")
__NL__

// Mario
BF_DYNAMIC_STATE(s16, mario_health, gMarioState->health, "TODO: document this")
BF_DYNAMIC_STATE(s16, swim_strength, sSwimStrength, "TODO: document this")
BF_DYNAMIC_VEC3(mario, gMarioState->pos, "TODO: document this")
BF_DYNAMIC_VEC3(mario_vel, gMarioState->vel, "TODO: document this")
BF_DYNAMIC_STATE(f32, mario_h_speed, gMarioState->forwardVel, "TODO: document this")
BF_DYNAMIC_STATE(u32, mario_action, gMarioState->action, "TODO: document this")
BF_DYNAMIC_STATE(u16, mario_action_timer, gMarioState->actionTimer, "TODO: document this")
BF_DYNAMIC_STATE(u32, mario_action_arg, gMarioState->actionArg, "TODO: document this")
BF_DYNAMIC_STATE(u8, mario_wallkick_timer, gMarioState->wallKickTimer, "TODO: document this")
BF_DYNAMIC_STATE(u32, mario_previous_action, gMarioState->prevAction, "TODO: document this")
BF_DYNAMIC_STATE(s16, mario_yaw_facing, gMarioState->faceAngle[1], "TODO: document this")
BF_DYNAMIC_STATE(s16, pitch, gMarioState->faceAngle[0], "TODO: document this")
BF_DYNAMIC_STATE(s16, mario_slide_yaw, gMarioState->slideYaw, "TODO: document this")
BF_DYNAMIC_STATE(s16, yaw_velocity, gMarioState->angleVel[1], "TODO: document this")
BF_DYNAMIC_STATE(s16, pitch_velocity, gMarioState->angleVel[0], "TODO: document this")

#endif