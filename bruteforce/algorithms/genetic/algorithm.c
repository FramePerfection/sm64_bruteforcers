#include "bruteforce/algorithms/genetic/algorithm.h"

#include <time.h>

#include "src/game/game_init.h"

#include "bruteforce/framework/interface/interface.h"

#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/states.h"
#include "bruteforce/framework/misc_util.h"

static u32 sCandidateTransmissionSize;
static u32 sSequenceSize;
static u32 sCandidateTransmissionBufferSize;

// 1 bit for "merge buffer written", 1 bit each for "process written for next merge"
static u8* sMergeBits;
static unsigned int sNumMergeBytes;
static char *sTransmissionBuffer;

static void sWriteSurvivorsToBuffer(Candidate *survivors)
{
    char *sharedMemoryTarget = sTransmissionBuffer + bf_get_process_index() * sCandidateTransmissionBufferSize;
    u32 i;
    for (i = 0; i < bfStaticState.survivors_per_generation; i++)
    {
        char *where = sharedMemoryTarget + sCandidateTransmissionSize * i;
        memcpy(where, &survivors[i], sizeof(Candidate));
        ((Candidate*)where)->sequence = NULL;
        memcpy(where + offsetof(Candidate, sequence), survivors[i].sequence, sSequenceSize);
    }
    unsigned int procIndex = bf_get_process_index();
    sMergeBits[procIndex / 8] |= (1 << (procIndex % 8));
}

static u8 sMergeReady() {
    // early return if we're up to date
    if ((sMergeBits[0] & 1) != 0)
        return FALSE;

    unsigned int i;
    for (i = 1; i < bfStaticState.max_processes; i++) {
        if ((sMergeBits[i / 8] & (1 << (i % 8))) == 0)
            return FALSE;
    }
    return TRUE;
}

static void sParentRequestMergeCandidates()
{
    if (bfStaticState.max_processes <= 1)
        return;

    // Send a merge request to all children by clearing their "ready to merge" bits
    unsigned int i;
    for (i = 0; i < sNumMergeBytes; i++)
        sMergeBits[i] = 0;
}

static void sParentPerformMergeCandidates(Candidate *survivors) {
    bf_merge_candidates(survivors, sTransmissionBuffer + sCandidateTransmissionBufferSize, bfStaticState.survivors_per_generation * (bfStaticState.max_processes - 1), sCandidateTransmissionSize);
    sWriteSurvivorsToBuffer(survivors);

    // Notify child processes that the merge has been processed
    sMergeBits[0] = 1;
    unsigned int i;
    for (i = 1; i < sNumMergeBytes; i++)
        sMergeBits[i] = 0;
}

static void sChildMergeCandidates(Candidate *survivors) {
    bf_merge_candidates(survivors, sTransmissionBuffer, bfStaticState.survivors_per_generation, sCandidateTransmissionSize);

    // Notify parent process that the merge has been processed
    unsigned int i = bf_get_process_index();
    sMergeBits[i / 8] |= (1 << (i % 8));
}

void bf_algorithm_genetic_update_interprocess(Candidate *survivors) {
    if (bf_is_parent_process()) {
        static unsigned int sMergeCounter = 0;
        sMergeCounter++;
        if (sMergeCounter >= bfControlState->merge_interval) {
            sMergeCounter = 0;
            sParentRequestMergeCandidates();
        }

        if (sMergeReady())
            sParentPerformMergeCandidates(survivors);
    }
    else {
        unsigned int i = bf_get_process_index();
        if ((sMergeBits[i / 8] & (1 << (i % 8))) == 0) {
            if ((sMergeBits[0] & 1) == 0)
                sWriteSurvivorsToBuffer(survivors);
            else
                sChildMergeCandidates(survivors);
        }
    }
}

static void sInitMergeBits(char *data) {
    sMergeBits = (u8*)data;
    unsigned int i;
    for (i = 0; i < sNumMergeBytes; i++)
        sMergeBits[i] = 0xFF;

    // default to 25 if not set
    if (bfControlState->merge_interval == 0)
        bfControlState->merge_interval = 25;
}

