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
	s16 angle;
	s16 pitch;
	f32 x;
	f32 y;
	f32 z;
} CandidateStats;

#endif // BF_STATE_H