#include "include/macros.h"
#include <PR/ultratypes.h>

#include "engine/graph_node.h"

// interaction.c
u32 does_mario_have_normal_cap_on_head(UNUSED struct MarioState *m) { return TRUE; }
u32 get_door_save_file_flag(UNUSED struct Object *door) { return 0; }
void mario_blow_off_cap(UNUSED struct MarioState *m, UNUSED f32 capSpeed) {}
void mario_drop_held_object(UNUSED struct MarioState *m) {}
u32 mario_check_object_grab(UNUSED struct MarioState *m) { return FALSE; }
struct Object *mario_get_collided_object(UNUSED struct MarioState *m, UNUSED u32 interactType) { return NULL; }
void mario_grab_used_object(UNUSED struct MarioState *m) {}
void mario_handle_special_floors(UNUSED struct MarioState *m) {}
u32 mario_lose_cap_to_enemy(UNUSED u32 arg) { return TRUE; }
s16 mario_obj_angle_to_object(UNUSED struct MarioState *m, UNUSED struct Object *o) { return 0; }
void mario_process_interactions(UNUSED struct MarioState *m) {}
void mario_retrieve_cap(void) {}
void mario_stop_riding_and_holding(UNUSED struct MarioState *m) {}
void mario_stop_riding_object(UNUSED struct MarioState *m) {}
void mario_throw_held_object(UNUSED struct MarioState *m) {}