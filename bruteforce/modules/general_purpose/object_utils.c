
#include "bruteforce/modules/general_purpose/object_utils.h"

#include "bruteforce/framework/readers.h"
#include "bruteforce/framework/json.h"
#include <string.h>
#include <stdlib.h>

void bf_read_BehaviorScriptWrapper(Json *jsonNode, BehaviorScriptWrapper *target) {
	// Assumes Json-node type Array
	if (jsonNode->type == Json_Array)
	{;}
	target->size = jsonNode->size;
	target->data = malloc(sizeof(BehaviorScript) * target->size);
	Json *child = jsonNode->child;
	u32 i = 0;
	while (child != NULL) {
		long lal = (BehaviorScript)bf_readers_util_read_int(child);
		target->data[i++] = lal;
		child = child->next;
	}
}

void bf_read_ObjectTriangles(Json *jsonNode, ObjectTriangles *target) {
	// Assumes Json-node type Object
	Json *prop = jsonNode->child;
	while (prop != NULL) {
		if (strcmp(prop->name, "overrides") == 0)
			target->overrides = bf_readers_util_read_int(prop);
		else if (strcmp(prop->name, "data") == 0) {
			target->data = malloc(sizeof(s16) * prop->size);
			Json *child = prop->child;
			u32 i = 0;
			while (child != NULL) {
				target->data[i++] = (s16)bf_readers_util_read_int(child);
				child = child->next;
			}
		}
		prop = prop->next;
	}
}