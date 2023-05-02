#ifndef STATE_H
#define STATE_H

#include <PR/ultratypes.h>

typedef struct
{
	f64 bestScore;
} ProgramState;

typedef struct
{
	u32 frame_index;
} CandidateStats;

#endif // STATE_H