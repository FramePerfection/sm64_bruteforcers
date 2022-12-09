#ifdef BF_STATE_INCLUDE

#else

BF_STATIC_STATE(s32, max_processes, bfStaticState.max_processes)
BF_STATIC_STATE(u32, rnd_seed, bfStaticState.rnd_seed)
BF_STATIC_STATE(u32, max_perturbation, bfStaticState.max_perturbation)
BF_STATIC_STATE(u32, runs_per_survivor, bfStaticState.runs_per_survivor)
BF_STATIC_STATE(u32, survivors_per_generation, bfStaticState.survivors_per_generation)
BF_STATIC_STATE(u32, max_generations, bfStaticState.max_generations)
BF_STATIC_STATE(u32, forget_rate, bfStaticState.forget_rate)
BF_STATIC_STATE(u32, print_interval, bfStaticState.print_interval)
BF_STATIC_STATE(u32, merge_interval, bfStaticState.merge_interval)
BF_STATIC_STATE(u32, display_desync_messages, bfStaticState.display_desync_messages)

BF_STATIC_STATE(string, m64_input, bfStaticState.m64_input)
BF_STATIC_STATE(string, m64_output, bfStaticState.m64_output)
BF_STATIC_STATE(u32, m64_start, bfStaticState.m64_start)
BF_STATIC_STATE(u32, m64_count, bfStaticState.m64_count)

#endif