/* This file contains stubs for functions that shall not be necessary to execute, 
/ but may be called from necessary function.
/ Examples are playing sound, rendering graphics etc.
*/

#include <PR/ultratypes.h>
#include "src/engine/math_util.h"
#include "game/level_update.h"

// external.c; data.c; sound_init.c
u32 gAudioRandom;
f32 gGlobalSoundSource[3];
void play_sound(s32 soundBits, f32 *pos) {}
void set_sound_moving_speed(u8 bank, u8 speed) {}
void raise_background_noise(s32 a) {}
void lower_background_noise(s32 a) {}
void stop_cap_music(void) {}
void fadeout_cap_music(void) {}
void play_cap_music(u16 seqArgs) {}
void play_infinite_stairs_music(void) {}
void play_music(u8 player, u16 seqArgs, u16 fadeTimer) {}
void stop_sound(u32 soundBits, f32 *pos) {}

// memory.c
s32 load_patchable_table(struct DmaHandlerList *list, s32 index) { return FALSE; }

// level_update.c
s16 level_trigger_warp(struct MarioState *m, s32 warpOp) {}
struct HudDisplay gHudDisplay;

struct MarioState gMarioStates[1];
struct MarioState *gMarioState = &gMarioStates[0];

// interaction.c
void mario_stop_riding_and_holding(struct MarioState *m) {}
void mario_drop_held_object(struct MarioState *m) {}
void mario_stop_riding_object(struct MarioState *m) {}
u32 mario_check_object_grab(struct MarioState *m) {}
void mario_grab_used_object(struct MarioState *m) {}

// main.c
s8 gDebugLevelSelect = FALSE;

// behavior_actions.c
void spawn_wind_particles(s16 pitch, s16 yaw) {}

// ingame_menu.c
s16 get_dialog_id(void) {}