#define __A_THING(type, struct_name, _2, documentation) \
__NL__ type struct_name; __NL__ documentation

#define BF_DYNAMIC_STATE(type, struct_name, target_expr, documentation) __A_THING(type, struct_name, _, documentation)
#define BF_STATIC_STATE(type, struct_name, target_expr, documentation) __A_THING(type, struct_name, _, documentation)
#define BF_CONTROL_STATE(type, struct_name, target_expr, documentation) __A_THING(type, struct_name, _, documentation)

#define BF_DYNAMIC_VEC3(name, expr, documentation) \
	BF_DYNAMIC_STATE(f32, name##_x, expr[0], "The x component of "documentation) __NL__\
	BF_DYNAMIC_STATE(f32, name##_y, expr[1], "The z component of "documentation) __NL__\
	BF_DYNAMIC_STATE(f32, name##_z, expr[2], "The y component of "documentation)
#undef __NL__
#define STATE_DEFINITION_FILE <bruteforce/modules/MODULE_PATH/bf_state_definitions.inc.c>
#include STATE_DEFINITION_FILE