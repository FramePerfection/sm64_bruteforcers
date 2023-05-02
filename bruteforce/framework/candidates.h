#ifndef CANDIDATES_H
#define CANDIDATES_H

#define STATE_INCLUDE <bruteforce/modules/MODULE_PATH/state.h>

#define STR1(x) #x
#define STR2(x) STR1(x)
#define MODULE_PATH_STR STR2(MODULE_PATH)

#include "bruteforce/framework/m64.h"
#include <PR/ultratypes.h>
#include <stdio.h>
#include STATE_INCLUDE

extern u8 desynced;

void bf_desync(char *message);

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
void bf_merge_candidates(Candidate *survivors, Candidate **externalSurvivors, u32 externalSurvivorsCount);

#endif // CANDIDATES_H