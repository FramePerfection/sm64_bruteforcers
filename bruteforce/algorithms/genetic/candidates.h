#ifndef BF_CANDIDATES_H
#define BF_CANDIDATES_H

#include "bruteforce/framework/m64.h"

#include <PR/ultratypes.h>
#include <stdio.h>

#define STATE_INCLUDE <bruteforce/modules/MODULE_PATH/state.h>
#include STATE_INCLUDE

#define STR1(x) #x
#define STR2(x) STR1(x)
#define MODULE_PATH_STR STR2(MODULE_PATH)

typedef struct Candidate_s
{
    InputSequence *sequence;
    struct Candidate_s *children;
    f64 score;
    CandidateStats stats;
} Candidate;

void bf_init_candidates(InputSequence *original_inputs, Candidate **survivors);
void bf_update_survivors(Candidate *survivors);
void bf_update_best(Candidate *best, Candidate *survivors);
void bf_merge_candidates(Candidate *survivors, Candidate *externalSurvivors, u32 externalSurvivorsCount, u32 stride);

#endif // BF_CANDIDATES_H