#define CREATE_PARAM_DEFINITION_FILE
#define __CAT(A) /#A
#define __COMMENT __CAT(/)

#define PARAM_MEMBER(TYPE, NAME, COMMENT) __NL__\
	TYPE NAME; __COMMENT COMMENT

#define SCORING_FUNC(NAME) NAME: PARAM_MEMBERS_##NAME __NL__ __NL__

#include "bruteforce/modules/general_purpose/scoring_funcs.inc.c"

#undef CREATE_PARAM_DEFINITION_FILE