#ifndef READERS_H
#define READERS_H

#include "bruteforce/json.h"
#include <PR/ultratypes.h>

s32 read_s32(Json*);
s16 read_s16(Json*);
u32 read_u32(Json*);
u16 read_u16(Json*);
u8 read_u8(Json*);
f32 read_f32(Json*);
u32 read_u32hex(Json*);

typedef u32 u32hex;

typedef struct Triangle {
	s16 x1, y1, z1;
	s16 x2, y2, z2;
	s16 x3, y3, z3;
} Triangle;

typedef struct Triangles {
	u32 data_size;
	Triangle* data;
} Triangles;

Triangles read_Triangles(Json*);

#endif // READERS_H