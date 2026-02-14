#include "game/world.h"
#include "nu2api.saga/nuandroid/ios_graphics.h"

#include <string.h>

WORLDINFO WorldInfo;
WORLDINFO *WORLD = &WorldInfo;

/// @brief Pointer to the currently loading world info
static WORLDINFO *LWORLD = &WorldInfo;
extern bool g_BackgroundUsedFogColour;
void WorldInfo_Activate(void) {
    WORLD = LWORLD;
    WorldInfo_Init(LWORLD);
    int result = 0; 
    if (NuIOS_IsLowEndDevice()) {
        if (WORLD) {
            char* inner_ptr = (char*)(WORLD->current_level);
            if (inner_ptr) {
                if (*(float*)(inner_ptr + 0x8C) < 20000.0f) {
                    result = (*(float*)(inner_ptr + 0x90) < 20000.0f);
                }
            }
        }
    }
    g_BackgroundUsedFogColour = result;
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