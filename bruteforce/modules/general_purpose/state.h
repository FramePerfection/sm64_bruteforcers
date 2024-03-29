#ifndef BF_STATE_H
#define BF_STATE_H

#include <PR/ultratypes.h>

typedef struct
{
    f64 bestScore;
} ProgramState;

typedef struct
{
    u32 frame_index;
} CandidateStats;

#endif // BF_STATE_H