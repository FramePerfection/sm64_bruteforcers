#include "include/types.h"
#include "bruteforce/framework/json.h"

typedef u8 quarterstep;

void read_quarterstep(Json *jsonNode, quarterstep *target);

extern Vec3f quarterSteps[4][4];

Vec3f *GetQuarterstep(int step, int substep);

void SetQuarterstep(int step, int substep, Vec3f value);