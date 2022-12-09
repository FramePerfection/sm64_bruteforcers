#ifndef CANDIDATES_H
#define CANDIDATES_H

#define STATE_INCLUDE <bruteforce/MODULE_PATH/state.h>

#include "bruteforce/m64.h"
#include <PR/ultratypes.h>
#include <stdio.h>
#include STATE_INCLUDE


extern u8 desynced;

void desync(char *message);

typedef struct Candidate_s {
	InputSequence *sequence;
	struct Candidate_s *children;
	f64 score;
	CandidateStats stats;
} Candidate;

void initCandidates(InputSequence *original_inputs, Candidate **survivors);
void updateBestCandidates(Candidate *survivors);
void mergeCandidates(Candidate *survivors, Candidate **externalSurvivors, u32 externalSurvivorsCount);

#endif //CANDIDATES_H