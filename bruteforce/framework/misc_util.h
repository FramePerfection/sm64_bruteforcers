

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

void init_static_surfaces(Triangles tris);
struct Surface *gen_surface(s16 x1, s16 y1, s16 z1, s16 x2, s16 y2, s16 z2, s16 x3, s16 y3, s16 z3, s16 surf_type);
void adjust_analog_stick(struct Controller *controller);

#endif // MISC_UTIL_H