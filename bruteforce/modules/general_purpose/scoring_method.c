#include "scoring_method.h"

#include "include/types.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "bruteforce/framework/interface/readers.h"

#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/states.h"

void bf_read_ScoringMethods(Json *jsonNode, ScoringMethods *target)
{
    target->n_methods = jsonNode->size;
    target->methods = calloc(jsonNode->size, sizeof(ScoringMethod));
    Json *outerNode = jsonNode->child;
    u32 i = 0;
    while (outerNode != NULL)
    {
        ScoringMethod *currentReadMethod = &(target->methods[i++]);
        currentReadMethod->weight = 1.0;
        Json *innerNode = outerNode->child;
        Json *paramsNode = NULL;
        const char *methodName = NULL;
        while (innerNode != NULL)
        {
            if (strcmp(innerNode->name, "weight") == 0)
            {
                currentReadMethod->weight = innerNode->valueFloat;
            }
            else if (strcmp(innerNode->name, "frame") == 0)
            {
                currentReadMethod->frame = innerNode->valueInt;
            }
            else if (strcmp(innerNode->name, "func") == 0)
            {
                methodName = innerNode->valueString;
                // clang-format off
				
				#define SCORING_FUNC(NAME) \
					if (strcmp(innerNode->valueString, #NAME) == 0) \
						currentReadMethod->func = (scoringFunc)&sm_##NAME;
				#include "scoring_funcs.inc.c"
				#undef SCORING_FUNC
                // clang-format on

                if (currentReadMethod->func == NULL)
                    bf_safe_printf("Warning: Unknown scoring function '%s'\n", methodName);
            }
            else if (strcmp(innerNode->name, "params") == 0)
                paramsNode = innerNode;

            innerNode = innerNode->next;
        }

        if (paramsNode)
        {
            // clang-format off

			#define SCORING_FUNC(NAME) \
				if (strcmp(methodName, #NAME) == 0) { \
					currentReadMethod->args = calloc(1, sizeof(struct NAME##Parameters_s)); \
					bf_read_##NAME##Parameters(paramsNode, (NAME##Parameters*)&(currentReadMethod->args)); \
				}
			#include "scoring_funcs.inc.c"
			#undef SCORING_FUNC
            // clang-format on
        }

        outerNode = outerNode->next;
    }
}

void applyMethod(ScoringMethod *method, Candidate *candidate, u8 *success, u8 *abort)
{
    if (!method->func)
        return;
    u8 partialSuccess = TRUE;
    f64 partialScore = method->func(method->args, candidate, &partialSuccess, abort);
    candidate->score += partialScore * method->weight;
    *success &= partialSuccess;
}

// scoringfuncs may not need to use their args field, so ignore the warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define SCORING_FUNC_IMPL
#include "scoring_funcs.inc.c"
#undef SCORING_FUNC_IMPL

#define PARAM_MEMBER(type, MEMBER_NAME, _)                            \
    if (strcmp(#MEMBER_NAME, innerNode->name) == 0)                   \
    {                                                                 \
        bf_read_##type(innerNode, (type *)&((*target)->MEMBER_NAME)); \
    }

#define SCORING_FUNC(NAME)                                                \
    void bf_read_##NAME##Parameters(Json *json, NAME##Parameters *target) \
    {                                                                     \
        Json *innerNode = json->child;                                    \
        while (innerNode != NULL)                                         \
        {                                                                 \
            PARAM_MEMBERS_##NAME                                          \
                innerNode = innerNode->next;                              \
        }                                                                 \
    }
#include "scoring_funcs.inc.c"

#undef SCORING_FUNC
#undef PARAM_MEMBER
#pragma GCC diagnostic pop