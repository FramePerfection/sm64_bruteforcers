#include "sm64.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bruteforce/framework/json.h"
#include "bruteforce/framework/readers.h"

// clang-format off

#define read_int(cast_type) \
void bf_read_##cast_type(Json *jsonNode, cast_type *target) { \
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


void bf_read_f32(Json *jsonNode, f32 *target)
{
	// clang-format on
	*target = (f32)jsonNode->valueFloat;
}

void bf_read_f64(Json *jsonNode, f64 *target)
{
	*target = (f64)jsonNode->valueFloat;
}

void bf_read_string(Json *jsonNode, string *target)
{
	*target = strdup(jsonNode->valueString);
}

void bf_read_boolean(Json *jsonNode, boolean *target)
{
	if (jsonNode->type == Json_String)
	{
		char *p = (char *)jsonNode->valueString;
		for (; *p; ++p)
			*p = tolower(*p);
		if (strcmp(jsonNode->valueString, "true") == 0)
			*target = 1;
		else if (strcmp(jsonNode->valueString, "false") == 0)
			*target = 0;
	}
	else if (jsonNode->type == Json_Number)
		*target = (jsonNode->valueInt != 0) ? 1 : 0;
}

static f32 s_advance_read(Json **nodePtr)
{
	f32 val = (*nodePtr)->valueFloat;
	*nodePtr = (*nodePtr)->next;
	return val;
}

void bf_read_Vec3f(Json *jsonNode, Vec3f *target)
{
	(*target)[0] = s_advance_read(&jsonNode);
	(*target)[1] = s_advance_read(&jsonNode);
	(*target)[2] = s_advance_read(&jsonNode);
}

void bf_read_Triangles(Json *jsonNode, Triangles *target)
{
	Json *triNode = jsonNode->child;
	target->data_size = jsonNode->size;
	target->data = calloc(target->data_size, sizeof(Triangle));
	u32 i = 0;
	while (triNode != NULL)
	{
		Json *vertNode = triNode->child;
		// clang-format off

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
		if (vertNode) {
			target->data[i].surf_type = ADVANCE
		}
		i++;
		triNode = triNode->next;

		#undef ADVANCE
		// clang-format on
	}
}

void bf_read_EnvironmentRegions(Json *jsonNode, EnvironmentRegions *target)
{
	*target = calloc(jsonNode->size, sizeof(s16));
	s32 i;
	Json *n = jsonNode->child;
	for (i = 0; i < jsonNode->size; i++)
	{
		(*target)[i] = n->valueInt;
		n = n->next;
	}
}

void bf_read_HitBoxes(Json *jsonNode, HitBoxes *target)
{
	Json *n = jsonNode->child;
	target->data_size = jsonNode->size;
	target->data = calloc(target->data_size, sizeof(HitBox));
	u32 i = 0;
	while (n != NULL)
	{
		Json *hitbox = n->child;
		// clang-format off

		#define ADVANCE \
			(s16)hitbox->valueInt; \
			hitbox = hitbox->next;
		
		target->data[i].x = ADVANCE
		target->data[i].y = ADVANCE
		target->data[i].z = ADVANCE
		target->data[i].radius = ADVANCE
		target->data[i].above = ADVANCE
		target->data[i].below = ADVANCE

		i++;
		n = n->next;

		#undef ADVANCE
		// clang-format on
	}
}