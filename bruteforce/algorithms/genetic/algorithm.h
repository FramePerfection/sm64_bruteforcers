#ifndef ALGORITHMS_GENETIC_H
#define ALGORITHMS_GENETIC_H

#include "bruteforce/framework/types.h"
#include "bruteforce/framework/candidates.h"

typedef void (*PerturbInputFunc)(Candidate *candidate, OSContPad *controller, u32 frame_index);
typedef void (*ScoringFunc)(Candidate *candidate, u32 frame_index, boolean *abort);
typedef void (*UpdateGameFunc)(OSContPad *inputs);

void bruteforce_loop_genetic(InputSequence *inputs, UpdateGameFunc updateGame, PerturbInputFunc perturbInput, ScoringFunc updateScore);

#endif // ALGORITHMS_GENETIC_H