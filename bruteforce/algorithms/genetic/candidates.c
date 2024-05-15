#include "bruteforce/algorithms/genetic/candidates.h"

#include "bruteforce/framework/states.h"

#include "sm64.h"
#include <memory.h>
#include <stdlib.h>

Candidate **temp = NULL;

void bf_init_candidates(InputSequence *original_inputs, Candidate **survivors)
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
            (*survivors)[i].children[k].sequence = bf_clone_m64(original_inputs);
        (*survivors)[i].sequence = bf_clone_m64(original_inputs);
        if (assignTemp)
            temp[i] = &((*survivors)[i]);
    }
}

static void s_insert_sorted(Candidate *new)
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

static void s_clear_temp()
{
    u32 candidate_idx;
    for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
        temp[candidate_idx] = NULL;
}

static void s_apply_temp(Candidate *survivors)
{
    u32 candidate_idx;
    for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
    {
        bf_clone_m64_inputs(survivors[candidate_idx].sequence, temp[candidate_idx]->sequence);
        survivors[candidate_idx].score = temp[candidate_idx]->score;
        memcpy(&survivors[candidate_idx].stats, &temp[candidate_idx]->stats, sizeof(CandidateStats));
    }
}

void bf_update_survivors(Candidate *survivors)
{
    s_clear_temp();

    u32 candidate_idx;
    for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
    {
        u32 run_idx = 0;
        for (run_idx = 0; run_idx < bfStaticState.runs_per_survivor; run_idx++)
            s_insert_sorted(&(survivors[candidate_idx].children[run_idx]));
    }

    s_apply_temp(survivors);
}

void bf_merge_candidates(Candidate *survivors, Candidate *externalSurvivors, u32 externalSurvivorsCount, u32 stride)
{
    s_clear_temp();

    u32 candidate_idx;
    for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
    {
        s_insert_sorted(&survivors[candidate_idx]);
    }

    for (candidate_idx = 0; candidate_idx < externalSurvivorsCount; candidate_idx++)
    {
        Candidate *externalCandidate = (Candidate*)((char*)externalSurvivors + stride);
        externalCandidate->sequence = externalCandidate + sizeof(Candidate);
        s_insert_sorted(externalCandidate);
    }

    s_apply_temp(survivors);
}

void bf_update_best(Candidate *temp, Candidate *survivors)
{
    s_clear_temp();

    u32 candidate_idx;
    for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
    {
        s_insert_sorted(&temp[candidate_idx]);
    }

    for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
    {
        s_insert_sorted(&survivors[candidate_idx]);
    }

    s_apply_temp(temp);
}