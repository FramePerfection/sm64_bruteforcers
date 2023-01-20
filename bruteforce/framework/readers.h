#ifndef READERS_H
#define READERS_H

#include "bruteforce/framework/json.h"
#include <PR/ultratypes.h>
#include "types.h"

typedef char* string;
typedef u8 Boolean;
void read_s32(Json*, s32*);
void read_s16(Json*, s16*);
void read_u32(Json*, u32*);
void read_u16(Json*, u16*);
void read_u8(Json*, u8*);
void read_f32(Json*, f32*);
void read_f64(Json*, f64*);
void read_string(Json*, string*);
void read_Boolean(Json*, Boolean*);

typedef struct Triangle {
	s16 x1, y1, z1;
	s16 x2, y2, z2;
	s16 x3, y3, z3;
	s16 surf_type;
} Triangle;

typedef struct Triangles {
	u32 data_size;
	Triangle* data;
} Triangles;


typedef struct Coin {
	s16 x, y, z;
} Coin;
typedef struct Coins {
	u32 data_size;
	Coin* data;
} Coins;

void read_Triangles(Json*, Triangles*);
void read_Coins(Json*, Coins*);
typedef s16* EnvironmentRegions;

void read_EnvironmentRegions(Json*, EnvironmentRegions*);

void read_Vec3f(Json*, Vec3f*);

#define JSON_PARSE_FIELD(target, type, field_name, node) \
	if (strcmp(node->name, field_name) == 0) \
		target##read_##type(node);

#endif // READERS_H