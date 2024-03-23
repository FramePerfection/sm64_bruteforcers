#include "bruteforce/algorithms/debug_desyncs/algorithm.h"

#include "bruteforce/framework/interface/debug_json_writer.h"
#include "bruteforce/framework/interprocess.h"

#include "bruteforce/framework/states.h"

#include <stdlib.h>

void bf_debug_desyncs(InputSequence *inputs, char *outputPath, UpdateGameFunc updateGame) {
    u32 i;
    BFDynamicState state;
    BFDebugJsonWriter *debugWriter = bf_create_debug_json_writer();

    if (bfStaticState.debug_desyncs) {
        bf_load_dynamic_state(&bfInitialDynamicState);

        for (i = 0; i <= bfStaticState.m64_end - bfStaticState.m64_start; i++) {
            bf_save_dynamic_state(&state);
            
            bf_append_debug_json(debugWriter, &state);

            updateGame(&inputs->inputs[i], i);
        }
        bf_output_debug_json(debugWriter, outputPath);
    }
}

s32 bf_read_default_m64_and_debug_desyncs(InputSequence **inputs, UpdateGameFunc updateGame) {
    const s32 result = bf_read_m64_from_file(bfStaticState.m64_input, bfStaticState.m64_start, bfStaticState.m64_end, inputs);
    if (bfStaticState.debug_desyncs) {
        bf_debug_desyncs(*inputs, "debug_output.json", updateGame);
        bf_safe_printf("Wrote debug output to debug_output.json and terminated early.");
        exit(0);
    }
    return result;
}