#ifndef BF_QUARTER_STEPS_H
#define BF_QUARTER_STEPS_H

#include "include/types.h"
#include "bruteforce/framework/json.h"

typedef u8 quarterstep;

void bf_read_quarterstep(Json *jsonNode, quarterstep *target);

extern Vec3f quarterSteps[4][4];

Vec3f *bf_get_quarterstep(int step, int substep);

void bf_set_quarterstep(int step, int substep, Vec3f value);

#endif // BF_QUARTER_STEPS_H