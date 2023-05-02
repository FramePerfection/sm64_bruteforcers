#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bruteforce/framework/readers.h"
#include "bruteforce/framework/states.h"
#include "bruteforce/framework/json.h"
#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/interface.h"

#define BF_STATE_INCLUDE
#include STATE_DEFINITION_FILE
#undef BF_STATE_INCLUDE

static BFControlState bfInitialControlState;
BFStaticState bfStaticState;
BFDynamicState bfInitialDynamicState;
BFControlState *bfControlState;
ProgramState *programState;

static void s_read_state_json(Json *node)
{
	// clang-format off
	
	#define BF_CONTROL_STATE(type, struct_name, documentation) \
		if (strcmp(#struct_name, node->name) == 0) \
		{ \
			bf_read_##type(node, &bfInitialControlState.struct_name); \
			return; \
		}

	#define BF_DYNAMIC_STATE(type, struct_name, target_expr, documentation) \
		if (strcmp(#struct_name, node->name) == 0) \
		{ \
			bf_read_##type(node, &bfInitialDynamicState.struct_name); \
			memcpy(&(target_expr), &bfInitialDynamicState.struct_name, sizeof bfInitialDynamicState.struct_name); \
			return; \
		}

	#define BF_STATIC_STATE(type, struct_name, target_expr, documentation) \
		if (strcmp(#struct_name, node->name) == 0) \
		{ \
			bf_read_##type(node, &bfStaticState.struct_name); \
			memcpy(&(target_expr), &bfStaticState.struct_name, sizeof bfStaticState.struct_name); \
			return; \
		}

	#include STATE_DEFINITION_FILE

	#undef BF_CONTROL_STATE
	#undef BF_DYNAMIC_STATE
	#undef BF_STATIC_STATE
	// clang-format on

	bf_safe_printf("Warning: Undefined state \"%s\" set.\n", node->name);
}

u8 bf_init_states()
{
	bfControlState = &bfInitialControlState;
	const char *fileContents = bf_read_file(override_config_file != NULL ? override_config_file : "configuration.json");
	Json *root = Json_create(fileContents);
	if (!root)
	{
		printf("error in configuration.json starting at:\n%s\n", Json_getError());
		free(fileContents);
		return FALSE;
	}
	Json *node = root->child;
	while (node != NULL)
	{
		s_read_state_json(node);
		node = node->next;
	}
	free(fileContents);
	Json_dispose(root);
	return TRUE;
}

void bf_load_dynamic_state(BFDynamicState *state)
{
	// clang-format off

	#define BF_CONTROL_STATE(_0, _1, _2)
	#define BF_DYNAMIC_STATE(type, struct_name, target_expr, documentation) \
		memcpy(&(target_expr), &state->struct_name, sizeof state->struct_name);
	#define BF_STATIC_STATE(_0, _1, _2, _3)
	
	#include STATE_DEFINITION_FILE

	#undef BF_CONTROL_STATE
	#undef BF_DYNAMIC_STATE
	#undef BF_STATIC_STATE
	// clang-format on
}

void bf_save_dynamic_state(BFDynamicState *state)
{
	// clang-format off
	
	#define BF_CONTROL_STATE(_0, _1, _2)
	#define BF_DYNAMIC_STATE(type, struct_name, target_expr, documentation) \
		memcpy(&state->struct_name, &(target_expr), sizeof state->struct_name);
	#define BF_STATIC_STATE(_0, _1, _2, _3)
	
	#include STATE_DEFINITION_FILE

	#undef BF_CONTROL_STATE
	#undef BF_DYNAMIC_STATE
	#undef BF_STATIC_STATE
	// clang-format on
}

void bf_update_control_state(char *input)
{
	Json *root = Json_create(input);
	if (!root)
	{
		bf_safe_printf("Invalid conrol state json starting at:\n%s\n", Json_getError());
		return FALSE;
	}
	Json *node = root->child;
	// TODO(Important): lock a mutex to prevent "torn reads" in running processes
	while (node != NULL)
	{
		// clang-format off

		#define BF_CONTROL_STATE(type, struct_name, documentation) \
			if (strcmp(#struct_name, node->name) == 0) \
				bf_read_##type(node, &bfControlState->struct_name);
		#define BF_DYNAMIC_STATE(_0, _1, _2, _3)
		#define BF_STATIC_STATE(_0, _1, _2, _3)

		#include STATE_DEFINITION_FILE

		#undef BF_CONTROL_STATE
		#undef BF_DYNAMIC_STATE
		#undef BF_STATIC_STATE
		// clang-format on

		node = node->next;
	}
	Json_dispose(root);
}