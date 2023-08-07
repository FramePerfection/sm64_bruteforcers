
#include "bruteforce/modules/general_purpose/object_utils.h"

#include "bruteforce/framework/readers.h"
#include "bruteforce/framework/json.h"
#include <string.h>
#include <stdlib.h>

void bf_read_BehaviorScript(Json *jsonNode, BehaviorScript *target) {
	*target = (BehaviorScript)bf_readers_util_read_int(jsonNode);
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

#define BF_JSON_STRUCT_READER_IMPLEMENTATION

#include "json_structs.inc.h"