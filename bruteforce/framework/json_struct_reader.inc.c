/* Utility file to define json readers for composite structs.
Usage:

// In header (.h):
#define BF_JSON_STRUCT_NAME <StructName>
#define BF_JSON_STRUCT_DEFINITION_<StructName> \
	BF_JSON_STRUCT_FIELD(<type_1>, <name_1>) \
	BF_JSON_STRUCT_FIELD(<type_2>, <name_2>) \
	...
	BF_JSON_STRUCT_FIELD(<type_n>, <name_n>) \
#include "bruteforce/framework/json_struct.inc.c"

// In implementation (.c):
#define BF_JSON_STRUCT_READER_IMPLEMENTATION

#define BF_JSON_STRUCT_NAME <StructName1>
#include "bruteforce/framework/json_struct.inc.c"
#define BF_JSON_STRUCT_NAME <StructName2>
#include "bruteforce/framework/json_struct.inc.c"
...
#define BF_JSON_STRUCT_NAME <StructNameN>
#include "bruteforce/framework/json_struct.inc.c"

*/

#ifndef BF_JSON_STRUCT_NAME
#error BF_JSON_STRUCT_NAME must be defined
#endif

#include "bruteforce/framework/json.h"
#include "bruteforce/framework/interprocess.h"

#define CONCAT(A, B) A##B
#define CONCAT2(A, B, C) A##B##C
#define BF_UNROLL_JSON_STRUCT_DEFINITION(STRUCT_NAME) CONCAT(BF_JSON_STRUCT_DEFINITION_, STRUCT_NAME)

#ifndef BF_JSON_STRUCT_READER_IMPLEMENTATION
// struct
#define BF_JSON_STRUCT_FIELD(FIELD_TYPE, FIELD_NAME) \
	FIELD_TYPE FIELD_NAME;
#define BF_BUILD_JSON_STRUCT(STRUCT_NAME) \
typedef struct CONCAT(STRUCT_NAME, _s) { \
	BF_UNROLL_JSON_STRUCT_DEFINITION(STRUCT_NAME) \
} STRUCT_NAME; \
void CONCAT(bf_read_, STRUCT_NAME)(Json *jsonNode, STRUCT_NAME *target);

BF_BUILD_JSON_STRUCT(BF_JSON_STRUCT_NAME)
#undef BF_JSON_STRUCT_FIELD

#else
// reader function
#define BF_JSON_STRUCT_FIELD(FIELD_TYPE, FIELD_NAME) \
	if (strcmp(child->name, #FIELD_NAME) == 0) { \
		bf_read_##FIELD_TYPE(child, &target->FIELD_NAME); \
	}

#define BF_BUILD_JSON_READER(STRUCT_NAME) \
void CONCAT(bf_read_, STRUCT_NAME)(Json *jsonNode, STRUCT_NAME *target) { \
	Json *child = jsonNode->child; \
	while (child != 0) { /* Can't use NULL here lmfao */ \
		BF_UNROLL_JSON_STRUCT_DEFINITION(STRUCT_NAME) \
		child = child->next; \
	} \
\
};

BF_BUILD_JSON_READER(BF_JSON_STRUCT_NAME)

#undef BF_JSON_STRUCT_FIELD

#endif