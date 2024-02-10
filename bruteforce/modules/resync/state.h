#ifndef BF_STATE_H
#define BF_STATE_H

#include <PR/ultratypes.h>

typedef struct
{
	f64 bestScore;
	u32 furthestActionMatch;
	u8 breakLoop;
} ProgramState;

typedef struct
{
	u32 frame_index;
	f64 lastScore;
} CandidateStats;

#endif // BF_STATE_H