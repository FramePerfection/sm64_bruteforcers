
#ifndef BF_JSON_STRUCT_NAME
#error BF_JSON_STRUCT_NAME must be defined
#else

#undef BF_BUILD_JSON_STRUCT_ARRAY
#undef BF_BUILD_JSON_ARRAY_READER

#include "json.h"

#define CONCAT(A, B) A##B
#define CONCAT2(A, B, C) A##B##C
#define CONCAT3(A, B, C, D) A##B##C##D

#ifndef BF_JSON_STRUCT_READER_IMPLEMENTATION

#ifndef BF_JSON_ARRAY_SIZE

#define BF_BUILD_JSON_STRUCT_ARRAY(STRUCT_NAME) \
typedef struct CONCAT(STRUCT_NAME, Array_s) { \
STRUCT_NAME *data; \
u32 size; \
} CONCAT(STRUCT_NAME, Array); \
void CONCAT2(bf_read_, STRUCT_NAME, Array)(Json *jsonNode, CONCAT(STRUCT_NAME, Array) *target);

BF_BUILD_JSON_STRUCT_ARRAY(BF_JSON_STRUCT_NAME)
#else

#define BF_BUILD_JSON_STRUCT_ARRAY(STRUCT_NAME, ARRAY_SIZE) \
typedef STRUCT_NAME CONCAT2(STRUCT_NAME, Array, ARRAY_SIZE)[ARRAY_SIZE]; \
void CONCAT3(bf_read_, STRUCT_NAME, Array, ARRAY_SIZE)(Json *jsonNode, CONCAT2(STRUCT_NAME, Array, ARRAY_SIZE) *target);

BF_BUILD_JSON_STRUCT_ARRAY(BF_JSON_STRUCT_NAME, BF_JSON_ARRAY_SIZE)
#endif // BF_JSON_ARRAY_SIZE

#else

#ifndef BF_JSON_ARRAY_SIZE

#define BF_BUILD_JSON_ARRAY_READER(STRUCT_NAME) \
void CONCAT2(bf_read_, STRUCT_NAME, Array)(Json *jsonNode, CONCAT(STRUCT_NAME, Array) *target) { \
	target->size = jsonNode->size; \
    target->data = calloc(target->size, sizeof(STRUCT_NAME)); \
	Json *child = jsonNode->child; \
	u32 i = 0; \
	while (child != 0) { \
		CONCAT(bf_read_, STRUCT_NAME)(child, &target->data[i++]); \
		child = child->next; \
	} \
};

BF_BUILD_JSON_ARRAY_READER(BF_JSON_STRUCT_NAME)
#else

#define BF_BUILD_JSON_ARRAY_READER(STRUCT_NAME, ARRAY_SIZE) \
void CONCAT3(bf_read_, STRUCT_NAME, Array, ARRAY_SIZE)(Json *jsonNode, CONCAT2(STRUCT_NAME, Array, ARRAY_SIZE) *target) { \
	Json *child = jsonNode->child; \
	u32 i = 0; \
	while (child != 0 && i < ARRAY_SIZE) { \
		CONCAT(bf_read_, STRUCT_NAME)(child, (STRUCT_NAME*)&((*target)[i++])); \
		child = child->next; \
	} \
};

BF_BUILD_JSON_ARRAY_READER(BF_JSON_STRUCT_NAME, BF_JSON_ARRAY_SIZE)
#endif // BF_JSON_ARRAY_SIZE

#endif // BF_JSON_STRUCT_READER_IMPLEMENTATION

#undef BF_JSON_ARRAY_SIZE

#endif // BF_JSON_STRUCT_NAME
