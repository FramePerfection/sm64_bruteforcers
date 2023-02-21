#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bruteforce/framework/readers.h"
#include "bruteforce/framework/bf_states.h"
#include "bruteforce/framework/json.h"
#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/interface.h"

#define BF_STATE_INCLUDE
#include STATE_DEFINITION_FILE
#undef BF_STATE_INCLUDE


BFStaticState bfStaticState;
BFDynamicState bfInitialDynamicState;
ProgramState *programState;

void read_state_json(Json *node) {
	#define BF_STATIC_STATE(type, struct_name, target_expr) \
		if (strcmp(#struct_name, node->name) == 0) \
		{ \
			read_##type(node, &bfStaticState.struct_name); \
			memcpy(&(target_expr), &bfStaticState.struct_name, sizeof bfStaticState.struct_name); \
			return; \
		}

	#define BF_DYNAMIC_STATE(type, struct_name, target_expr) \
		if (strcmp(#struct_name, node->name) == 0) \
		{ \
			read_##type(node, &bfInitialDynamicState.struct_name); \
			memcpy(&(target_expr), &bfInitialDynamicState.struct_name, sizeof bfInitialDynamicState.struct_name); \
			return; \
		}

	#include STATE_DEFINITION_FILE

	#undef BF_STATIC_STATE
	#undef BF_DYNAMIC_STATE
	
	safePrintf("Warning: Undefined state \"%s\" set.\n", node->name);
}

u8 bf_init_states() {
	const char* fileContents = read_file(override_config_file != NULL ? override_config_file : "configuration.json");
	Json *root = Json_create(fileContents);
	if (!root) {
		printf("error in configuration.json starting at:\n%s\n", Json_getError());
		free(fileContents);
		return FALSE;
	}
	Json *node = root->child;
	while (node != NULL) {
		read_state_json(node);
		node = node->next;
	}
	free(fileContents);
	Json_dispose(root);
	return TRUE;
}

void bf_load_dynamic_state(BFDynamicState *state) {
	#define BF_STATIC_STATE(_0, _1, _2)
	#define BF_DYNAMIC_STATE(type, struct_name, target_expr) \
		memcpy(&(target_expr), &state->struct_name, sizeof state->struct_name);
	
	#include STATE_DEFINITION_FILE

	#undef BF_STATIC_STATE
	#undef BF_DYNAMIC_STATE
}