#include "game/world.h"

#include <string.h>

WORLDINFO WorldInfo;
WORLDINFO *WORLD = &WorldInfo;

/// @brief Pointer to the currently loading world info
static WORLDINFO *LWORLD = &WorldInfo;

void WorldInfo_Activate(void) {
    WORLD = LWORLD;
    WorldInfo_Init(LWORLD);
}

void WorldInfo_Init(WORLDINFO *info) {
}

WORLDINFO *WorldInfo_CurrentlyActive(void) {
    return WORLD;
}

WORLDINFO *WorldInfo_CurrentlyLoading(void) {
    return LWORLD;
}

void WorldInfo_InitOnce(void) {
    memset(&WorldInfo, 0, sizeof(WorldInfo));
}