#ifndef BF_STATE_H
#define BF_STATE_H

#include <PR/ultratypes.h>

typedef struct
{
	f64 bestScore;
} ProgramState;

typedef struct
{
	f32 hSpeed;
} CandidateStats;

#endif // BF_STATE_H