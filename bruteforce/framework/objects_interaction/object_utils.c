
#include "bruteforce/framework/objects_interaction/object_utils.h"

#include "bruteforce/framework/interface/readers.h"
#include "bruteforce/framework/interface/json.h"

#include "bruteforce/framework/objects_interaction/behavior_function_map.h"
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

static void remapBehaviorScript(BehaviorScriptArray script, ObjectTrianglesArray object_triangles) {
    u32 i;
    for (i = 0; i < script.size; i++) {
        u8 command = script.data[i] >> 0x18; 
        if (command == 0x2A) { // LOAD_COLLISION_DATA
            // TODO: warn on mapping not found
            s16* mappedValue = 0;
            u32 k;
            for (k = 0; k < object_triangles.size; k++) {
                if (object_triangles.data[k].overrides == (BehaviorScript)script.data[i + 1]) {
                    mappedValue = object_triangles.data[k].data;
                    break;
                }
            }
            script.data[i + 1] = (BehaviorScript)mappedValue;
        }
        else if (command == 0x0C) { // EXECUTE_NATIVE_FUNC
            script.data[i + 1] = (BehaviorScript)bf_map_native_behavior_func((u32)script.data[i + 1]);
        }
    }
}

void bf_remap_behavior_scripts(BehaviorScriptArrayArray behavior_scripts, ObjectTrianglesArray object_triangles) {
    u32 i;
    for (i = 0; i < behavior_scripts.size; i++)
        remapBehaviorScript(behavior_scripts.data[i], object_triangles);
}


void bf_reset_objects(BehaviorScriptArrayArray behavior_scripts, BfObjectStateArray object_states) {
    clear_objects();
    clear_dynamic_surfaces();
    gMarioState->marioObj = gMarioObject = create_object(bhvMario);
    vec3f_copy(gMarioObject->header.gfx.pos, gMarioState->pos);

    u32 i;
    for (i = 0; i < object_states.size; i++) {
        BfObjectState *state = &object_states.data[i];
        struct Object *obj = create_object(behavior_scripts.data[state->behavior_script_index].data);
        memcpy(obj->rawData.asU32, &state->raw_data, sizeof(u32) * 0x50);
        obj->activeFlags = state->active_flags;
        obj->bhvDelayTimer = state->bhv_delay_timer;
        memcpy(obj->bhvStack, state->bhv_stack, sizeof(uintptr_t) * 8);
        obj->bhvStackIndex = state->bhv_stack_index;
        obj->collidedObjInteractTypes = state->collided_obj_interact_types;
        obj->hitboxRadius = state->hitbox_radius;
        obj->hitboxHeight = state->hitbox_height;
        obj->hurtboxRadius = state->hurtbox_radius;
        obj->hurtboxHeight = state->hurtbox_height;
        obj->hitboxDownOffset = state->hitbox_down_offset;
    }
}

#define BF_JSON_STRUCT_READER_IMPLEMENTATION
#include "json_structs.inc.h"