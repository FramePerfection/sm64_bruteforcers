
#include "debug_json_writer.h"

#include "types.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PRINT_NUMBER(TYPE, FORMAT) \
static int bf_print_##TYPE(char *target, TYPE value) { \
    return sprintf(target, FORMAT, value); \
}

PRINT_NUMBER(s8, "%d")
PRINT_NUMBER(s16, "%d")
PRINT_NUMBER(s32, "%d")
PRINT_NUMBER(u8, "%d")
PRINT_NUMBER(u16, "%d")
PRINT_NUMBER(u32, "%d")
PRINT_NUMBER(f32, "%f")
PRINT_NUMBER(f64, "%f")

#undef PRINT_NUMBER

BFDebugJsonWriter *bf_create_debug_json_writer() {
	BFDebugJsonWriter *result = calloc(1, sizeof(BFDebugJsonWriter));
	result->cursor += sprintf(result->buffer, "[\n");
    return result;
}

void bf_append_debug_json(BFDebugJsonWriter *writer, BFDynamicState *state) {
        if (writer->hasWritten)
            writer->cursor += sprintf(writer->buffer + writer->cursor, ",\n");
		writer->hasWritten = 1;

        writer->cursor += sprintf(writer->buffer + writer->cursor, "\"global_timer_%d\": {\n", gGlobalTimer);

        writer->cursor += sprintf(writer->buffer + writer->cursor, "\t\"frame_number\": %d,\n", gGlobalTimer);

#define BF_DYNAMIC_STATE(type, name, target_expression, documentation) \
            writer->cursor += sprintf(writer->buffer + writer->cursor, "\t\"%s\": ", #name); \
            writer->cursor += bf_print_##type(writer->buffer + writer->cursor, state->name); \
            writer->cursor += sprintf(writer->buffer + writer->cursor, ",\n");
#define BF_STATIC_STATE(type, name, target_expression, documentation)
#define BF_CONTROL_STATE(type, name, documentation)

#include STATE_DEFINITION_FILE

#undef BF_CONTROL_STATE
#undef BF_DYNAMIC_STATE
#undef BF_STATIC_STATE

        writer->cursor += sprintf(writer->buffer + writer->cursor - 2, "\n}") - 2;
}

void bf_output_debug_json(BFDebugJsonWriter *writer, char *file_name) {
	FILE *file = fopen(file_name, "w");
	fprintf(file, writer->buffer);
	fprintf(file, "\n]");
	fclose(file);
}
