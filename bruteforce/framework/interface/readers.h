#ifndef BF_READERS_H
#define BF_READERS_H

#include <PR/ultratypes.h>

#include "json.h"
#include "types.h"

long long bf_readers_util_read_int(Json *jsonNode);

void bf_read_uintptr_t(Json *, uintptr_t *);
void bf_read_s32(Json *, s32 *);
void bf_read_s16(Json *, s16 *);
void bf_read_u32(Json *, u32 *);
void bf_read_u16(Json *, u16 *);
void bf_read_u8(Json *, u8 *);
void bf_read_f32(Json *, f32 *);
void bf_read_f64(Json *, f64 *);
void bf_read_string(Json *, string *);
void bf_read_boolean(Json *, boolean *);

void bf_read_Triangles(Json *, Triangles *);
void bf_read_HitBoxes(Json *, HitBoxes *);

void bf_read_EnvironmentRegions(Json *, EnvironmentRegions *);

void bf_read_Vec3f(Json *, Vec3f *);

#endif // BF_READERS_H