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

BF_DYNAMIC_VEC3(camera_old_position, sOldPosition)
BF_DYNAMIC_VEC3(camera_old_focus, sOldFocus)

BF_DYNAMIC_STATE(s16, camera_state_camera_event, gPlayerCameraState[0].cameraEvent)
BF_DYNAMIC_STATE(u32, camera_state_usedObj, gPlayerCameraState[0].usedObj)

BF_DYNAMIC_STATE(s16, camera_transition_pos_pitch, sModeTransition.posPitch)
BF_DYNAMIC_STATE(s16, camera_transition_pos_yaw, sModeTransition.posYaw)
BF_DYNAMIC_STATE(f32, camera_transition_pos_dist, sModeTransition.posDist)
BF_DYNAMIC_STATE(s16, camera_transition_foc_pitch, sModeTransition.focPitch)
BF_DYNAMIC_STATE(s16, camera_transition_foc_yaw, sModeTransition.focYaw)
BF_DYNAMIC_STATE(f32, camera_transition_foc_dist, sModeTransition.focDist)
BF_DYNAMIC_STATE(s32, camera_transition_frames_left, sModeTransition.framesLeft)
BF_DYNAMIC_VEC3(camera_transition_mario_pos, sModeTransition.marioPos)

BF_DYNAMIC_STATE(s16, camera_avoid_yaw_vel, sAvoidYawVel)
BF_DYNAMIC_STATE(s16, camera_yaw_after_door_cutscene, sCameraYawAfterDoorCutscene)
BF_DYNAMIC_STATE(s16, camera_selection_flags, sSelectionFlags)
BF_DYNAMIC_STATE(s16, camera_2nd_rotate_flags, s2ndRotateFlags)
BF_DYNAMIC_STATE(u16, camera_c_buttons_pressed, sCButtonsPressed)

//gLakituState may be incomplete
BF_DYNAMIC_VEC3(camera_lakitu_cur_focus, gLakituState.curFocus)
BF_DYNAMIC_VEC3(camera_lakitu_cur_pos, gLakituState.curPos)
BF_DYNAMIC_VEC3(camera_lakitu_goal_focus, gLakituState.goalFocus)
BF_DYNAMIC_VEC3(camera_lakitu_goal_pos, gLakituState.goalPos)
BF_DYNAMIC_STATE(u8, camera_lakitu_mode, gLakituState.mode)
BF_DYNAMIC_STATE(u8, camera_lakitu_def_mode, gLakituState.defMode)

BF_DYNAMIC_STATE(s16, camera_area_yaw, sAreaYaw)
BF_DYNAMIC_STATE(s16, camera_area_yaw_change, sAreaYawChange)
BF_DYNAMIC_STATE(s16, camera_lakitu_dist, sLakituDist)
BF_DYNAMIC_STATE(s16, camera_lakitu_pitch, sLakituPitch)
BF_DYNAMIC_STATE(f32, camera_zoom_amount, sZoomAmount)
BF_DYNAMIC_STATE(s16, camera_c_side_button_yaw, sCSideButtonYaw)
BF_DYNAMIC_STATE(f32, camera_c_up_camera_pitch, sCUpCameraPitch)
BF_DYNAMIC_STATE(s16, camera_mode_offset_yaw, sModeOffsetYaw)
BF_DYNAMIC_STATE(s16, camera_8dir_mode_base_yaw, s8DirModeBaseYaw)
BF_DYNAMIC_STATE(s16, camera_8dir_mode_yaw_offset, s8DirModeYawOffset)
BF_DYNAMIC_STATE(f32, camera_pan_distance, sPanDistance)
BF_DYNAMIC_STATE(f32, camera_cannon_y_offset, sCannonYOffset)

#define BF_DYNAMIC_LINEAR_TRANSITION_POINT(name, expr) \
	BF_DYNAMIC_VEC3(name##_focus, expr.focus) \
	BF_DYNAMIC_VEC3(name##_pos, expr.pos) \
	BF_DYNAMIC_STATE(f32, name##_dist, expr.dist) \
	BF_DYNAMIC_STATE(s16, name##_pitch, expr.pitch) \
	BF_DYNAMIC_STATE(s16, name##_yaw, expr.yaw)
	
#define BF_DYNAMIC_MODE_TRANSITION_INFO(name, expr) \
	BF_DYNAMIC_STATE(s16, name##_new_mode, expr.newMode) \
	BF_DYNAMIC_STATE(s16, name##_last_mode, expr.lastMode) \
	BF_DYNAMIC_STATE(s16, name##_max, expr.max) \
	BF_DYNAMIC_STATE(s16, name##_frame, expr.frame) \
	BF_DYNAMIC_LINEAR_TRANSITION_POINT(name##_transition_start, expr.transitionStart) \
	BF_DYNAMIC_LINEAR_TRANSITION_POINT(name##_transition_end, expr.transitionEnd)
	
BF_DYNAMIC_MODE_TRANSITION_INFO(camera_mode_info, sModeInfo)
BF_DYNAMIC_VEC3(camera_castle_entrance_offset, sCastleEntranceOffset)

// TODO: implement sParTrackIndex, sParTrackPath, sParTrackTransOff, sCameraStoreCUp, sCameraStoreCutscene

BF_DYNAMIC_STATE(s16, camera_movement_flags, gCameraMovementFlags)
BF_DYNAMIC_STATE(s16, camera_status_flags, sStatusFlags)

// base 8033cbd0 US; 8033b860 JP
BF_DYNAMIC_STATE(u8, camera_mode, gCamera->mode)
BF_DYNAMIC_STATE(u8, camera_def_mode, gCamera->defMode)
BF_DYNAMIC_STATE(u8, camera_cutscene, gCamera->cutscene)
BF_DYNAMIC_STATE(s16, camera_yaw, gCamera->yaw)
BF_DYNAMIC_STATE(s16, camera_next_yaw, gCamera->nextYaw)
BF_DYNAMIC_VEC3(camera_focus, gCamera->focus)
BF_DYNAMIC_VEC3(camera_pos, gCamera->pos)
BF_DYNAMIC_STATE(f32, camera_area_center_x, gCamera->areaCenX)
BF_DYNAMIC_STATE(f32, camera_area_center_z, gCamera->areaCenZ)

#endif