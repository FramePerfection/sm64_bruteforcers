#ifndef BF_MISC_UTIL_H
#define BF_MISC_UTIL_H

#include "bruteforce/framework/interface/readers.h"
#include "bruteforce/framework/interface/types.h"

#include "engine/graph_node.h"
#include <PR/ultratypes.h>

f32 bf_random_float();
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

#endif // BF_MISC_UTIL_H