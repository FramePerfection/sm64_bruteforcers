#include "bruteforce/misc_util.h"
#include "stdlib.h"

f32 randFloat() {
	return ((float)rand()/(float)(RAND_MAX));
}

void init_static_surfaces(Triangles tris) {
	u32 i;
	for (i = 0; i < tris.data_size; i++) {
		Triangle t = tris.data[i];
		struct Surface *surface = gen_surface(t.x1, t.y1, t.z1, t.x2, t.y2, t.z2, t.x3, t.y3, t.z3, t.surf_type);
		if (surface != NULL)
			add_surface(surface, FALSE);
		else
			printf("found degenerate triangle: (%d,%d,%d),(%d,%d,%d),(%d,%d,%d)\n", t.x1, t.y1, t.z1, t.x2, t.y2, t.z2, t.x3, t.y3, t.z3);
	}

}

struct Surface *gen_surface(s16 x1, s16 y1, s16 z1, s16 x2, s16 y2, s16 z2, s16 x3, s16 y3, s16 z3, s16 surf_type) {
    // (v2 - v1) x (v3 - v2)
    f32 nx = (y2 - y1) * (z3 - z2) - (z2 - z1) * (y3 - y2);
    f32 ny = (z2 - z1) * (x3 - x2) - (x2 - x1) * (z3 - z2);
    f32 nz = (x2 - x1) * (y3 - y2) - (y2 - y1) * (x3 - x2);
    f32 mag = sqrtf(nx * nx + ny * ny + nz * nz);

    // Could have used min_3 and max_3 for this...
    f32 minY = y1;
    if (y2 < minY) {
        minY = y2;
    }
    if (y3 < minY) {
        minY = y3;
    }

    f32 maxY = y1;
    if (y2 > maxY) {
        maxY = y2;
    }
    if (y3 > maxY) {
        maxY = y3;
    }

    // Checking to make sure no DIV/0
    if (mag < 0.0001) {
        return NULL;
    }
    mag = (f32)(1.0 / mag);
    nx *= mag;
    ny *= mag;
    nz *= mag;

    struct Surface *surface = alloc_surface();

    surface->vertex1[0] = x1;
    surface->vertex2[0] = x2;
    surface->vertex3[0] = x3;

    surface->vertex1[1] = y1;
    surface->vertex2[1] = y2;
    surface->vertex3[1] = y3;

    surface->vertex1[2] = z1;
    surface->vertex2[2] = z2;
    surface->vertex3[2] = z3;

    surface->normal.x = nx;
    surface->normal.y = ny;
    surface->normal.z = nz;

    surface->originOffset = -(nx * x1 + ny * y1 + nz * z1);

    surface->lowerY = minY - 5;
    surface->upperY = maxY + 5;

    surface->type = surf_type;

    return surface;
}

/**
 * Initialize a geo node with a given type. Sets all links such that there
 * are no siblings, parent or children for this node.
 */
void init_scene_graph_node_links(struct GraphNode *graphNode, s32 type) {
    graphNode->type = type;
    graphNode->flags = GRAPH_RENDER_ACTIVE;
    graphNode->prev = graphNode;
    graphNode->next = graphNode;
    graphNode->parent = NULL;
    graphNode->children = NULL;
}

/**
 * Allocates and returns a newly created object node
 */
struct GraphNodeObject *init_graph_node_object(struct AllocOnlyPool *pool,
                                               struct GraphNodeObject *graphNode,
                                               struct GraphNode *sharedChild, Vec3f pos, Vec3s angle,
                                               Vec3f scale) {
    if (graphNode != NULL) {
        init_scene_graph_node_links(&graphNode->node, GRAPH_NODE_TYPE_OBJECT);
        vec3f_copy(graphNode->pos, pos);
        vec3f_copy(graphNode->scale, scale);
        vec3s_copy(graphNode->angle, angle);
        graphNode->sharedChild = sharedChild;
        graphNode->throwMatrix = NULL;
        graphNode->animInfo.animID = 0;
        graphNode->animInfo.curAnim = NULL;
        graphNode->animInfo.animFrame = 0;
        graphNode->animInfo.animFrameAccelAssist = 0;
        graphNode->animInfo.animAccel = 0x10000;
        graphNode->animInfo.animTimer = 0;
        graphNode->node.flags |= GRAPH_RENDER_HAS_ANIMATION;
    }

    return graphNode;
}

/**
 * Take the updated controller struct and calculate the new x, y, and distance floats.
 */
void adjust_analog_stick(struct Controller *controller) {
    UNUSED u8 filler[8];

    // Reset the controller's x and y floats.
    controller->stickX = 0;
    controller->stickY = 0;

    // Modulate the rawStickX and rawStickY to be the new f32 values by adding/subtracting 6.
    if (controller->rawStickX <= -8) {
        controller->stickX = controller->rawStickX + 6;
    }

    if (controller->rawStickX >= 8) {
        controller->stickX = controller->rawStickX - 6;
    }

    if (controller->rawStickY <= -8) {
        controller->stickY = controller->rawStickY + 6;
    }

    if (controller->rawStickY >= 8) {
        controller->stickY = controller->rawStickY - 6;
    }

    // Calculate f32 magnitude from the center by vector length.
    controller->stickMag =
        sqrtf(controller->stickX * controller->stickX + controller->stickY * controller->stickY);

    // Magnitude cannot exceed 64.0f: if it does, modify the values
    // appropriately to flatten the values down to the allowed maximum value.
    if (controller->stickMag > 64) {
        controller->stickX *= 64 / controller->stickMag;
        controller->stickY *= 64 / controller->stickMag;
        controller->stickMag = 64;
    }
}