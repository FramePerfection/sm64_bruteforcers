#ifdef BF_STATE_INCLUDE
#include "bruteforce/framework/interface/types.h"
extern char *m64_input;
extern char *m64_output;
extern u32 gGlobalTimer;
extern struct Controller *gPlayer1Controller;

#else
// general bruteforcing parameters
BF_STATIC_STATE(boolean, debug_desyncs, bfStaticState.debug_desyncs, \
"When set, the input sequence in m64_input will be played back exactly once, __NL__\
and the bfDynamicState for each frame of the run will be written to debug_output.json")
BF_STATIC_STATE(s32, max_processes, bfStaticState.max_processes, "The number of processes to start. Should usually be equal to your logical core count minus one for other tasks.")
BF_STATIC_STATE(boolean, display_desync_messages, bfStaticState.display_desync_messages, \
"Display messages when the code reaches a section that may fail to produce accurate results. __NL__\
 This can happen a lot and the printing can slow down the process, so use it only if you suspect inaccuracies in the module.")
__NL__

// m64 parameters
BF_STATIC_STATE(string, m64_input, m64_input, "The .m64 file to take the base inputs from.")
BF_STATIC_STATE(string, m64_output, m64_output, "The .m64 file path to output the bruteforcing results to.")
BF_STATIC_STATE(u32, m64_start, bfStaticState.m64_start, "The 1-indexed input frame number of the frame to start on in the base .m64 file.")
BF_STATIC_STATE(u32, m64_end, bfStaticState.m64_end, "The 1-indexed input frame number of the frame to end on in the base .m64 file. __NL__\
Must be greater than m64_start.")
__NL__

BF_DYNAMIC_STATE(u32, global_timer, gGlobalTimer, "The global timer value on the frame represented by m64_start. Used to synchronize tools.")
__NL__

#endif