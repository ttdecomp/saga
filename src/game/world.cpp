#include "game/world.h"
#include "game/level.h"
#include "globals.h"
#include "nu2api.saga/nuandroid/ios_graphics.h"

#include <string.h>

WORLDINFO WorldInfo;
WORLDINFO *WORLD = &WorldInfo;

/// @brief Pointer to the currently loading world info
static WORLDINFO *LWORLD = &WorldInfo;

void WorldInfo_Activate(void) {
    WORLD = LWORLD;
    WorldInfo_Init(LWORLD);

    char result = 0;

    if (NuIOS_IsLowEndDevice()) {
        if (WORLD != NULL) {
            LEVELDATA *current_level = WORLD->current_level;
            if (current_level) {
                if (current_level->data_display.level_width < 20000.0f) {
                    result = current_level->data_display.level_depth < 20000.0f;
                }
            }
        }
    }

    g_BackgroundUsedFogColour = result;

    return;
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
