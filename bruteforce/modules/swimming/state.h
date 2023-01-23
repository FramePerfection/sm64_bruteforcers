#ifndef STATE_H
#define STATE_H

#include <PR/ultratypes.h>

typedef struct {
	f32 bestScore;
} ProgramState;

typedef struct {
	f32 hSpeed;
	s16 angle;
	f32 x;
	f32 y;
	f32 z;
} CandidateStats;

#endif // STATE_H