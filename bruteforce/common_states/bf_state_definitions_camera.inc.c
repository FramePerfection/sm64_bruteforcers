#ifdef BF_STATE_INCLUDE

#include "types.h"
#include "src/game/camera.h"

extern Vec3f sOldPosition;
extern Vec3f sOldFocus;
extern struct TransitionInfo sModeTransition;
extern s16 sAvoidYawVel;
extern s16 sCameraYawAfterDoorCutscene;
extern s16 sSelectionFlags;
extern s16 s2ndRotateFlags;
extern u16 sCButtonsPressed;
extern s16 sAreaYaw;
extern s16 sAreaYawChange;
extern s16 sLakituDist;
extern s16 sLakituPitch;
extern f32 sZoomAmount;
extern s16 sCSideButtonYaw;
extern f32 sZeroZoomAmount;
extern s16 sCUpCameraPitch;
extern s16 sModeOffsetYaw;
extern s16 s8DirModeBaseYaw;
extern s16 s8DirModeYawOffset;
extern f32 sPanDistance;
extern f32 sCannonYOffset;
extern struct ModeTransitionInfo sModeInfo;
extern Vec3f sCastleEntranceOffset;
extern s16 sStatusFlags;
extern struct Camera *gCamera;

#else

// Camera data
BF_DYNAMIC_VEC3(camera_old_position, sOldPosition, "the camera's position on the last frame")
BF_DYNAMIC_VEC3(camera_old_focus, sOldFocus, "the camera's look at point of the last frame")
__NL__

BF_DYNAMIC_STATE(s16, camera_state_camera_event, gPlayerCameraState[0].cameraEvent, "The camera's current event, 0 if none")
BF_DYNAMIC_STATE(u32, camera_state_usedObj, gPlayerCameraState[0].usedObj, "Mario's used object, as reflected by his state for camera")
__NL__

BF_DYNAMIC_STATE(s16, camera_transition_pos_pitch, sModeTransition.posPitch, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_transition_pos_yaw, sModeTransition.posYaw, "TODO: document this")
BF_DYNAMIC_STATE(f32, camera_transition_pos_dist, sModeTransition.posDist, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_transition_foc_pitch, sModeTransition.focPitch, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_transition_foc_yaw, sModeTransition.focYaw, "TODO: document this")
BF_DYNAMIC_STATE(f32, camera_transition_foc_dist, sModeTransition.focDist, "TODO: document this")
BF_DYNAMIC_STATE(s32, camera_transition_frames_left, sModeTransition.framesLeft, "TODO: document this")
BF_DYNAMIC_VEC3(camera_transition_mario_pos, sModeTransition.marioPos, "TODO: document this")
__NL__

BF_DYNAMIC_STATE(s16, camera_avoid_yaw_vel, sAvoidYawVel, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_yaw_after_door_cutscene, sCameraYawAfterDoorCutscene, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_selection_flags, sSelectionFlags, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_2nd_rotate_flags, s2ndRotateFlags, "TODO: document this")
BF_DYNAMIC_STATE(u16, camera_c_buttons_pressed, sCButtonsPressed, "TODO: document this")
__NL__

//gLakituState may be incomplete
BF_DYNAMIC_VEC3(camera_lakitu_cur_focus, gLakituState.curFocus, "Lakitu's current look at point")
BF_DYNAMIC_VEC3(camera_lakitu_cur_pos, gLakituState.curPos, "Lakitu's current position")
BF_DYNAMIC_VEC3(camera_lakitu_goal_focus, gLakituState.goalFocus, "the position Lakitu's focus point is moving towards")
BF_DYNAMIC_VEC3(camera_lakitu_goal_pos, gLakituState.goalPos, "the position Lakitu is flying towards")
BF_DYNAMIC_STATE(u8, camera_lakitu_mode, gLakituState.mode, "The current Lakitu state mode")
BF_DYNAMIC_STATE(u8, camera_lakitu_def_mode, gLakituState.defMode, "The Lakitu state default mode")
BF_DYNAMIC_STATE(f32, camera_lakitu_foc_hspeed, gLakituState.focHSpeed, "The rate at which Lakitu's look-at point approaches its target on the xz-plane")
BF_DYNAMIC_STATE(f32, camera_lakitu_foc_vspeed, gLakituState.focVSpeed, "The rate at which Lakitu's look-at point approaches its target on the y-axis")
BF_DYNAMIC_STATE(f32, camera_lakitu_pos_hspeed, gLakituState.posHSpeed, "The rate at which Lakitu's position approaches its target on the xz-plane")
BF_DYNAMIC_STATE(f32, camera_lakitu_pos_vspeed, gLakituState.posVSpeed, "The rate at which Lakitu's position approaches its target on the y-axis")
BF_DYNAMIC_STATE(s16, camera_lakitu_yaw, gLakituState.yaw, "Lakitu's yaw value in angle units")
BF_DYNAMIC_STATE(s16, camera_lakitu_next_yaw, gLakituState.nextYaw, "TODO: document this")
__NL__

