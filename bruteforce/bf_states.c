#include "sm64.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bruteforce/readers.h"
#include "bruteforce/bf_states.h"
#include "bruteforce/json.h"
#include <getopt.h>

#define BF_STATE_INCLUDE
#include STATE_DEFINITION_FILE
#undef BF_STATE_INCLUDE


BFStaticState bfStaticState;
BFDynamicState bfInitialDynamicState;
ProgramState *programState;
char *override_config_file = NULL;
char *child_args = NULL;

void parse_command_line_args(int argc, char *argv[]) {
	int c, long_opt_idx = 0;
	static struct option long_opts[] = {
		{"file", required_argument, 0, 'f'},
		{"child", required_argument, 0, 'c'}
	};
	while ((c = getopt_long(argc, argv, "f:c:", long_opts, &long_opt_idx)) != -1) {
		switch (c) {
			case 'f':
				override_config_file = malloc(sizeof(char) * (strlen(optarg) + 1));
				strcpy(override_config_file, optarg);
				break;
			case 'c':
				child_args = malloc(sizeof(char) * (strlen(optarg) + 1));
				strcpy(child_args, optarg);
				break;
		}
	}

}

const char *read_file(const char *fileName) {
	FILE *file = fopen(fileName, "r");
	if (!file)
		return NULL;
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);
	const char *fileContents = calloc(size, sizeof(char));
	fread(fileContents, sizeof(char), size, file);
	fclose(file);
	return fileContents;
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
	
		#define BF_STATIC_STATE(type, struct_name, target_expr) \
			if (strcmp(#struct_name, node->name) == 0) \
			{ \
				read_##type(node, &bfStaticState.struct_name); \
				memcpy(&(target_expr), &bfStaticState.struct_name, sizeof bfStaticState.struct_name); \
			}

		#define BF_DYNAMIC_STATE(type, struct_name, target_expr) \
			if (strcmp(#struct_name, node->name) == 0) \
			{ \
				read_##type(node, &bfInitialDynamicState.struct_name); \
				memcpy(&(target_expr), &bfInitialDynamicState.struct_name, sizeof bfInitialDynamicState.struct_name); \
			}

		#include STATE_DEFINITION_FILE

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
		memcpy(&(target_expr), &state->struct_name, sizeof state->struct_name);
	
	#include STATE_DEFINITION_FILE

	#undef BF_STATIC_STATE
	#undef BF_DYNAMIC_STATE
}