#include "engine/graph_node.h"

void geo_obj_init_animation(UNUSED struct GraphNodeObject *graphNode, UNUSED struct Animation **animPtrAddr) {}
s16 geo_update_animation_frame(UNUSED struct AnimInfo *obj, UNUSED s32 *accelAssist) {}
void geo_obj_init(UNUSED struct GraphNodeObject *graphNode, UNUSED void *sharedChild, UNUSED Vec3f pos, UNUSED Vec3s angle) {}
void geo_obj_init_spawninfo(UNUSED struct GraphNodeObject *graphNode, UNUSED struct SpawnInfo *spawn) {}
void geo_reset_object_node(UNUSED struct GraphNodeObject *graphNode) {}
void geo_obj_init_animation_accel(UNUSED struct GraphNodeObject *graphNode, UNUSED struct Animation **animPtrAddr, UNUSED u32 animAccel) {}
struct GraphNode *geo_add_child(UNUSED struct GraphNode *parent, UNUSED struct GraphNode *childNode) {}
struct GraphNode *geo_remove_child(UNUSED struct GraphNode *graphNode) {}