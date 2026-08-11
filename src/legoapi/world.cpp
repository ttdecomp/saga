#include "legoapi/world.h"

#include <string.h>

#include "globals.h"
#include "legoapi/area.h"
#include "legoapi/level.h"
#include "legogame/game.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nutime.h"

void DebrisSetThinningLevel(f32 level) {}
void DebrisSetForcedThinning(i32 forced) {}
void DebrisSetDetailLevel(i32 level) {}
void SetCameraZoom(f32 zoom) {}
i32 ActionFromQuiet(i32 idx) { return -1; }
i32 AmbientFromQuiet(i32 idx) { return -1; }

WORLDINFO WorldInfo[2];
WORLDINFO *WORLD = &WorldInfo[0];

f32 g_BgLoadDelayHackTimer;

i32 waiting_for_level;
i32 level_already_loaded = -1;
i32 next_level;

/// @brief Pointer to the currently loading world info
static WORLDINFO *LWORLD = &WorldInfo[0];

void WorldInfo_InitOnce(void) {
    memset(WorldInfo, 0, sizeof(WorldInfo));
}

void WorldInfo_Init(WORLDINFO *world) {
    i32 local_menu_id = -1;
    i32 local_menu_y = -1;

    disable_narrow_socks = 0;
    script_spline_selected = 0;

    if (g_isLowEndDevice) {
        if (world->current_level != NULL) {
            DebrisSetThinningLevel(world->current_level->data_display.particle_thin);
            DebrisSetForcedThinning(world->current_level->data_display.particle_thin > 0.0f);
            character_farclip = world->current_level->unknown_11c;
            SetCameraZoom(world->current_level->unknown_120);
        }
    } else if (world->current_level == NULL) {
        // goto low_end_path: default debris settings
        DebrisSetDetailLevel(4);
        DebrisSetThinningLevel(1.0f);
        DebrisSetForcedThinning(0);
    } else {
        char *nameCheck = NuStrIStr(world->current_level->name, "JabbasPalace_Intro1");
        if (nameCheck == NULL) {
            if (g_isLowestEndDevice && world->current_level != NULL) {
                nameCheck = NuStrIStr(world->current_level->name, "Dogfight_A");
                if (nameCheck != NULL || NuStrIStr(world->current_level->name, "DeathStarRescue") != NULL ||
                    NuStrIStr(world->current_level->name, "GunGan_A") != NULL ||
                    NuStrIStr(world->current_level->name, "SpeederChase") != NULL) {
                    DebrisSetDetailLevel(1);
                    g_lowEndLevelBehaviour = 1;
                    DebrisSetThinningLevel(6.0f);
                    DebrisSetForcedThinning(1);
                    character_farclip = world->current_level->unknown_11c;
                    SetCameraZoom(world->current_level->unknown_120);
                } else {
                    DebrisSetDetailLevel(4);
                    DebrisSetThinningLevel(1.0f);
                    DebrisSetForcedThinning(0);
                }
            } else {
                DebrisSetDetailLevel(4);
                DebrisSetThinningLevel(1.0f);
                DebrisSetForcedThinning(0);
            }
        } else {
            DebrisSetThinningLevel(6.0f);
            DebrisSetForcedThinning(1);
        }
    }

    if (g_isLowestEndDevice && world->current_level != NULL &&
        NuStrIStr(world->current_level->name, "Negotiations_") != NULL) {
        Reflections_On = 0;
    }

    // Music setup
    LevMusicAction = ActionFromQuiet((i32)world->current_level->music_index);
    LevMusicAmbient = AmbientFromQuiet((i32)world->current_level->music_index);
    LevMusicOtherAction = ActionFromQuiet((i32)(i16)world->current_level->unknown_0a8);
    LevMusicOtherAmbient = AmbientFromQuiet((i32)(i16)world->current_level->unknown_0a8);

    ResetBits |= 1;

    if (NOSOUND == 0) {
        WorldInfo_Register(world);
    }

    // Level init function
    if (world->current_level->init_fn != NULL) {
        ((void (*)(WORLDINFO *))world->current_level->init_fn)(world);
    }

    // Menu setup
    if (local_menu_id != -1) {
        local_menu_id = -1;
    }

    // FreePlay check
    if (world->area != NULL) {
        if ((world->area->flags & AREAFLAG_BONUS_AREA) == 0) {
            if (world->area == HUB_ADATA) {
                NextArea_FreePlay = 0;
                FreePlay = 0;
            }
        } else {
            NextArea_FreePlay = 1;
            FreePlay = 1;
        }
    }

    reset_restart = 1;

    // Init last function
    Game_WorldInfo_InitLast(world);
}

