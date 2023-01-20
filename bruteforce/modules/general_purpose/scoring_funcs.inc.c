#ifndef CREATE_PARAM_DEFINITION_FILE
/* scoring functions, declared as follows:

#define PARAM_MEMBERS_<function_name> \
	PARAM_MEMBER(<type_1>, <name_1>, <comment_1>) \
	PARAM_MEMBER(<type_2>, <name_2>, <comment_2>) \
	...
	PARAM_MEMBER(<type_n>, <name_n>, <comment_n>)
SCORING_FUNC(<function_name>)

implementation must look as follows:
f64 sm_<function_name>(<function_name>Parameters args, Candidate *candidate, u8 *success, u8 *abort) { ... }

*/
#endif

#ifndef SCORING_FUNC_IMPL

#define PARAM_MEMBERS_MaximizeHSpeed
SCORING_FUNC(MaximizeHSpeed)

#define PARAM_MEMBERS_CheckAction \
	PARAM_MEMBER(u32, action, "Mario's action to check against") 
SCORING_FUNC(CheckAction)

#define PARAM_MEMBERS_CheckWall \
	PARAM_MEMBER(u32, wall, "Mario's wall reference to check against") \
	PARAM_MEMBER(Boolean, invert, "Whether to invert result of this check")
SCORING_FUNC(CheckWall)

#define PARAM_MEMBERS_RestrictAngle \
	PARAM_MEMBER(s16, angle, "Mario's angle to check against") \
	PARAM_MEMBER(s16, margin, "The margin of error within which Mario's angle will be accepted")
SCORING_FUNC(RestrictAngle)

#define PARAM_MEMBERS_RestrictHPosition \
	PARAM_MEMBER(f64, nx, "The plane normal's x component") \
	PARAM_MEMBER(f64, nz, "The plane normal's z component") \
	PARAM_MEMBER(f64, d, "The plane's offset value") \
	PARAM_MEMBER(Boolean, approach, "Determines whether a score should be computed for getting close to the restriction plane")
SCORING_FUNC(RestrictHPosition)

#define PARAM_MEMBERS_RestrictHDist \
	PARAM_MEMBER(f32, x, "The cylinder's x position") \
	PARAM_MEMBER(f32, z, "The cylinder's z position") \
	PARAM_MEMBER(f32, dist, "The maximum distance of Mario from the cylinder that will be accepted")
SCORING_FUNC(RestrictHDist)

#else

// not necessary for compilation, but allows intellisense to find struct definitions
#include "bruteforce/modules/general_purpose/scoring_method.h"

extern struct MarioState *gMarioState;
f64 sm_MaximizeHSpeed(MaximizeHSpeedParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	*success = TRUE;
	return gMarioState->forwardVel;
}

f64 sm_CheckAction(CheckActionParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	u8 matchAction = gMarioState->action == args->action;
	*abort = !matchAction;
	*success = matchAction;
	if (!*success)
		return -INFINITY;
	return 0;
}

f64 sm_CheckWall(CheckWallParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	u8 matchWall = (gMarioState->wall == args->wall) ^ args->invert;
	*abort = !matchWall;
	*success = matchWall;
	if (!*success)
		return -INFINITY;
	return 0;
}

f64 sm_RestrictAngle(RestrictAngleParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	s16 dYaw = gMarioState->faceAngle[1] - args->angle;
	if (dYaw < 0)
		dYaw = -dYaw;
	*success = dYaw <= args->margin;
	if (!*success) {
		candidate->score = -dYaw;
		return 0;
	}
	return 0;
}

f64 sm_RestrictHPosition(RestrictHPositionParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	f64 diff = (gMarioState->pos[0] * args->nx + gMarioState->pos[2] * args->nz) - args->d;
	if (diff > 0) {
		*success = FALSE;
		*abort = TRUE;
	}
	else if (args->approach)
		return diff * -diff;
	return 0;
}

f64 sm_RestrictHDist(RestrictHDistParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	f32 dx = gMarioState->pos[0] - args->x;
	f32 dz = gMarioState->pos[2] - args->z;
	f32 distSq = (dx * dx + dz * dz);
	if (distSq >= args->dist * args->dist) {
		*abort = TRUE;
		*success = FALSE;
	}
}

#endif