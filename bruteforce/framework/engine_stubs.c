/* This file contains stubs for functions that shall not be necessary to execute,
 but may be called from necessary functions.
 Examples are playing sound, rendering graphics etc.
 Note that anything you'll find here is essentially an expression of a TODO, 
 in the sense that anyhing that's been stubbed here might be a breaking change to the simulation's behavior.
*/

#include "include/macros.h"
#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "engine/graph_node.h"

#include "game/level_update.h"
#include "src/engine/math_util.h"
#include "include/object_fields.h"
#include "src/game/spawn_sound.h"

// external.c; data.c; sound_init.c
void play_sound(UNUSED s32 soundBits, UNUSED f32 *pos) {}
void set_sound_moving_speed(UNUSED u8 bank, UNUSED u8 speed) {}
void raise_background_noise(UNUSED s32 a) {}
void lower_background_noise(UNUSED s32 a) {}
void stop_cap_music(void) {}
void fadeout_cap_music(void) {}
void play_cap_music(UNUSED u16 seqArgs) {}
void play_infinite_stairs_music(void) {}
void play_music(UNUSED u8 player, UNUSED u16 seqArgs, UNUSED u16 fadeTimer) {}
void stop_sound(UNUSED u32 soundBits, UNUSED f32 *pos) {}
void stop_shell_music(void) {}
void play_shell_music(void) {}
void play_puzzle_jingle(void) {}
void fade_volume_scale(UNUSED u8 player, UNUSED u8 targetScale, UNUSED u16 fadeDuration) {}
void stop_background_music(UNUSED u16 seqId) {}
void enable_background_sound(void) {}
void disable_background_sound(void) {}
void play_peachs_jingle(void) {}
void play_cutscene_music(UNUSED u16 seqArgs) {}
void sound_banks_enable(UNUSED u8 player, UNUSED u16 bankMask) {}
void play_course_clear(void) {}
void func_80321080(UNUSED u16 fadeTimer) {}
void play_secondary_music(UNUSED u8 seqId, UNUSED u8 bgMusicVolume, UNUSED u8 volume, UNUSED u16 fadeTimer) {}
void play_power_star_jingle(UNUSED u8 arg0) {}
void seq_player_unlower_volume(UNUSED u8 player, UNUSED u16 fadeDuration) {}
void seq_player_fade_out(UNUSED u8 player, UNUSED u16 fadeDuration) {}
void seq_player_lower_volume(UNUSED u8 player, UNUSED u16 fadeDuration, UNUSED u8 percentage) {}
void stop_sounds_from_source(UNUSED f32 *pos) {}
void play_race_fanfare(void) {}
void drop_queued_background_music(void) {}
void fadeout_level_music(UNUSED s16 fadeTimer) {}

// memory.c
s32 load_patchable_table(UNUSED struct DmaHandlerList *list, UNUSED s32 index) { return FALSE; }

// level_update.c
s16 level_trigger_warp(UNUSED struct MarioState *m, UNUSED s32 warpOp) { return 0; }
void load_level_init_text(UNUSED u32 arg) {}
void fade_into_special_warp(UNUSED u32 arg, UNUSED u32 color) {}
u16 level_control_timer(UNUSED s32 timerOp) { return 0; }

// behavior_actions.c
void spawn_wind_particles(UNUSED s16 pitch, UNUSED s16 yaw) {}

// ingame_menu.c
s16 get_dialog_id(void) { return 0; }
void set_menu_mode(UNUSED s16 mode) {}
s8 gRedCoinsCollected; // Yes, it IS weird that this value is defined in that file

// rendering_graph_node.c
void geo_process_root(UNUSED struct GraphNodeRoot *node, UNUSED Vp *b, UNUSED Vp *c, UNUSED s32 clearColor) {}

// surface_load.c
WEAK void load_object_collision_model(void) {}

// spawn_sound.c
void cur_obj_play_sound_1(UNUSED s32 soundMagic) {}
void cur_obj_play_sound_2(UNUSED s32 soundMagic) {}
void exec_anim_sound_state(UNUSED struct SoundState *soundStates) {}

// TODO: implement this such that it creates an appropriate object
void create_sound_spawner(UNUSED s32 soundMagic) {}

// star_select.c
void bhv_act_selector_init(void) {}

// save_file.c
void save_file_do_save(UNUSED s32 fileIndex) {}
void save_file_set_flags(UNUSED u32 flags) {}
void save_file_clear_flags(UNUSED u32 flags) {}
u32 save_file_get_flags(void) { return 0; }
u32 save_file_get_star_flags(UNUSED s32 fileIndex, UNUSED s32 courseIndex) { return 0; }
void save_file_set_star_flags(UNUSED s32 fileIndex, UNUSED s32 courseIndex, UNUSED u32 starFlags) {}
s32 save_file_get_course_coin_score(UNUSED s32 fileIndex, UNUSED s32 courseIndex) { return 0; }
s32 save_file_is_cannon_unlocked(void) { return FALSE; }
void save_file_set_cannon_unlocked(void) {}
void save_file_set_cap_pos(UNUSED s16 x, UNUSED s16 y, UNUSED s16 z) {}
s32 save_file_get_cap_pos(UNUSED Vec3s capPos) { return 0; }
s32 save_file_get_total_star_count(UNUSED s32 fileIndex, UNUSED s32 minCourse, UNUSED s32 maxCourse) { return 0; }

// debug.c
void print_debug_bottom_up(UNUSED const char *str, UNUSED s32 number) {}
void print_debug_top_down_objectinfo(UNUSED const char *str, UNUSED s32 number) {}