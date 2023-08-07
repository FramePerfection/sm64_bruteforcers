#ifndef BF_STATES_H
#define BF_STATES_H

#include "bruteforce/framework/readers.h"

#define STATE_INCLUDE <bruteforce/modules/MODULE_PATH/state.h>
#include STATE_INCLUDE

#define STATE_DEFINITION_FILE <bruteforce/modules/MODULE_PATH/bf_state_definitions.inc.c>

// Include necessary forward declarations and external symbols
#define BF_STATE_INCLUDE
#include STATE_DEFINITION_FILE
#undef BF_STATE_INCLUDE

// TODO: replace this with a vec3 type
#define BF_DYNAMIC_VEC3(name, expr, documentation) \
	BF_DYNAMIC_STATE(f32, name##_x, expr[0], "The x component of "documentation)   \
	BF_DYNAMIC_STATE(f32, name##_y, expr[1], "The y component of "documentation)   \
	BF_DYNAMIC_STATE(f32, name##_z, expr[2], "The z component of "documentation)
#define __NL__

// Define BFControlState
#define BF_CONTROL_STATE(type, name, documentation) \
	type name;
#define BF_DYNAMIC_STATE(_0, _1, _2, _3)
#define BF_STATIC_STATE(_0, _1, _2, _3)

typedef struct BFControlState_s
{
#include STATE_DEFINITION_FILE
} BFControlState;

#undef BF_STATIC_STATE
#undef BF_DYNAMIC_STATE
#undef BF_CONTROL_STATE

// Define BFDynamicState
#define BF_CONTROL_STATE(_0, _1, _2)
#define BF_DYNAMIC_STATE(type, name, target_expr, documentation) \
	type name;
#define BF_STATIC_STATE(_0, _1, _2, _3)

typedef struct BFDynamicState_s
{
#include STATE_DEFINITION_FILE
} BFDynamicState;

#undef BF_STATIC_STATE
#undef BF_DYNAMIC_STATE
#undef BF_CONTROL_STATE

// Define BFStaticState
#define BF_DYNAMIC_STATE(_0, _1, _2, _3)
#define BF_CONTROL_STATE(_0, _1, _2)
#define BF_STATIC_STATE(type, name, target_expr, documentation) \
	type name;

typedef struct BFStaticState_s
{
#include STATE_DEFINITION_FILE
} BFStaticState;

#undef BF_STATIC_STATE
#undef BF_DYNAMIC_STATE
#undef BF_CONTROL_STATE

extern BFControlState *bfControlState;
extern BFDynamicState bfInitialDynamicState;
extern BFStaticState bfStaticState;

extern ProgramState *programState;

u8 bf_init_states();
void bf_load_dynamic_state(BFDynamicState *);
void bf_save_dynamic_state(BFDynamicState *);
void bf_update_control_state(char *);

#endif // BF_STATES_H