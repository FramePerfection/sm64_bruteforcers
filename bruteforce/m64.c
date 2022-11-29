#include "bruteforce/m64.h"
#include <stdio.h>

u32 min(u32 x, u32 y) {
	return ((x) < (y)) ? (x) : (y);
}

u8 read_m64_from_file(char *fileName, u32 offset, u32 max_count, InputSequence **inputs) {
	FILE *file = fopen(fileName, "r");
	if (!file)
		return FALSE;
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0x400 + offset * sizeof(u32), SEEK_SET);
	u32 buffersize = sizeof(InputSequence) + max_count * sizeof(u32);
	*inputs = malloc(buffersize);
	(*inputs)->count = min(max_count, (size - 0x400) / 4);
	(*inputs)->offset = offset;

	char *inputBuffer = malloc(buffersize);
	fread(inputBuffer, sizeof(u32), (*inputs)->count, file);

	u32 i;
	for (i = 0; i < (*inputs)->count; i++) {
		u32 base = i * 4;
		s8 stick_y = inputBuffer[base + 3];
		s8 stick_x = inputBuffer[base + 2];
		u8 buttons_lo = inputBuffer[base + 1];
		u8 buttons_hi = inputBuffer[base + 0];
		(*inputs)->inputs[i].stick_x = stick_x;
		(*inputs)->inputs[i].stick_y = stick_y;
		(*inputs)->inputs[i].button = (buttons_hi << 8) | buttons_lo;
	}

	fclose(file);
	return TRUE;
}