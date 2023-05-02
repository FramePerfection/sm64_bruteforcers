#ifndef STATE_H
#define STATE_H

#include <PR/ultratypes.h>

typedef struct
{
	f64 bestScore;
} ProgramState;

typedef struct
{
	f32 hSpeed;
} CandidateStats;

#endif // STATE_H