BF_DYNAMIC_STATE(s16, camera_area_yaw, sAreaYaw, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_area_yaw_change, sAreaYawChange, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_lakitu_dist, sLakituDist, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_lakitu_pitch, sLakituPitch, "TODO: document this")
BF_DYNAMIC_STATE(f32, camera_zoom_amount, sZoomAmount, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_c_side_button_yaw, sCSideButtonYaw, "TODO: document this")
BF_DYNAMIC_STATE(f32, camera_c_up_camera_pitch, sCUpCameraPitch, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_mode_offset_yaw, sModeOffsetYaw, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_8dir_mode_base_yaw, s8DirModeBaseYaw, "TODO: document this")
BF_DYNAMIC_STATE(s16, camera_8dir_mode_yaw_offset, s8DirModeYawOffset, "TODO: document this")
BF_DYNAMIC_STATE(f32, camera_pan_distance, sPanDistance, "TODO: document this")
BF_DYNAMIC_STATE(f32, camera_cannon_y_offset, sCannonYOffset, "TODO: document this")
__NL__

#define BF_DYNAMIC_LINEAR_TRANSITION_POINT(name, expr) \
	BF_DYNAMIC_VEC3(name##_focus, expr.focus, "TODO: document this") __NL__\
	BF_DYNAMIC_VEC3(name##_pos, expr.pos, "TODO: document this") __NL__\
	BF_DYNAMIC_STATE(f32, name##_dist, expr.dist, "TODO: document this") __NL__\
	BF_DYNAMIC_STATE(s16, name##_pitch, expr.pitch, "TODO: document this") __NL__\
	BF_DYNAMIC_STATE(s16, name##_yaw, expr.yaw, "TODO: document this")
	
#define BF_DYNAMIC_MODE_TRANSITION_INFO(name, expr) \
	BF_DYNAMIC_STATE(s16, name##_new_mode, expr.newMode, "The mode to transition into") __NL__\
	BF_DYNAMIC_STATE(s16, name##_last_mode, expr.lastMode, "The mode to transition out of") __NL__\
	BF_DYNAMIC_STATE(s16, name##_max, expr.max, "TODO: document this") __NL__\
	BF_DYNAMIC_STATE(s16, name##_frame, expr.frame, "TODO: document this") __NL__\
	BF_DYNAMIC_LINEAR_TRANSITION_POINT(name##_transition_start, expr.transitionStart) __NL__\
	BF_DYNAMIC_LINEAR_TRANSITION_POINT(name##_transition_end, expr.transitionEnd)
	
BF_DYNAMIC_MODE_TRANSITION_INFO(camera_mode_info, sModeInfo)
BF_DYNAMIC_VEC3(camera_castle_entrance_offset, sCastleEntranceOffset, "the position at which the camera is locked in the castle's lobby when entering for the first time iirc")
__NL__

// TODO: implement sParTrackIndex, sParTrackPath, sParTrackTransOff, sCameraStoreCUp, sCameraStoreCutscene
__NL__

BF_DYNAMIC_STATE(s16, camera_movement_flags, gCameraMovementFlags, "gCameraMovementFlags")
BF_DYNAMIC_STATE(s16, camera_status_flags, sStatusFlags, "sStatusFlags")
__NL__

// base 8033cbd0 US; 8033b860 JP
BF_DYNAMIC_STATE(u8, camera_mode, gCamera->mode, "Camera mode")
BF_DYNAMIC_STATE(u8, camera_def_mode, gCamera->defMode, "Camera default mode")
BF_DYNAMIC_STATE(u8, camera_cutscene, gCamera->cutscene, "Current cutscene index")
BF_DYNAMIC_STATE(s16, camera_yaw, gCamera->yaw, "Current camera yaw")
BF_DYNAMIC_STATE(s16, camera_next_yaw, gCamera->nextYaw, "TODO: document this")
BF_DYNAMIC_VEC3(camera_focus, gCamera->focus, "the point the camera looks at")
BF_DYNAMIC_VEC3(camera_pos, gCamera->pos, "the position of the camera")
BF_DYNAMIC_STATE(f32, camera_area_center_x, gCamera->areaCenX, "TODO: document this")
BF_DYNAMIC_STATE(f32, camera_area_center_z, gCamera->areaCenZ, "TODO: document this")
__NL__
__NL__

#endif