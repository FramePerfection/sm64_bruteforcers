#include "bruteforce/framework/m64.h"
#include <string.h>
#include <stdlib.h>

u32 MIN(u32 x, u32 y) {
	return ((x) < (y)) ? (x) : (y);
}

u8 read_m64_from_file(char *fileName, u32 offset, u32 count, InputSequence **inputs) {
	FILE *file = fopen(fileName, "rb");
	if (!file)
		return FALSE;
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	
	u32 buffersize = sizeof(InputSequence) + count * sizeof(OSContPad);
	*inputs = calloc(buffersize, sizeof(char));
	(*inputs)->count = MIN(count, (size - 0x400) / 4);
	(*inputs)->offset = offset;

	OSContPad *inputSequence = &(*inputs)->inputs[0];
	if (offset > 0) {
		count++;
		offset--;
		inputSequence = &(*inputs)->originalInput;
	}

	fseek(file, 0x400 + offset * sizeof(u32), SEEK_SET);
	char *inputBuffer = calloc(count, sizeof(u32));
	fread(inputBuffer, sizeof(u32), count, file);

	u32 i;
	for (i = 0; i < count; i++) {
		u32 base = i * 4;
		s8 stick_y = inputBuffer[base + 3];
		s8 stick_x = inputBuffer[base + 2];
		u8 buttons_lo = inputBuffer[base + 1];
		u8 buttons_hi = inputBuffer[base + 0];

		OSContPad *targetInput = &inputSequence[i];
		targetInput->stick_x = stick_x;
		targetInput->stick_y = stick_y;
		targetInput->button = (buttons_hi << 8) | buttons_lo;
	}

	fclose(file);
	return TRUE;
}

u8 fwrite_input_sequence(FILE *file, InputSequence *sequence) {
	u32 i;
	for (i = 0; i < sequence->count; i++) {
		u8 input_buffer[] = {
			(char)(sequence->inputs[i].button >> 8),
			(char)(sequence->inputs[i].button & 0xFF), 
			(char)(sequence->inputs[i].stick_x & 0xFF),
			(char)(sequence->inputs[i].stick_y & 0xFF),
		};
		fprintf(file, "0x%lx ", (long unsigned int)(((u32)input_buffer[0]) << 0x18 | ((u32)input_buffer[1]) << 0x10 | ((u32)input_buffer[2]) << 0x8 | ((u32)input_buffer[3])));
	}
	fprintf(file, ";\n");
}

u8 save_to_m64_file(char* originalFileName, char* fileName, InputSequence *sequence) {
	FILE *src_file = fopen(originalFileName, "rb");
	FILE *dst_file = fopen(fileName, "wb");
	if (!src_file)
		return FALSE;
	if (!dst_file) {
		fclose(src_file);
		return FALSE;
	}

	fseek(src_file, 0, SEEK_END);
	long size = ftell(src_file);
	fseek(src_file, 0, SEEK_SET);

	char *buffer = calloc(size, sizeof(char));
	fread(buffer, sizeof(char), size, src_file);
	fwrite(buffer, sizeof(char), size, dst_file);

	fseek(dst_file, 0x400 + sizeof(u32) * sequence->offset, SEEK_SET);
	u32 i;
	for (i = 0; i < sequence->count; i++) {
		fputc((char)(sequence->inputs[i].button >> 8), dst_file); 
		fputc((char)(sequence->inputs[i].button & 0xFF), dst_file);
		fputc((char)(sequence->inputs[i].stick_x & 0xFF), dst_file);
		fputc((char)(sequence->inputs[i].stick_y & 0xFF), dst_file);
	}

	fclose(src_file);
	fclose(dst_file);
	free(buffer);
	return TRUE;
}

void clone_m64_inputs(InputSequence *dest, InputSequence *src) {
	memcpy(&dest->originalInput, &src->originalInput, sizeof(OSContPad));
	memcpy(dest->inputs, src->inputs, sizeof(OSContPad) * dest->count);
}

InputSequence *clone_m64(InputSequence *src) {
	InputSequence *dest = calloc(sizeof(InputSequence) + sizeof(OSContPad) * src->count, sizeof(char));
	dest->count = src->count;
	dest->offset = src->offset;
	clone_m64_inputs(dest, src);
	return dest;
}