static void sInitTransmissionBuffer(char *data) {
    sTransmissionBuffer = data;
}

void bf_algorithm_genetic_init(InputSequence *original_inputs) {
    sSequenceSize = offsetof(InputSequence, inputs) + original_inputs->count * sizeof(OSContPad);
    sCandidateTransmissionSize = ((sizeof(Candidate) + sSequenceSize + 7) / 8) * 8;
    sCandidateTransmissionBufferSize = sCandidateTransmissionSize * bfStaticState.survivors_per_generation;
    sNumMergeBytes = (bfStaticState.max_processes + 7) / 8;
    bf_reserve_shared_memory(sNumMergeBytes, sInitMergeBits);
    bf_reserve_shared_memory(bfStaticState.max_processes * sCandidateTransmissionBufferSize, sInitTransmissionBuffer);
}

void bf_algorithm_genetic_loop(
    InputSequence *original_inputs, 
    BFDynamicState *startingSavestate,
    UpdateGameFunc updateGame, 
    PerturbInputFunc perturbInput, 
    ScoringFunc updateScore, 
    PrintStateFunc printState, 
    BreakLoopFunc breakLoop)
{
    u8 resetToken = 0;
    Candidate *survivors;
    Candidate *best;
    bf_init_candidates(original_inputs, &survivors);
    bf_init_candidates(original_inputs, &best);

    clock_t lastClock = clock();

    u32 frames = 0;
    u32 gen;
    for (gen = 0; gen < bfStaticState.max_generations; gen++)
    {
        u8 newResetToken = bfControlState->reset_token;
        // grab the latest output from file if a reset was issued
        if (resetToken != newResetToken)
            bf_read_m64_from_file(
                bfStaticState.m64_output, 
                bfStaticState.m64_start, 
                bfStaticState.m64_end, 
                &original_inputs);

        if (breakLoop && breakLoop()) {
            bf_safe_printf("%d broke out of loop prematurely!\n", bf_get_process_index());
            break;
        }

        clock_t curClock = clock();
        float seconds = (float)(curClock - lastClock) / CLOCKS_PER_SEC;
        if (seconds >= bfControlState->print_interval)
        {
            float fps = frames / seconds;
            lastClock = curClock;
            frames = 0;
            if (printState != NULL)
                printState(gen, fps);
        }

        // perform all runs
        u32 candidate_idx;
        for (candidate_idx = 0; candidate_idx < bfStaticState.survivors_per_generation; candidate_idx++)
        {

            u32 run_idx;
            for (run_idx = 0; run_idx < bfStaticState.runs_per_survivor; run_idx++)
            {
                Candidate *candidate = &survivors[candidate_idx].children[run_idx];
                Candidate *survivor = &survivors[candidate_idx];

                InputSequence *inputs = candidate->sequence;
                if (resetToken != newResetToken)
                    bf_clone_m64_inputs(inputs, survivor->sequence);
                else
                    bf_clone_m64_inputs(inputs, original_inputs);

                u8 perturbRun = (resetToken == newResetToken) && (run_idx > 0 || (bf_random_float() > bfControlState->forget_rate));

                bf_load_dynamic_state(startingSavestate);
                gPlayer1Controller->buttonDown = inputs->originalInput.button;
                desynced = FALSE;

                u32 frame_idx;
                for (frame_idx = startingSavestate->global_timer - bfInitialDynamicState.global_timer; frame_idx < inputs->count; frame_idx++)
                {
                    frames++;
                    OSContPad *currentInput = &inputs->inputs[frame_idx];
                    if (!perturbRun)
                        perturbInput(candidate, currentInput, frame_idx);
                    updateGame(currentInput, frame_idx);
                    boolean abort = desynced;
                    updateScore(candidate, frame_idx, &abort);
                    if (abort)
                        break;
                }
            }

            resetToken = newResetToken;
        }

        // sort by scoring
        // TODO: allow for custom comparators
        bf_update_survivors(survivors);
        bf_update_best(best, survivors);

        bf_algorithm_genetic_update_interprocess(survivors);
    }
    // TODO: free candidates
}