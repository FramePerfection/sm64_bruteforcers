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
// SCORING_FUNC declarations

#define PARAM_MEMBERS_MaximizeHSpeed
SCORING_FUNC(MaximizeHSpeed)

#define PARAM_MEMBERS_CheckAction \
	PARAM_MEMBER(u32, action, "Mario's action to check against") 
SCORING_FUNC(CheckAction)

#define PARAM_MEMBERS_CheckWall \
	PARAM_MEMBER(u32, wall, "Mario's wall reference to check against") \
	PARAM_MEMBER(boolean, invert, "Whether to invert result of this check")
SCORING_FUNC(CheckWall)

#define PARAM_MEMBERS_RestrictAngle \
	PARAM_MEMBER(s16, angle, "Mario's angle to check against") \
	PARAM_MEMBER(s16, margin, "The margin of error within which Mario's angle will be accepted")
SCORING_FUNC(RestrictAngle)

#define PARAM_MEMBERS_RestrictHPosition \
	PARAM_MEMBER(quarterstep, quarterstep, "The quarter-step on which to apply the function") \
	PARAM_MEMBER(f64, nx, "The plane normal's x component") \
	PARAM_MEMBER(f64, nz, "The plane normal's z component") \
	PARAM_MEMBER(f64, d, "The plane's offset value") \
	PARAM_MEMBER(boolean, approach, "Determines whether a score should be computed for getting close to the restriction plane")
SCORING_FUNC(RestrictHPosition)

#define PARAM_MEMBERS_RestrictVPosition \
	PARAM_MEMBER(quarterstep, quarterstep, "The quarter-step on which to apply the function") \
	PARAM_MEMBER(f32, y, "The y position restriction value to compare to") \
	PARAM_MEMBER(boolean, above, "If set, the measured quarter-step must be greater than the restriction value") \
	PARAM_MEMBER(boolean, approach, "Determines whether a score should be computed for getting close to the restriction plane")
SCORING_FUNC(RestrictVPosition)

#define PARAM_MEMBERS_XZRadialLimit \
	PARAM_MEMBER(quarterstep, quarterstep, "The quarter-step on which to apply the function") \
	PARAM_MEMBER(f32, x, "The cylinder's x position") \
	PARAM_MEMBER(f32, z, "The cylinder's z position") \
	PARAM_MEMBER(f32, dist, "The maximum distance of Mario from the cylinder that will be accepted") \
	PARAM_MEMBER(boolean, approach, "if set, a negative score will be applied when the tested position is outside of the accepted radius") \
	PARAM_MEMBER(boolean, abort_on_failure, "If set, attempts will abort on this frame if the distance is not accepted.")
SCORING_FUNC(XZRadialLimit)

#define PARAM_MEMBERS_MatchHSpeed \
	PARAM_MEMBER(f32, hspeed, "The hspeed to match")
SCORING_FUNC(MatchHSpeed)

#else
// SCORING_FUNC implementations

// not necessary for compilation, but allows intellisense to find struct definitions
#include "bruteforce/modules/general_purpose/scoring_method.h"
#include "bruteforce/framework/types.h"
#include "bruteforce/framework/quarter_steps.h"

#include <math.h>

extern struct MarioState *gMarioState;
extern 

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
	u8 matchWall = (gMarioState->wall == (struct Surface*)args->wall) ^ args->invert;
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
	Vec3f *srcPos = GetQuarterstep(args->quarterstep / 4, args->quarterstep % 4);
	f64 diff = ((*srcPos)[0] * args->nx + (*srcPos)[2] * args->nz) - args->d;
	if (diff > 0) {
		*success = FALSE;
		*abort = TRUE;
	}
	else if (args->approach)
		return diff * -diff;
	return 0;
}

f64 sm_RestrictVPosition(RestrictVPositionParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	Vec3f *srcPos = GetQuarterstep(args->quarterstep / 4, args->quarterstep % 4);
	f64 diff = (*srcPos)[1] - args->y;
	if (!args->above)
		diff *= -1;
	if (diff < 0) {
		*success = FALSE;
		*abort = TRUE;
	}
	else if (args->approach)
		return diff * -diff;
	return 0;
}

f64 sm_XZRadialLimit(XZRadialLimitParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	Vec3f *srcPos = GetQuarterstep(args->quarterstep / 4, args->quarterstep % 4);
	f32 dx = (*srcPos)[0] - args->x;
	f32 dz = (*srcPos)[2] - args->z;
	f32 distSq = (dx * dx + dz * dz);
	if (distSq >= args->dist * args->dist) {
		*abort = args->abort_on_failure;
		*success = !*abort;
		if (args->approach) {
			double mismatch = sqrt(distSq) - args->dist;
			return -mismatch * mismatch;
		}
	}
	return 0.0;
}

f64 sm_MatchHSpeed(MatchHSpeedParameters args, Candidate *candidate, u8 *success, u8 *abort) {
	f64 diff = args->hspeed - gMarioState->forwardVel;
	return -(diff * diff);
}

#endif