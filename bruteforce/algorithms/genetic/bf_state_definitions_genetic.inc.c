#ifdef BF_STATE_INCLUDE

#else

BF_STATIC_STATE(u32, runs_per_survivor, bfStaticState.runs_per_survivor)
BF_STATIC_STATE(u32, survivors_per_generation, bfStaticState.survivors_per_generation)
BF_STATIC_STATE(u32, max_generations, bfStaticState.max_generations)
BF_STATIC_STATE(f32, forget_rate, bfStaticState.forget_rate)
BF_STATIC_STATE(f32, score_leniency, bfStaticState.score_leniency)
BF_STATIC_STATE(u32, print_interval, bfStaticState.print_interval)
BF_STATIC_STATE(u32, merge_interval, bfStaticState.merge_interval)

#endif