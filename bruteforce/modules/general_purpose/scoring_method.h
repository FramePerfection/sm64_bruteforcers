#ifndef SCORING_METHOD_H
#define SCORING_METHOD_H

#include <PR/ultratypes.h>
#include "bruteforce/framework/candidates.h"
#include "bruteforce/framework/json.h"

typedef f64 (*scoringFunc)(void*, Candidate*, u8*, u8*);

typedef struct ScoringMethod_s {
	f64 weight;
	void *args;
	scoringFunc func;
} ScoringMethod;

typedef struct ScoringMethods_s {
	u32 n_methods;
	ScoringMethod *methods;
} ScoringMethods;

void read_ScoringMethods(Json *jsonNode, ScoringMethods *target);

void applyMethod(ScoringMethod *method, Candidate *candidate, u8 *success, u8 *abort);


#define PARAM_MEMBER(TYPE, MEMBER_NAME) TYPE MEMBER_NAME;
#define SCORING_FUNC(NAME) \
typedef struct { \
 PARAM_MEMBERS_##NAME \
} *NAME##Parameters; \
 \
f64 sm_##NAME(NAME##Parameters parameters, Candidate *candidate, u8 *success, u8 *abort); \
void read_##NAME##Parameters(Json*, NAME##Parameters*);

#include "scoring_funcs.inc.c"

#undef PARAM_MEMBER
#undef SCORING_FUNC

#endif // SCORING_METHOD_H