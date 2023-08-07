#ifndef OBJECT_UTILS_H
#define OBJECT_UTILS_H

#include "types.h"
#include "bruteforce/framework/readers.h"

typedef struct ObjectTriangles_s {
	u32 overrides;
	s16 *data;
} ObjectTriangles;

#include "json_structs.inc.h"

#endif