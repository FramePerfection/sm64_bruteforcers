#define BF_DYNAMIC_STATE(type, struct_name, target_expr) \
type struct_name
#define BF_STATIC_STATE(type, struct_name, target_expr) BF_DYNAMIC_STATE(type, struct_name, target_expr)
#define BF_DYNAMIC_VEC3(name, expr) \
	BF_DYNAMIC_STATE(f32, name##_x, expr[0]) __NL__\
	BF_DYNAMIC_STATE(f32, name##_y, expr[1]) __NL__\
	BF_DYNAMIC_STATE(f32, name##_z, expr[2])
#undef __NL__
#define STATE_DEFINITION_FILE <bruteforce/MODULE_PATH/bf_state_definitions.inc.c>
#include STATE_DEFINITION_FILE