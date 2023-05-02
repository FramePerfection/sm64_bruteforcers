#ifndef BF_READERS_H
#define BF_READERS_H

#include <PR/ultratypes.h>

#include "bruteforce/framework/types.h"
#include "bruteforce/framework/json.h"

#include "include/types.h"

void bf_read_s32(Json *, s32 *);
void bf_read_s16(Json *, s16 *);
void bf_read_u32(Json *, u32 *);
void bf_read_u16(Json *, u16 *);
void bf_read_u8(Json *, u8 *);
void bf_read_f32(Json *, f32 *);
void bf_read_f64(Json *, f64 *);
void bf_read_string(Json *, string *);
void bf_read_boolean(Json *, boolean *);

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

void bf_read_Triangles(Json *, Triangles *);
void bf_read_HitBoxes(Json *, HitBoxes *);
typedef s16 *EnvironmentRegions;

void bf_read_EnvironmentRegions(Json *, EnvironmentRegions *);

void bf_read_Vec3f(Json *, Vec3f *);

#define JSON_PARSE_FIELD(target, type, field_name, node) \
	if (strcmp(node->name, field_name) == 0)             \
		target##bf_read_##type(node);

#endif // BF_READERS_H