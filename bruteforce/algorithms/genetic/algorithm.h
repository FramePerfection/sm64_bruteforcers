#ifndef BF_ALGORITHMS_GENETIC_H
#define BF_ALGORITHMS_GENETIC_H

#include "bruteforce/algorithms/genetic/candidates.h"
#include "bruteforce/framework/types.h"

typedef struct BFDynamicState_s BFDynamicState;

typedef void (*PerturbInputFunc)(Candidate *candidate, OSContPad *controller, u32 frame_index);
typedef void (*ScoringFunc)(Candidate *candidate, u32 frame_index, boolean *abort);
typedef void (*UpdateGameFunc)(OSContPad *inputs, u32 frame_index);
typedef void (*PrintStateFunc)(u32 generation, float fps);
typedef u8 (*BreakLoopFunc)();

void bf_algorithm_genetic_init(InputSequence *original_inputs);
void bf_algorithm_genetic_loop(
    InputSequence *original_inputs, 
	BFDynamicState *startingSavestate,
    UpdateGameFunc updateGame, 
    PerturbInputFunc perturbInput, 
    ScoringFunc updateScore, 
    PrintStateFunc printState, 
    BreakLoopFunc breakLoop);
void bf_algorithm_genetic_update_interprocess(Candidate *survivors);

#endif // BF_ALGORITHMS_GENETIC_H