#include "sm64.h"
#include <stdlib.h>
#include <string.h>
#include "bruteforce/json.h"
#include "bruteforce/readers.h"

#define read_int(cast_type) \
void read_##cast_type(Json *jsonNode, cast_type *target) { \
	if (jsonNode->type == Json_String) { \
		char* end; \
		*target = (cast_type)strtol(jsonNode->valueString, &end, 0); \
	} \
	else *target = (cast_type)jsonNode->valueInt; \
}

read_int(s32)
read_int(s16)
read_int(u32)
read_int(u16)
read_int(u8)
#undef read_int

void read_f32(Json *jsonNode, f32 *target) {
	*target = (f32)jsonNode->valueFloat;
}

void read_string(Json *jsonNode, string *target) {
	*target = strdup(jsonNode->valueString);
}

f32 advance_read(Json **nodePtr) {
	f32 val = (*nodePtr)->valueFloat;
	*nodePtr = (*nodePtr)->next;
	return val;
}

void read_Vec3f(Json *jsonNode, Vec3f *target) {
	(*target)[0] = advance_read(&jsonNode);
	(*target)[1] = advance_read(&jsonNode);
	(*target)[2] = advance_read(&jsonNode);
}

void read_Triangles(Json *jsonNode, Triangles *target) {
	Json *triNode = jsonNode->child;
	target->data_size = jsonNode->size;
	target->data = malloc(sizeof(Triangle) * target->data_size);
	u32 i = 0;
	while (triNode != NULL) {
		Json *vertNode = triNode->child;
		#define ADVANCE \
			(s16)vertNode->valueInt; \
			vertNode = vertNode->next;
		target->data[i].x1 = ADVANCE
		target->data[i].y1 = ADVANCE
		target->data[i].z1 = ADVANCE
		target->data[i].x2 = ADVANCE
		target->data[i].y2 = ADVANCE
		target->data[i].z2 = ADVANCE
		target->data[i].x3 = ADVANCE
		target->data[i].y3 = ADVANCE
		target->data[i].z3 = ADVANCE
		i++;
		triNode = triNode->next;
	}
}