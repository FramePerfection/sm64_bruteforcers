#ifndef BF_M64_H
#define BF_M64_H

#include <PR/os_cont.h>
#include <PR/ultratypes.h>
#include <stdio.h>

typedef struct InputSequence_s
{
    u32 offset;
    u32 count;
    OSContPad originalInput;
    OSContPad inputs[0];
} InputSequence;

void bf_fwrite_hex32string(FILE *file, u8 input_buffer[4]);
u8 bf_read_m64_from_file(char *fileName, u32 offset, u32 max_count, InputSequence **inputs);
void bf_clone_m64_inputs(InputSequence *dest, InputSequence *src);
InputSequence *bf_clone_m64(InputSequence *src);
u8 bf_fwrite_input_sequence(FILE *file, InputSequence *sequence);
u8 bf_save_to_m64_file(char *originalFileName, char *fileName, InputSequence *sequence);

#endif // BF_M64_H