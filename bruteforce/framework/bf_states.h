#ifndef BF_STATES_H
#define BF_STATES_H

#include "bruteforce/framework/readers.h"

#define STATE_INCLUDE <bruteforce/modules/MODULE_PATH/state.h>
#include STATE_INCLUDE

#define STATE_DEFINITION_FILE <bruteforce/modules/MODULE_PATH/bf_state_definitions.inc.c>

#define BF_STATE_INCLUDE
#include STATE_DEFINITION_FILE
#undef BF_STATE_INCLUDE

#define BF_DYNAMIC_VEC3(name, expr) \
	BF_DYNAMIC_STATE(f32, name##_x, expr[0]) \
	BF_DYNAMIC_STATE(f32, name##_y, expr[1]) \
	BF_DYNAMIC_STATE(f32, name##_z, expr[2])
#define __NL__

#define BF_STATIC_STATE(type, name, target_expr) \
	type name;
#define BF_DYNAMIC_STATE(type, name, target_expr)

typedef struct BFStaticState_s {
#include STATE_DEFINITION_FILE
} BFStaticState;


#define BF_DYNAMIC_STATE(type, name, target_expr) \
	type name;
#define BF_STATIC_STATE(type, name, target_expr)

typedef struct BFDyanmicState_s {
#include STATE_DEFINITION_FILE
} BFDynamicState;

#undef BF_STATIC_STATE
#undef BF_DYANMIC_STATE

extern BFStaticState bfStaticState;
extern BFDynamicState bfInitialDynamicState;
extern ProgramState *programState;

void read_state_json(Json*);
u8 bf_init_states();
void bf_load_dynamic_state(BFDynamicState*);

#endif // BF_STATES_H