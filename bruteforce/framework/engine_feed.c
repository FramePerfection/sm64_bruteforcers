/**
 * Variable definitions needed by (nearly) all modules that cut across files
 * in such a way that including those modules would seem unstructured or wasteful.
 */

#include <PR/ultratypes.h>

#include "sm64.h"
#include "src/engine/surface_load.h"
#include "game/level_update.h"

// object_list_processor.c
s16 gFindFloorIncludeSurfaceIntangible;
s16 gCheckingSurfaceCollisionsForCamera;
u32 gTimeStopState;

// surface_load.c
SpatialPartitionCell gStaticSurfacePartition[NUM_CELLS][NUM_CELLS];
SpatialPartitionCell gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS];

// game_init.c
u32 gGlobalTimer;
s16 *gEnvironmentRegions;
struct Controller gController;
struct Controller *gPlayer1Controller = &gController;

// save_file.c
u8 gSpecialTripleJump;
u8 gLastCompletedStarNum;
u8 gLastCompletedCourseNum;
s8 gNeverEnteredCastle;

// paintings.c
struct Painting *gRipplingPainting;

// area.c
s16 gCurrLevelNum = 1;
s16 gCurrCourseNum;
s16 gCurrActNum;
s16 gCurrAreaIndex;
struct Area *gCurrentArea = NULL;

// geo_layout.c
struct GraphNode gObjParentGraphNode;

// rendering_graph_node.c
struct GraphNodeCamera *gCurGraphNodeCamera = NULL;
struct GraphNodeObject *gCurGraphNodeObject = NULL;
struct GraphNodeHeldObject *gCurGraphNodeHeldObject = NULL;

// camera.c
struct DemoInput *gCurrDemoInput = NULL;

// envfx_bubbles.c
s16 gEnvFxBubbleConfig[10];

// src/audio/data.c
u32 gAudioRandom;

// src/audio/external.c
f32 gGlobalSoundSource[3];

// level_update.c
struct HudDisplay gHudDisplay;
struct MarioState gMarioStates[1];
struct MarioState *gMarioState = &gMarioStates[0];

// main.c
s8 gDebugLevelSelect = FALSE;

// behavior_script.c
u16 gRandomSeed16;