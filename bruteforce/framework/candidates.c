#include "bruteforce/framework/candidates.h"
#include "bruteforce/framework/states.h"
#include "bruteforce/framework/interprocess.h"
#include <memory.h>
#include "sm64.h"

Candidate **temp = NULL;

void initCandidates(InputSequence *original_inputs, Candidate **survivors)
{
	u8 assignTemp = temp == NULL;
	*survivors = calloc(bfStaticState.survivors_per_generation, sizeof(Candidate));
	if (assignTemp)
		temp = calloc(bfStaticState.survivors_per_generation, sizeof(Candidate *));

	u32 i;
	for (i = 0; i < bfStaticState.survivors_per_generation; i++)
	{
		(*survivors)[i].score = -INFINITY;
		(*survivors)[i].children = calloc(bfStaticState.runs_per_survivor, sizeof(Candidate));
		u32 k;
		for (k = 0; k < bfStaticState.runs_per_survivor; k++)
			(*survivors)[i].children[k].sequence = clone_m64(original_inputs);
		(*survivors)[i].sequence = clone_m64(original_inputs);
		if (assignTemp)
			temp[i] = &((*survivors)[i]);
	}
}

void insertSorted(Candidate *new)
{
	if (temp[0] == NULL)
	{
		temp[0] = new;
		return;
	}

	s32 rank;
	for (rank = bfStaticState.survivors_per_generation; rank > 0; rank--)
		if (temp[rank - 1] != NULL && temp[rank - 1]->score >= new->score)
			break;

	if (rank == bfStaticState.survivors_per_generation)
		return;

	s32 move_idx;
	for (move_idx = bfStaticState.survivors_per_generation - 1; move_idx > rank; move_idx--)
	{
		temp[move_idx] = temp[move_idx - 1];
	}
	temp[rank] = new;
}

static void clearTemp()
{
	u32 candidate_idx;
	for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
		temp[candidate_idx] = NULL;
}

static void applyTemp(Candidate *survivors)
{
	u32 candidate_idx;
	for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
	{
		clone_m64_inputs(survivors[candidate_idx].sequence, temp[candidate_idx]->sequence);
		survivors[candidate_idx].score = temp[candidate_idx]->score;
		memcpy(&survivors[candidate_idx].stats, &temp[candidate_idx]->stats, sizeof(CandidateStats));
	}
}

void updateSurvivors(Candidate *survivors)
{
	clearTemp();

	u32 candidate_idx;
	for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
	{
		u32 run_idx = 0;
		for (run_idx = 0; run_idx < bfStaticState.runs_per_survivor; run_idx++)
			insertSorted(&(survivors[candidate_idx].children[run_idx]));
	}

	applyTemp(survivors);
}

void mergeCandidates(Candidate *survivors, Candidate **externalSurvivors, u32 externalSurvivorsCount)
{
	clearTemp();

	u32 candidate_idx;
	for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
	{
		insertSorted(&survivors[candidate_idx]);
	}

	for (candidate_idx = 0; candidate_idx < externalSurvivorsCount; candidate_idx++)
	{
		insertSorted(externalSurvivors[candidate_idx]);
	}

	applyTemp(survivors);
}

void updateBest(Candidate *temp, Candidate *survivors)
{
	clearTemp();

	u32 candidate_idx;
	for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
	{
		insertSorted(&temp[candidate_idx]);
	}

	for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
	{
		insertSorted(&survivors[candidate_idx]);
	}

	applyTemp(temp);
}

u8 desynced;

void desync(char *message)
{
	desynced = TRUE;
	if (bfStaticState.display_desync_messages)
		safePrintf(message);
}