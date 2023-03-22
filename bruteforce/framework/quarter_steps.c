#include "bruteforce/framework/quarter_steps.h"
#include "bruteforce/framework/readers.h"

#include "src/engine/math_util.h"

void read_quarterstep(Json *jsonNode, quarterstep *target) {
	*target = (quarterstep)jsonNode->valueInt;
}

Vec3f quarterSteps[4][4];

Vec3f *GetQuarterstep(int step, int substep) {
	return &quarterSteps[step][substep];
}

void SetQuarterstep(int step, int substep, Vec3f value) {
	vec3f_copy(quarterSteps[step][substep], value);
}