#ifndef BF_ALGORITHMS_GENETIC_H
#define BF_ALGORITHMS_GENETIC_H

#include "bruteforce/framework/types.h"
#include "bruteforce/framework/candidates.h"

typedef void (*PerturbInputFunc)(Candidate *candidate, OSContPad *controller, u32 frame_index);
typedef void (*ScoringFunc)(Candidate *candidate, u32 frame_index, boolean *abort);
typedef void (*UpdateGameFunc)(OSContPad *inputs);

void bf_algorithm_genetic_loop(InputSequence *inputs, UpdateGameFunc updateGame, PerturbInputFunc perturbInput, ScoringFunc updateScore);

#endif // BF_ALGORITHMS_GENETIC_H