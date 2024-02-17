#ifndef BF_SCORING_METHOD_H
#define BF_SCORING_METHOD_H

#include <PR/ultratypes.h>

#include "bruteforce/framework/interface/json.h"
#include "bruteforce/framework/interface/types.h"

#include "bruteforce/framework/quarter_steps.h"

#include "bruteforce/algorithms/genetic/candidates.h"

typedef f64 (*scoringFunc)(void *, Candidate *, u8 *, u8 *);

typedef struct ScoringMethod_s
{
    f64 weight;
    u32 frame;
    void *args;
    scoringFunc func;
} ScoringMethod;

typedef struct ScoringMethods_s
{
    u32 n_methods;
    ScoringMethod *methods;
} ScoringMethods;

void bf_read_ScoringMethods(Json *jsonNode, ScoringMethods *target);

void applyMethod(ScoringMethod *method, Candidate *candidate, u8 *success, u8 *abort);

#undef SCORING_FUNC_IMPL
#define PARAM_MEMBER(TYPE, MEMBER_NAME, _) TYPE MEMBER_NAME;
#define SCORING_FUNC(NAME)                                                                    \
    struct NAME##Parameters_s                                                                 \
    {                                                                                         \
        PARAM_MEMBERS_##NAME                                                                  \
    };                                                                                        \
    typedef struct NAME##Parameters_s *NAME##Parameters;                                      \
                                                                                              \
    f64 sm_##NAME(NAME##Parameters parameters, Candidate *candidate, u8 *success, u8 *abort); \
    void bf_read_##NAME##Parameters(Json *, NAME##Parameters *);

#include "scoring_funcs.inc.c"

#undef PARAM_MEMBER
#undef SCORING_FUNC

#endif // BF_SCORING_METHOD_H