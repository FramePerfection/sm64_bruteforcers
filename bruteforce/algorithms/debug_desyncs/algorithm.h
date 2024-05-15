#ifndef BF_ALGORITHMS_DEBUG_DESYNCS_H
#define BF_ALGORITHMS_DEBUG_DESYNCS_H

#include "bruteforce/framework/interface/m64.h"

typedef void (*UpdateGameFunc)(OSContPad *inputs, u32 frame_index);

void bf_debug_desyncs(InputSequence *inputs, char *outputPath, UpdateGameFunc updateGame);
s32 bf_read_default_m64_and_debug_desyncs(InputSequence **inputs, UpdateGameFunc updateGame);

#endif // BF_ALGORITHMS_DEBUG_DESYNCS_H