#ifdef BF_STATE_INCLUDE

#else

BF_STATIC_STATE(u32, runs_per_survivor, bfStaticState.runs_per_survivor, "The number of best runs to perform for each survivor of the previous generation.")
BF_STATIC_STATE(u32, survivors_per_generation, bfStaticState.survivors_per_generation, "The number of runs to keep at the end of each generation")
BF_STATIC_STATE(u32, max_generations, bfStaticState.max_generations, "The maximum number of generations to iterate through. Input a very large number for a basically endless mode.")
BF_STATIC_STATE(f32, forget_rate, bfStaticState.forget_rate, "The rate at which a survivor will be 'forgotten', from 0.0 to 1.0.")
BF_STATIC_STATE(f32, score_leniency, bfStaticState.score_leniency, "TODO: document this")
BF_STATIC_STATE(u32, print_interval, bfStaticState.print_interval, "The rate at which to print information about the current state of bruteforcing in seconds.")
BF_STATIC_STATE(u32, merge_interval, bfStaticState.merge_interval, "The rate at which processes should share their best runs with each other, and merge them down into one set of survivors.")

#endif