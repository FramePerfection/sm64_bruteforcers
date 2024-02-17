#ifndef BRUTEFORCE_TYPES_H
#define BRUTEFORCE_TYPES_H

#include "include/types.h"

typedef char *string;
typedef unsigned char boolean;

typedef struct Triangle
{
    s16 x1, y1, z1;
    s16 x2, y2, z2;
    s16 x3, y3, z3;
    s16 surf_type;
} Triangle;

typedef struct Triangles
{
    u32 data_size;
    Triangle *data;
} Triangles;

typedef struct HitBox
{
    f32 x, y, z, radius, above, below;
} HitBox;
typedef struct HitBoxes
{
    u32 data_size;
    HitBox *data;
} HitBoxes;

typedef s16 *EnvironmentRegions;

#endif // BRUTEFORCE_TYPES_H