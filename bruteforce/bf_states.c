#include "sm64.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bruteforce/readers.h"
#include "bruteforce/bf_states.h"
#include "bruteforce/json.h"

#define BF_STATE_INCLUDE
#include "bruteforce/bf_state_definitions.inc.c"
#undef BF_STATE_INCLUDE

BFStaticState bfStaticState;
BFDynamicState bfInitialDynamicState;

const char *read_file(const char *fileName) {
	FILE *file = fopen(fileName, "r");
	if (!file)
		return NULL;
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);
	const char *fileContents = malloc(size);
	fread(fileContents, sizeof(char), size, file);
	fclose(file);
	return fileContents;
}

u8 bf_init_states() {
	const char* fileContents = read_file("configuration.json");
	Json *root = Json_create(fileContents);
	if (!root) {
		printf("error in configuration.json starting at:\n%s\n", Json_getError());
		free(fileContents);
		return FALSE;
	}
	Json *node = root->child;
	while (node != NULL) {
	
		#define BF_STATIC_STATE(type, struct_name, target_expr) \
			if (strcmp(#struct_name, node->name) == 0) \
				bfStaticState.struct_name = target_expr = read_##type(node);

		#define BF_DYNAMIC_STATE(type, struct_name, target_expr) \
			if (strcmp(#struct_name, node->name) == 0) \
				bfInitialDynamicState.struct_name = target_expr = read_##type(node);

		#include "bruteforce/bf_state_definitions.inc.c"

		#undef BF_STATIC_STATE
		#undef BF_DYNAMIC_STATE

		node = node->next;
	}
	free(fileContents);
	Json_dispose(root);
	return TRUE;
}

void bf_load_dynamic_state(BFDynamicState *state) {
	#define BF_STATIC_STATE(_0, _1, _2)
	#define BF_DYNAMIC_STATE(type, struct_name, target_expr) \
		target_expr = state->struct_name;
	
	#include "bruteforce/bf_state_definitions.inc.c"

	#undef BF_STATIC_STATE
	#undef BF_DYNAMIC_STATE
}