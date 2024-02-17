#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "src/engine/surface_collision.h"
#include "src/engine/surface_load.h"
#include "src/engine/math_util.h"
#include "src/game/area.h"
#include "src/game/camera.h"
#include "src/game/game_init.h"
#include "src/game/level_update.h"
#include "src/game/mario.h"
#include "src/game/mario_step.h"
#include "src/game/object_list_processor.h"
#include "src/game/spawn_object.h"
#include "include/types.h"

#include "bruteforce/framework/interface/interface.h"
#include "bruteforce/framework/interface/m64.h"

#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/misc_util.h"
#include "bruteforce/framework/states.h"
#include "bruteforce/framework/objects_interaction/object_utils.h"
#include "bruteforce/framework/objects_interaction/behavior_function_map.h"

extern BehaviorScript bhvMario[];

static void initGame()
{
    bf_init_camera();
    bf_init_area();
    bf_init_mario();

    bf_safe_printf("Loading configuration...\n");
    if (!bf_init_states())
    {
        bf_safe_printf("Failed to load configuration! Exiting...\n");
        exit(-1);
    }
    bf_safe_printf("m64 path is %s, %p, loaded from %s \n", bfStaticState.m64_input, &bfStaticState.m64_input[0], override_config_file);

    clear_static_surfaces();
    clear_dynamic_surfaces();
    bf_init_static_surfaces(bfStaticState.static_tris);

    // srand(bfStaticState.rnd_seed);

    init_camera(gCamera);
    // Still required to initialize something?
    execute_mario_action(gMarioState->marioObj);
    update_camera(gCurrentArea->camera);

    if (bfStaticState.update_objects) {
        clear_objects();
        gMarioState->marioObj = gMarioObject = create_object(bhvMario);
        vec3f_copy(gMarioObject->header.gfx.pos, gMarioState->pos);
        bf_remap_behavior_scripts(bfStaticState.behavior_scripts, bfStaticState.dynamic_object_tris);
    }
}

static void updateGame(OSContPad *input, UNUSED u32 frame_index)
{
    if (bfStaticState.update_objects && frame_index == 0)
        bf_reset_objects(bfStaticState.behavior_scripts, bfStaticState.object_states);

    bf_update_controller(input);
    adjust_analog_stick(gPlayer1Controller);
    
    if (bfStaticState.update_objects)
        area_update_objects();
    else
        execute_mario_action(gMarioState->marioObj);
    
    if (gCurrentArea != NULL)
    {
        update_camera(gCurrentArea->camera);
    }
    gGlobalTimer++;
}

void main(int argc, char *argv[])
{
    u32 i;

    bf_parse_command_line_args(argc, argv);

    bf_safe_printf("Running Bruteforcer...\n");
    initGame();

    bf_safe_printf("Loading m64...\n");
    InputSequence *original_inputs;
    if (!bf_read_m64_from_file(bfStaticState.m64_input, bfStaticState.m64_start, bfStaticState.m64_end, &original_inputs))
    {
        bf_safe_printf("Failed to load m64! (%s) Exiting...\n", bfStaticState.m64_input);
        exit(-1);
    }

    bf_load_dynamic_state(&bfInitialDynamicState);

    for (i = 0; i < bfStaticState.m64_end - bfStaticState.m64_start; i++) {
        updateGame(&original_inputs->inputs[i], i);
    }
}