void WorldInfo_Load(WORLDINFO *world) {
    Level_LoadConfigFile(world);

    if (world->unknown_010c > 0) {
        LevelConfig_BeforeLoad(world->current_level, NULL, Level_ConfigBeforeLoad_GameKeywords);
    }

    if (world->current_level != NULL && (world->current_level->flags & LEVEL_STATUS) != 0) {
        // Status level: load titles
        if (world->current_level == TITLES_LDATA) {
            // Load titles-specific stuff
        }
    }

    // Initialize various subsystems
    WorldInfo_UpdateRoomVisibility(world, 1);

    if (world->current_level->init_fn != NULL) {
        ((void (*)(WORLDINFO *))world->current_level->init_fn)(world);
    }

    if (NOSOUND == 0) {
        // Timer reset
    }

    if ((world->current_level->flags & (LEVEL_OUTRO | LEVEL_MIDTRO | LEVEL_INTRO)) == 0) {
        WorldInfo_LoadObjectAnimFile(world);
    }

    reset_restart = 1;
}

i32 WorldInfo_Reset(WORLDINFO *world, i32 level_idx) {
    if (world->unknown_0110 == 0) {
        SetLevelExBlowupFlags(0);
    } else {
        SetLevelExBlowupFlags(0);
        if (world->unknown_011c == level_idx && LDataList[level_idx].unknown_0af != -1) {
            return 0;
        }
    }

    // Save buffer pointers
    void *bufStart = *(void **)&world->filler0[0xFC];
    void *bufEnd = world->unknown_0108.void_ptr;

    // Clear world
    memset(world, 0, sizeof(WORLDINFO));

    // Restore buffer pointers
    world->unknown_0108.void_ptr = bufEnd;
    *(void **)&world->filler0[0xFC] = bufStart;
    world->giz_buffer.void_ptr = bufStart;

    // Set level info
    world->unknown_011c = level_idx;
    world->unknown_0120 = -1;
    world->unknown_0124 = -1;

    if (level_idx != -1) {
        LEVELDATA *levelData = &LDataList[level_idx];
        world->current_level = levelData;
        i32 areaIdx = (i8)levelData->unknown_0af;
        world->unknown_0120 = areaIdx;
        if (areaIdx != -1) {
            world->area = &ADataList[areaIdx];
            world->unknown_0124 = (i8)ADataList[areaIdx].episode_index;
        }

        // Build config file path
        NuStrCpy(world->filler0, "levels\\");
        NuStrCat(world->filler0, levelData->dir);
        NuStrCat(world->filler0, "\\");
        NuStrCat(world->filler0, levelData->name);
        NuStrCpy(world->config_file, world->filler0);
    }

    return 1;
}

void WorldInfo_Activate(void) {
    char used_fog_color;

    WORLD = LWORLD;
    WorldInfo_Init(LWORLD);

    used_fog_color = 0;
    if (NuIOS_IsLowEndDevice() && WORLD != NULL) {
        LEVELDATA *current_level = WORLD->current_level;

        if (current_level && current_level->data_display.far_clip < 20000.0f) {
            used_fog_color = current_level->data_display.fog_start < 20000.0f;
        }
    }

    g_BackgroundUsedFogColour = used_fog_color;
}

