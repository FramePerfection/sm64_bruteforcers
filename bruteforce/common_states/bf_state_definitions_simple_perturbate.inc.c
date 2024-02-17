#ifdef BF_STATE_INCLUDE
#include "bruteforce/framework/interface/types.h"
#else
BF_STATIC_STATE(u32, max_perturbation, bfStaticState.max_perturbation, "The maximum perturbation to apply on each perturbed frame between 0 and 255")
BF_STATIC_STATE(f32, perturbation_chance, bfStaticState.perturbation_chance, "The chance at which a frame will be perturbed, between 0.0 and 1.0")
#endif