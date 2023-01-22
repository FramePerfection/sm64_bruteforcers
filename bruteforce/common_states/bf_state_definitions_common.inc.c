#ifdef BF_STATE_INCLUDE
extern char *m64_input;
extern char *m64_output;

#else
// general bruteforcing parameters
BF_STATIC_STATE(s32, max_processes, bfStaticState.max_processes)
BF_STATIC_STATE(u32, rnd_seed, bfStaticState.rnd_seed)
BF_STATIC_STATE(u32, max_perturbation, bfStaticState.max_perturbation)
BF_STATIC_STATE(f32, perturbation_chance, bfStaticState.perturbation_chance)
BF_STATIC_STATE(u32, display_desync_messages, bfStaticState.display_desync_messages)
__NL__

// m64 parameters
BF_STATIC_STATE(string, m64_input, m64_input)
BF_STATIC_STATE(string, m64_output, m64_output)
BF_STATIC_STATE(u32, m64_start, bfStaticState.m64_start)
BF_STATIC_STATE(u32, m64_count, bfStaticState.m64_count)
__NL__
__NL__

#endif