

#ifndef MISC_UTIL_H
#define MISC_UTIL_H

#include <PR/ultratypes.h>
#include "types.h"
#include "engine/graph_node.h"
#include "bruteforce/framework/readers.h"

f32 randFloat();
extern void create_camera(struct GraphNodeCamera *gc, struct AllocOnlyPool *pool);
extern void clear_static_surfaces(void);
extern void add_surface(struct Surface *surface, s32 dynamic);
extern struct Surface *alloc_surface(void);

void bf_init_static_surfaces(Triangles tris);
void bf_init_dynamic_surfaces(Triangles tris);
struct Surface *bf_gen_surface(s16 x1, s16 y1, s16 z1, s16 x2, s16 y2, s16 z2, s16 x3, s16 y3, s16 z3, s16 surf_type);
void adjust_analog_stick(struct Controller *controller);

void bf_init_camera();
void bf_init_area();
void bf_init_mario();
void bf_update_controller(OSContPad *input);

#endif // MISC_UTIL_H