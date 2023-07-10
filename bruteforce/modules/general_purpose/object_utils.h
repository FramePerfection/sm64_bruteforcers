#ifndef OBJECT_UTILS_H
#define OBJECT_UTILS_H

#include "types.h"

typedef struct BehaviorScriptWrapper_s {
	u32 size;
	BehaviorScript *data;
} BehaviorScriptWrapper;

typedef struct ObjectTriangles_s {
	u32 overrides;
	s16 *data;
} ObjectTriangles;

#endif