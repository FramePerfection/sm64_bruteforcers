#ifndef BF_STATES_H
#define BF_STATES_H

#include "bruteforce/readers.h"

#define BF_DYNAMIC_VEC3(name, expr) \
	BF_DYNAMIC_STATE(f32, name##_x, expr[0]) \
	BF_DYNAMIC_STATE(f32, name##_y, expr[1]) \
	BF_DYNAMIC_STATE(f32, name##_z, expr[2])

#define BF_STATIC_STATE(type, name, target_expr) \
	type name;
#define BF_DYNAMIC_STATE(type, name, target_expr)

typedef struct BFStaticState_s {
#include "bruteforce/bf_state_definitions.inc.c"
} BFStaticState;


#define BF_DYNAMIC_STATE(type, name, target_expr) \
	type name;
#define BF_STATIC_STATE(type, name, target_expr)

typedef struct BFDyanmicState_s {
#include "bruteforce/bf_state_definitions.inc.c"
} BFDynamicState;

#undef BF_STATIC_STATE
#undef BF_DYANMIC_STATE

extern BFStaticState bfStaticState;
extern BFDynamicState bfInitialDynamicState;

u8 bf_init_states();
void bf_load_dynamic_state(BFDynamicState*);

#endif // BF_STATES_H