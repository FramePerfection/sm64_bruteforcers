#define BF_JSON_STRUCT_NAME BehaviorScript
#include "bruteforce/framework/interface/json_array.inc.c"
#undef BF_JSON_STRUCT_NAME

#define BF_JSON_STRUCT_NAME uintptr_t
#define BF_JSON_ARRAY_SIZE 8
#include "bruteforce/framework/interface/json_array.inc.c"
#undef BF_JSON_STRUCT_NAME

#define BF_JSON_STRUCT_NAME u32
#define BF_JSON_ARRAY_SIZE 0x50
#include "bruteforce/framework/interface/json_array.inc.c"
#undef BF_JSON_STRUCT_NAME

#define BF_JSON_STRUCT_NAME BfObjectState
#define BF_JSON_STRUCT_DEFINITION_BfObjectState \
	BF_JSON_STRUCT_FIELD(u32Array0x50, raw_data) \
	BF_JSON_STRUCT_FIELD(u32, collided_obj_interact_types) \
	BF_JSON_STRUCT_FIELD(s16, active_flags) \
	BF_JSON_STRUCT_FIELD(s16, num_collided_objs) \
	BF_JSON_STRUCT_FIELD(u32, bhv_stack_index) \
	BF_JSON_STRUCT_FIELD(s16, bhv_delay_timer) \
	BF_JSON_STRUCT_FIELD(f32, hitbox_radius) \
	BF_JSON_STRUCT_FIELD(f32, hitbox_height) \
	BF_JSON_STRUCT_FIELD(f32, hurtbox_radius) \
	BF_JSON_STRUCT_FIELD(f32, hurtbox_height) \
	BF_JSON_STRUCT_FIELD(f32, hitbox_down_offset) \
	BF_JSON_STRUCT_FIELD(uintptr_tArray8, bhv_stack) \
	BF_JSON_STRUCT_FIELD(u32, behavior_script_index) \
	BF_JSON_STRUCT_FIELD(s32, parent_object_index)
#include "bruteforce/framework/interface/json_struct_reader.inc.c"
#include "bruteforce/framework/interface/json_array.inc.c"
#undef BF_JSON_STRUCT_NAME

#define BF_JSON_STRUCT_NAME BehaviorScriptArray
#include "bruteforce/framework/interface/json_array.inc.c"
#undef BF_JSON_STRUCT_NAME

#define BF_JSON_STRUCT_NAME ObjectTriangles
#include "bruteforce/framework/interface/json_array.inc.c"
#undef BF_JSON_STRUCT_NAME
