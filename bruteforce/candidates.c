#include "bruteforce/candidates.h"
#include "bruteforce/bf_states.h"
#include <memory.h>
#include "sm64.h"

void initCandidates(InputSequence *original_inputs, Candidate **survivors, Candidate ***best) {
	*survivors = malloc(sizeof(Candidate) * bfStaticState.survivors_per_generation);
	*best = malloc(sizeof(Candidate*) * bfStaticState.survivors_per_generation);

	u32 i;
	for (i = 0; i < bfStaticState.survivors_per_generation; i++) {
		(*survivors)[i].score = INFINITY;
		(*survivors)[i].children = malloc(sizeof(Candidate) * bfStaticState.runs_per_survivor);
		u32 k;
		for (k = 0; k < bfStaticState.runs_per_survivor; k++)
			(*survivors)[i].children[k].sequence = clone_m64(original_inputs);
		(*survivors)[i].sequence = clone_m64(original_inputs);
		(*best)[i] = &((*survivors)[i]);
	}
}

void insertSorted(Candidate **best, Candidate *new) {
	if (best[0] == NULL) {
		best[0] = new;
		return;
	}
	
	s32 rank;
	for (rank = bfStaticState.survivors_per_generation; rank > 0; rank--)
		if (best[rank-1] != NULL && best[rank-1]->score <= new->score)
			break;

	if (rank == bfStaticState.survivors_per_generation)
		return;

	s32 move_idx;
	for (move_idx = bfStaticState.survivors_per_generation - 1; move_idx > rank; move_idx--) {
		best[move_idx] = best[move_idx - 1];
	}
	best[rank] = new;
}

void updateBestCandidates(Candidate *survivors, Candidate **best) {
	u32 candidate_idx;
	for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++) {
		u32 run_idx = 0;
		for (run_idx = 0; run_idx < bfStaticState.runs_per_survivor; run_idx++)
			insertSorted(best, &(survivors[candidate_idx].children[run_idx]));
	}
	for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++) {
		clone_m64_inputs(survivors[candidate_idx].sequence, best[candidate_idx]->sequence);
		survivors[candidate_idx].score = best[candidate_idx]->score;
		memcpy(&survivors[candidate_idx].stats, &best[candidate_idx]->stats, sizeof(CandidateStats));
	}
}

u8 desynced;

void desync(char *message) {
	desynced = TRUE;
	if (bfStaticState.display_desync_messages)
		printf(message);
}