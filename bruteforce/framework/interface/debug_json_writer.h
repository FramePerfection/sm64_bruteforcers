
#include "bruteforce/framework/states.h"

#define DEBUG_JSON_BUFFER_SIZE 1000000

typedef struct BFDebugJsonWriter_s {
	unsigned int cursor;
	char hasWritten;
	char buffer[DEBUG_JSON_BUFFER_SIZE];
} BFDebugJsonWriter;

BFDebugJsonWriter *bf_create_debug_json_writer();
void bf_append_debug_json(BFDebugJsonWriter *writer, BFDynamicState *state);
void bf_output_debug_json(BFDebugJsonWriter *writer, char *file_name);