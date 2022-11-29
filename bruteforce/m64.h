#ifndef M64_H
#define M64_H

#include <PR/ultratypes.h>
#include <PR/os_cont.h>

typedef struct InputSequence_s {
	u32 offset;
	u32 count;
	OSContPad inputs[0];
} InputSequence;

u8 read_m64_from_file(char *fileName, u32 offset, u32 max_count, InputSequence **inputs);

#endif // M64_H