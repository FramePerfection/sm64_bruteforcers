#include "sm64.h"
#include <stdlib.h>
#include "bruteforce/json.h"
#include "bruteforce/readers.h"

#define read_int(cast_type) \
cast_type read_##cast_type(Json *jsonNode) { \
	return (cast_type)jsonNode->valueInt; \
}

read_int(s32)
read_int(s16)
read_int(u32)
read_int(u16)
read_int(u8)
#undef read_int

f32 read_f32(Json *jsonNode) {
	return (f32)jsonNode->valueFloat;
}

u32 read_u32hex(Json *jsonNode) {
	char* end;
	return (u32)strtol(jsonNode->valueString, &end, 0);
}

Triangles read_Triangles(Json *jsonNode) {
	Triangles t;
	Json *triNode = jsonNode->child;
	t.data_size = jsonNode->size;
	t.data = malloc(sizeof(Triangle) * t.data_size);
	u32 i = 0;
	while (triNode != NULL) {
		Json *vertNode = triNode->child;
		#define ADVANCE \
			(s16)vertNode->valueInt; \
			vertNode = vertNode->next;
		t.data[i].x1 = ADVANCE
		t.data[i].y1 = ADVANCE
		t.data[i].z1 = ADVANCE
		t.data[i].x2 = ADVANCE
		t.data[i].y2 = ADVANCE
		t.data[i].z2 = ADVANCE
		t.data[i].x3 = ADVANCE
		t.data[i].y3 = ADVANCE
		t.data[i].z3 = ADVANCE
		i++;
		triNode = triNode->next;
	}
	return t;
}