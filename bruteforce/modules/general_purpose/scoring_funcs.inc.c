/* scoring functions, declared as follows:

#define PARAM_MEMBERS_<function_name> \
	PARAM_MEMBER(<type_1>, <name_1>) \
	PARAM_MEMBER(<type_2>, <name_2>) \
	...
	PARAM_MEMBER(<type_n>, <name_n>) \
SCORING_FUNC(<function name>)

implementation must look as follows:
f64 sm_<function_name>(<function_name>Parameters args, Candidate *candidate, u8 *success) { ... }

*/

#ifndef SCORING_FUNC_IMPL

#define PARAM_MEMBERS_MaximizeHSpeed
SCORING_FUNC(MaximizeHSpeed)

#define PARAM_MEMBERS_CheckAction \
	PARAM_MEMBER(u32, frame) \
	PARAM_MEMBER(u32, action) \
	PARAM_MEMBER(u32, set_success)
SCORING_FUNC(CheckAction)

#define PARAM_MEMBERS_RestrictAngle \
	PARAM_MEMBER(u32, frame) \
	PARAM_MEMBER(s16, angle) \
	PARAM_MEMBER(s16, margin)
SCORING_FUNC(RestrictAngle)

#else

extern struct MarioState *gMarioState;
f64 sm_MaximizeHSpeed(MaximizeHSpeedParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	*success = TRUE;
	return gMarioState->forwardVel;
}

f64 sm_CheckAction(CheckActionParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	if ((candidate->stats.frame_index + 1) == args->frame) {
		u8 matchAction = gMarioState->action == args->action;
		*abort = !matchAction;
		*success = matchAction || !args->set_success;
		if (!*success)
			return -INFINITY;
	}
	else if (args->set_success)
		*success = FALSE;
	return 0;
}

f64 sm_RestrictAngle(RestrictAngleParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	if ((candidate->stats.frame_index + 1) == args->frame) {
		s16 dYaw = gMarioState->faceAngle[1] - bfStaticState.target_angle;
		if (dYaw < 0)
			dYaw = -dYaw;
		*success = dYaw <= bfStaticState.target_angle_margin;
		if (!*success) {
			candidate->score = -dYaw;
			return 0;
		}
	}
	return 0;
}

#endif