void WorldInfo_StreamLevel(BGPROCINFO *bg_info) {
    WORLDINFO *world;
    NUTIME time;

    world = WorldInfo_CurrentlyActive();

    if (world == LWORLD && (Area == -1 || (ADataList[Area].flags & AREAFLAG_SINGLE_BUFFER) == 0)) {
        LWORLD = world == &WorldInfo[0] ? &WorldInfo[1] : &WorldInfo[0];
    }

    if (WorldInfo_Reset(LWORLD, next_level) != 0) {
        waiting_for_level = next_level;

        WorldInfo_Load(LWORLD);
    }

    waiting_for_level = -1;

    if (LWORLD->unknown_0110 != 0) {
        level_already_loaded = next_level;
    }

    NuTimeGet(&time);
    g_BgLoadDelayHackTimer = NuTimeSeconds(&time) + 5.0f;
}

WORLDINFO *WorldInfo_CurrentlyActive(void) {
    return WORLD;
}

WORLDINFO *WorldInfo_CurrentlyLoading(void) {
    return LWORLD;
}

i32 WorldInfo_OtherLevel(WORLDINFO *world) {
    WORLDINFO *other = &WorldInfo[0];

    if (world == &WorldInfo[0]) {
        other = &WorldInfo[1];
    }

    if (other->unknown_0110 != 0) {
        return other->unknown_011c;
    }

    return -1;
}

void WorldInfo_Register(WORLDINFO *world) {
    // edbitsRegisterBaseScene(world->current_gscn);
    // edanimRegisterBaseScene(world->current_gscn);
    // edbitsRegisterBaseTerrain(*(undefined4 *)&world->field_0x295c);
}

void WorldInfo_ClearAllIfScreenFaded(void) {
    if (LastLData == TITLES_LDATA || new_level_from_menu != 0 || Area != last_area || BGLOAD == 0) {
        WorldInfo_Reset(&WorldInfo[0], -1);
        if (last_area == -1 || (ADataList[last_area].flags & AREAFLAG_SINGLE_BUFFER) == 0) {
            WorldInfo_Reset(&WorldInfo[1], -1);
        }
        level_already_loaded = -1;
    }
}

void WorldInfo_LoadObjectAnimFile(WORLDINFO *world) {
    // if (world->field10825_0x2aa0 == -1) {
    //     char path[256];
    //     sprintf(path, "%s.anm", world->config_file);
    //     if (NuFileExists(path)) {
    //         world->field10825_0x2aa0 = edanimLoadPage(path, world->current_gscn);
    //     }
    // }
}

void WorldInfo_DrawScene(WORLDINFO *world) {
    // if (world->current_gscn != NULL) {
    //     if (CUTCAM == 0 && world->current_gscn->display_list == NULL) {
    //         NuPortalInit();
    //         NuPortalVisibility(world->current_gscn);
    //         WorldInfo_UpdateRoomVisibility(world, 0);
    //     }
    //     NuGScnRndr3(world->current_gscn);
    //     if (world->current_gscn->display_list != NULL) {
    //         WorldInfo_UpdateRoomVisibility(world, 0);
    //     }
    // }
}

void WorldInfo_UpdateRoomVisibility(WORLDINFO *world, i32 param) {
    u8 *visBuf = (u8 *)&world->unknown_0140[0x2851]; // field_0x2991
    world->unknown_0140[0x2850] = 1; // field_0x2990
    *(u8 **)&world->unknown_0140[0x2954] = visBuf; // field_0x2a94
    memset(visBuf, 0, 0x100);

    if (param == 0 && world->current_gscn != NULL) {
        // Portal visibility from current scene
        // i32 portalCount = *(i32 *)&(world->current_gscn->portal_info).field_0x8;
        // if (portalCount > 0) {
        //     u8 *flags = *(u8 **)&world->field_0x2a94;
        //     i32 portalData = *(i32 *)&(world->current_gscn->portal_info).field_0xc;
        //     for (i32 i = 0; i < portalCount; i++) {
        //         flags[i] = (*(u8 *)(portalData + 0x10) >> 2) & 1;
        //         portalData += 0x18;
        //     }
        // }
    }
}

void WorldInfo_ReArrangeBuffers(i32 area1, i32 area2) {
    // Buffer rearrangement for area streaming
}
