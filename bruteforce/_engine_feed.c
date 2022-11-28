#include <PR/ultratypes.h>

#include "sm64.h"
#include "src/engine/surface_load.h"

/**
 *variables defined in other files that may be edited from outside later 
 */
s16 gFindFloorIncludeSurfaceIntangible;
s16 gCheckingSurfaceCollisionsForCamera;
SpatialPartitionCell gStaticSurfacePartition[NUM_CELLS][NUM_CELLS];
SpatialPartitionCell gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS];
u32 gGlobalTimer;
s16 gCurrLevelNum;
struct Area *gCurrentArea = NULL;

u8 gSpecialTripleJump;

struct Controller gController;
struct Controller *gPlayer1Controller = &gController;