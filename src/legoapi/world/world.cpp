#include "legoapi/world/world.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world_shared.h"

#include <stdio.h>
#include <string.h>

#include "gamelib/util/gamelib_util_types.h"
#include "gameapi/edtools/edstubs.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/world/area.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/world/level.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/core/input/timer.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"

// Globals shared across world loading — defined here until moved to globals.cpp
TIMER LevelTimer;
i32 abort_load = 0;
char ConfigBuffer[0x10000];
i32 numtl_force_mipmode = 0;
i32 GAMEDEMO = 0;
NUGSCN *big_icon_scene = NULL;
NUGSCN *area_scene = NULL;
NUGSCN *things_scene = NULL;
LEVELDATA *PLATFORM_LDATA = NULL;
LEVELDATA *RETAKED_LDATA = NULL;
LEVELDATA *CREDITS_LDATA = NULL;
u32 Text_Language = 1;
nufpcomjmp_s LevelConfigKeywords_AfterLoad;

// --- Players globals ---
// NOTE: `WORLD` (WORLDINFO) field offsets in world.h are an approximation; the
// original binary reaches the level/area pointers through WORLDINFO[0x128]
// while world.h places them at 0x130/0x134.  Players_Init still links and runs
// with the world.h layout but its level/area guard bytes will differ until that
// WORLDINFO drift is corrected.

GameObject_s *Player[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
i32 PLAYERCOUNT = 0;
i32 netclient = 0;
i32 UsePlayerList = 0;
i16 PlayerList[8];
i32 PlayerID[2] = {-1, -1};
i16 Area_PlayerIDList[9];
APICHARACTERMODELLIST_s Area_StoryModelList[52];
PLAYERPROGRESS PlayerProgress[8];
i32 Hub_UsePlayerList = 0;
i32 LevelChangesInArea = 0;
i32 bonusmodearcade = 0;
i32 VehicleArea = 0;
SOCKPOSITION OldPlrSPos[8];
char Batarang[8 * 0xb4];
void *PlayerSuit[8];
u8 PlayerTorpedoCount[8];
COINPACKET CoinPacket[2];
u32 BackUpPlayers[0x872];
char GizForceLOSInfo[0xc60];
i32 DEFAULT_PLAYERHITPOINTS = 8;
u32 LEGOOBJ_DEFAULTLASTCOIN = -1;

PLAYERDATA *apicharsys;

// --- World-module helpers (kept with the WorldInfo API) ---

void SetAreaPickupGravity(i32 area, i32 level) {
    AreaPickupGravity = -6.0f;
    if (area < 0 || area >= AREACOUNT) {
        return;
    }
    AREADATA *ad = &ADataList[area];
    if (ad->flags & AREAFLAG_NOPICKUPGRAVITY) {
        if ((*(u8 *)((char *)LDataList + level * 0x144 + 0x66) & 0x40) == 0) {
            AreaPickupGravity = 0.0f;
            return;
        }
    }
    if (ad->flags & AREAFLAG_VEHICLE_AREA) {
        AreaPickupGravity = -20.0f;
    }
}
void WorldInfo_Dump(WORLDINFO *world) {
    (void)world;
}
void StoreSceneProgress(NUGSCN *gscn, SCENEPROGRESS_s *progress, i32 param) {
    (void)gscn;
    (void)progress;
    (void)param;
}
void SaveSceneObjectAnimTFactors(NUGSCN *gscn) {
    i32 count;
    char *p;
    void *obj;

    if (gscn == NULL) {
        return;
    }
    if (*(void **)((char *)gscn + 0x110) != NULL) {
        return;
    }
    count = *(i32 *)((char *)gscn + 0x1c);
    p = *(char **)((char *)gscn + 0x20) + count * 0x50;
    if (count == 0) {
        return;
    }
    do {
        p -= 0x50;
        obj = *(void **)(p + 0x48);
        if (obj != NULL) {
            *(f32 *)((char *)obj + 0x54) = *(f32 *)((char *)obj + 0x40);
        }
        count--;
    } while (count != 0);
}
void CalculateWorldSize(WORLDINFO *world) {
    (void)world;
}

WORLDINFO WorldInfo[2];
WORLDINFO *WORLD = &WorldInfo[0];

f32 g_BgLoadDelayHackTimer;

static i32 EditBufferEndSize = 0;

i32 waiting_for_level;
i32 level_already_loaded = -1;
i32 next_level;

/// @brief Pointer to the currently loading world info
static WORLDINFO *LWORLD = &WorldInfo[0];

void WorldInfo_InitOnce(void) {
    memset(WorldInfo, 0, sizeof(WorldInfo));

#ifndef HOST_BUILD
    static_assert(sizeof(void *) != 4 || sizeof(WorldInfo) == 0xa360, "WorldInfo size mismatch");
#endif
}

void WorldInfo_Init(WORLDINFO *world) {
    i32 local_menu_id = -1;
    i32 local_menu_y = -1;

    disable_narrow_socks = 0;
    script_spline_selected = 0;

    // Reflections_On is set based on device specs in original
    // For now, keep existing logic
    g_lowEndLevelBehaviour = g_isLowEndDevice;

    if (g_isLowEndDevice) {
        if (world->current_level != NULL) {
            DebrisSetThinningLevel(world->current_level->data_display.particle_thin);
            DebrisSetForcedThinning(world->current_level->data_display.particle_thin > 0.0f);
            character_farclip = world->current_level->unknown_11c;
            SetCameraZoom(world->current_level->unknown_120);
        }
    } else if (world->current_level == NULL) {
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

    // Sound and SFX setup
    ResetSounds();
    SetLevelSfxBits(world);

    // Music setup
    LevMusicAction = ActionFromQuiet((i32)world->current_level->music_index);
    LevMusicAmbient = AmbientFromQuiet((i32)world->current_level->music_index);
    LevMusicOtherAction = ActionFromQuiet((i32)(i16)world->current_level->unknown_0a8);
    LevMusicOtherAmbient = AmbientFromQuiet((i32)(i16)world->current_level->unknown_0a8);

    ResetBits |= 1;

    if (NOSOUND == 0) {
        WorldInfo_Register(world);
    }

    // Start page loading for various subsystems
    i32 *page_handles = (i32 *)&world->page_pp;
    if (page_handles[0] != -1) {
        edppStartPage(page_handles[0]);
    }
    if (page_handles[1] != -1) {
        edpartStartPage(page_handles[1]);
    }
    if (page_handles[5] != -1) {
        edbriStartPage(page_handles[5]);
    }

    // Terrain initialization
    noterraininit();
    void *terrain_cur = world->terrain;
    TerrainSetCur(terrain_cur);
    TerrSetPlatScanDist((f32)(u8)world->current_level->unknown_0db);

    if (page_handles[4] != -1) {
        edgraStartPage(page_handles[4]);
    }

    TerrainPlatformOldUpdate();
    TerrainPlatformNewUpdate();
    TerrainSetWallDeflectYScale(1.0f);
    LevObj_FixUpPlatIDs(world);

    if (NOSOUND == 0) {
        NuRndrInitWorld();
    }

    TerrainPlatformOldUpdate();
    if (world->current_gscn != NULL) {
        NuGScnUpdate(world->current_gscn, 0);
    }
    TerrainPlatformNewUpdate();

    // Level progress copy
    LEVEL_PROGRESS_s *progress = (LEVEL_PROGRESS_s *)world->level_progress;
    if (NOSOUND == 0 && progress != NULL && (progress->flags & 1) == 0) {
        // Progress payload lives at the tail of the name/config buffer region;
        // there is no named field for it, so the source is addressed raw.
        i32 *src = (i32 *)&world->name[0x5c];
        i32 *dst = (i32 *)&progress->data;
        for (i32 i = 0xa00; i != 0; i--) {
            *dst = *src;
            src++;
            dst++;
        }
        progress->flags |= 1;
    }

    Doors_Init(world);
    Players_InitPositions(world);
    ClearGameObjects((APIOBJECTSYS_s *)world->ai_sys);
    PlayerItemTypes_Reset(world);
    Players_Init();
    rtlResetDynamic();
    SetPartRTLSet(world->rtl_set_id);

    WorldInfo_UpdateRoomVisibility(world, 1);

    // Level init function
    if (world->current_level->init_fn != NULL) {
        ((void (*)(WORLDINFO *))world->current_level->init_fn)(world);
    }

    // The game hook may provide a menu and Y position for the newly loaded
    // level.  The hook is not linked in this target, but the state transition
    // is part of WorldInfo_Init itself.
    if (newlevelfrommenu_newmenuid != -1) {
        local_menu_id = newlevelfrommenu_newmenuid;
        local_menu_y = newlevelfrommenu_newmenuy;
        newlevelfrommenu_newmenuid = -1;
        newlevelfrommenu_newmenuy = -1;
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

    MenuReset();
    NewMenu(local_menu_id, local_menu_y, -1);

    if (NOSOUND == 0) {
        ResetTimer(&LevelTimer, 0.0f);
    }

    if ((world->current_level->flags & (LEVEL_OUTRO | LEVEL_MIDTRO | LEVEL_INTRO)) == 0) {
        WorldInfo_LoadObjectAnimFile(world);
    }

    reset_restart = 1;
    *(i32 *)&world->filler1[0] = 1; // field_0x114

    InitGameObjectLights();

    // Unreferenced padding slot (0x5174); no named field.
    *(i32 *)((char *)world + 0x5174) = 1;

    // Init last function
    Game_WorldInfo_InitLast(world);
}

void WorldInfo_Load(WORLDINFO *world) {
    char buf[268];
    char titles[64];
    LEVELDATA *level;
    NUGSCN *cutscene_scene;
    i32 *page_handles;
    i32 aligned_buf;
    char ai_name[4];
    i32 ai_buf_size;
    i32 rtl_id;
    char *light_path;

    level = world->current_level;
    ai_name[0] = 'a';
    ai_name[1] = 'i';
    ai_name[2] = 0;
    ai_name[3] = 0;

    Level_LoadConfigFile(world);

    if (abort_load != 0 || (world->config_count > 0 && (LevelConfig_BeforeLoad(world->current_level, ConfigBuffer,
                                                                               Level_ConfigBeforeLoad_GameKeywords),
                                                        abort_load != 0))) {
        goto abort;
    }

    if ((level->flags & LEVEL_STATUS) != 0) {
        // Align giz_buffer
        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);

        if (level == TITLES_LDATA) {
            NuStrCpy(buf, "levels\\titles\\");
            if (Text_Language < 9) {
                // Language-specific titles loading (switch table)
                goto abort;
            }
            if (Text_Language == 0x12) {
                NuStrCpy(titles, "titles_us");
            } else {
                NuStrCpy(titles, "titles_uk");
            }
            NuStrCat(buf, titles);
        } else if (level == (LEVELDATA *)PLATFORM_LDATA) {
            NuStrCpy(buf, "levels\\episode_v\\cloudcityescape\\cloudcityescape_b\\cloudcityescape_b");
        } else {
            NuStrCpy(buf, world->config_file);
        }
        NuStrCat(buf, ".gsc");

        numtl_force_mipmode = (i32)(u8)level->mipmap_mode + 1;
        world->current_gscn = NuGScnRead(&world->giz_buffer, world->unknown_0108, buf);
        numtl_force_mipmode = 0;

        StoreSceneProgress(world->current_gscn, (SCENEPROGRESS_s *)world->progress_data, 1);
        SaveSceneObjectAnimTFactors(world->current_gscn);

        if (world->current_gscn != NULL) {
            CalculateWorldSize(world);
        }
        if (abort_load != 0)
            goto abort;
    }

    // Load pictures for titles/credits
    if (level == TITLES_LDATA || level == CREDITS_LDATA) {
        numtl_force_mipmode = (i32)(u8)level->mipmap_mode + 1;
        world->icons_gscn = NuGScnRead(&world->giz_buffer, world->unknown_0108, "levels\\titles\\pictures.gsc");
        if (abort_load != 0)
            goto abort;
    }

    // Create game animation system
    world->game_anim_sys = (GAMEANIMSYS_s *)GameAnimSys_Create(&world->giz_buffer, &world->unknown_0108);

    // Load particles
    Particles_Load(world, &debris_name, 400, 0x93);
    if (abort_load != 0)
        goto abort;
    LoadPartFile(world);
    if (abort_load != 0)
        goto abort;

    // Configure bolt types
    if (world->config_count > 0) {
        BoltTypes_Configure(world, ConfigBuffer);
    }

    // Create antinode system
    world->game_antinode_sys = (GAMEANTINODESYS_s *)GameAntnode_CreateSys(
        world, &world->giz_buffer, &world->unknown_0108, (i32)(u16)world->current_level->max_antinodes);

    // Create gizmo system
    world->gizmo_sys = (GIZMOSYS_s *)CreateGizmoSys(world, &world->giz_buffer, &world->unknown_0108);

    // Load editor splines
    sprintf(buf, "%s%s", world->config_file, ".spl");
    LoadEditorSplines(buf, &world->giz_buffer, &world->unknown_0108);

    GizmoBlowupResetNameTable();
    LoadGizmoSys((GIZMOSYS_s *)world->gizmo_sys, world, world->config_file);
    if (abort_load != 0)
        goto abort;

    // Area-specific loading
    if (world->area == NULL) {
        goto after_area;
    }
    if (world->area == HUB_ADATA && GAMEDEMO == 0) {
        Hub_LoadAndFixUpMiniKits(world, &world->giz_buffer, &world->unknown_0108);
        if (abort_load != 0)
            goto abort;
        goto after_area;
    }
    if ((level->flags & LEVEL_STATUS) != 0 && world->area->minikit_id != -1) {
        MiniKit_Load(&world->minikit, (i32)(i16)world->area->minikit_id, &world->giz_buffer, &world->unknown_0108,
                     NULL);
        if (world->minikit.gscn != NULL) {
            MiniKit_InitPieces(&world->minikit, 10, &world->giz_buffer, &world->unknown_0108);
        }
        if (abort_load != 0)
            goto abort;
    }

after_area:
    // Load big icon scene for hub/status levels
    if ((level == HUB_LDATA || (level->flags & LEVEL_STATUS) != 0) && big_icon_scene == NULL) {
        world->icons_gscn = NuGScnRead(&world->giz_buffer, world->unknown_0108, "stuff\\icons\\starwars_icons_all.gsc");
    }

    // Determine which scene to use for cutscenes
    cutscene_scene = area_scene;
    if (cutscene_scene == NULL || world->area == NULL || (world->area->flags & AREAFLAG_OVERRIDE_THINGS_SCENE) == 0) {
        cutscene_scene = things_scene;
    }

    // Load cutscenes
    page_handles = (i32 *)&world->page_pp;
    world->cutscene_sys = (CUTSYS *)CutScenes_Load(
        ConfigBuffer, world->current_gscn, cutscene_scene, page_handles[0], &world->giz_buffer,
        // 0x25c/0x260 are i32 reads into the progress_data region.
        &world->unknown_0108, *(i32 *)((char *)world + 0x25c), *(i32 *)((char *)world + 0x260), world);
    if (abort_load != 0)
        goto abort;

    // Character scenes
    aligned_buf = ALIGN((i32)world->giz_buffer.addr, 4);
    *(i32 *)&world->minikit.field_0x18 = aligned_buf;
    world->giz_buffer.addr = (usize)(aligned_buf + CHARCOUNT * 0x10);
    CharScenes_LevelLoad(world);

    // SockSys for certain level types
    if ((level->flags & (LEVEL_GAMEPLAY | LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) == LEVEL_GAMEPLAY) {
        world->sock_sys = SockSysInit(&world->giz_buffer, world->unknown_0108, world->current_gscn);
    }

    LevelSplines_InitForLevel(world);
    LevelObjects_InitForLevel(world);
    BoltTypes_Init(world);

    // Config-based subsystem initialization
    if (world->config_count > 0) {
        LevelConfig_AfterLoad(world->current_level, ConfigBuffer, &LevelConfigKeywords_AfterLoad);
        EquivalentObjects_Configure(world, ConfigBuffer);
        Teleports_Configure(world, ConfigBuffer);
        Doors_Configure(world, ConfigBuffer);
        Faders_Configure(world, ConfigBuffer);
        CharPlatforms_Configure(world, ConfigBuffer);
        Grabber_Configure(world, ConfigBuffer);
        Pulses_Configure(world, ConfigBuffer);
        TrafficAnimSys_Configure(world, ConfigBuffer);
        SpecialMiniKits_Configure(world, ConfigBuffer);
        GizForceSFX_Configure(world, ConfigBuffer);
    }

    // SockSys configuration
    if (world->sock_sys != NULL) {
        if (world->config_count > 0) {
            SockSys_Configure(world->sock_sys, ConfigBuffer, 0, &world->giz_buffer, &world->unknown_0108,
                              world->current_gscn);
        }
        SockSys_GenerateData(world->sock_sys, &world->giz_buffer, &world->unknown_0108);
        if (abort_load != 0)
            goto abort;
    }

    // Terrain/grass/bridge loading
    if (world->area != NULL &&
        (world->area->flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) ==
            (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA) &&
        (level->flags & (LEVEL_STATUS | LEVEL_OUTRO | LEVEL_MIDTRO | LEVEL_INTRO)) == 0) {
        CharacterMiniKits_Load((COLLECTION_s *)MiniKitCollection, world, &world->giz_buffer, &world->unknown_0108);
        if (abort_load != 0)
            goto abort;
    } else {
        LoadTerrainFile(world);
        if (abort_load != 0)
            goto abort;
        LoadGrassFile(world);
        if (abort_load != 0)
            goto abort;
        LoadBridgeFile(world);
        if (abort_load != 0)
            goto abort;
    }

    // AI system loading
    if ((level->flags & (LEVEL_GAMEPLAY | LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) == LEVEL_GAMEPLAY &&
        level != (LEVELDATA *)PLATFORM_LDATA) {
        world->ai_loaded = 0;
        ai_buf_size = 0x1cc00;
        if (RETAKED_LDATA != NULL && level == (LEVELDATA *)RETAKED_LDATA) {
            ai_buf_size = 0x1e800;
        }
        world->ai_sys = (AISYS *)AISysLoad(&world->giz_buffer, &world->unknown_0108, ai_buf_size, world->current_gscn,
                                           level->dir, level->name, ai_name);
        world->ai_path_cnx_control_sys =
            (AIPATHCNXCONTROLSYS_s *)AIPathCnxControlSysCreate(&world->giz_buffer, &world->unknown_0108, 0x40);
        world->ai_path_cnx_helper_sys =
            (AIPATHCNXHELPERSYS_s *)AIPathCnxHelperSysCreate(&world->giz_buffer, &world->unknown_0108, 0x20);
        world->ai_trigger_set_sys =
            (AITRIGGERSETSYS_s *)AITriggerSetSysCreate(&world->giz_buffer, &world->unknown_0108);

        if (world->ai_sys != NULL) {
            world->mech_auto_jump_manager = (MechAutoJumpManager *)1; // placeholder
        }

        GameAIScriptAddLevelSfx(world, &global_aiscripts);
        GameAIScriptAddLevelSfx(world, &world->ai_sys->scripts);

        world->climb_object_sys = (CLIMBOBJECTSYS_s *)CreateClimbObjectSys(&world->giz_buffer, &world->unknown_0108,
                                                                           (i32)(u8)level->max_climb_objs);
    } else {
        world->ai_loaded = 1;
    }

    if (abort_load != 0)
        goto abort;

    // API object system
    world->api_object_sys = (APIOBJECTSYS_s *)APIObjectSysInit(0x10e4, &world->giz_buffer, &world->unknown_0108);
    if (abort_load != 0)
        goto abort;

    // Lights
    if ((level->flags & LEVEL_STATUS) == 0) {
        world->rtl_id = -1;
        world->light_dir = 0;
    } else {
        light_path = world->config_file;
        LoadLights(world, light_path);
        rtl_id = rtlFindByUserId(world->rtl_set_id, 1);
        world->rtl_id = rtl_id;
        if (rtl_id != -1) {
            rtlGetDirection(world->rtl_set_id, rtl_id, (void **)&world->light_dir);
        } else {
            world->light_dir = 0;
        }
    }

    if (abort_load != 0)
        goto abort;

    // More config-based subsystems
    if (world->config_count > 0) {
        RippleEffects_Configure(world, ConfigBuffer);
        PortalDoors_Configure(world, ConfigBuffer);
        if (abort_load != 0)
            goto abort;
    }

    GizmoSysAddGizmos((GIZMOSYS_s *)world->gizmo_sys, (GIZFLOW_s *)world->giz_flow, world);
    if (abort_load != 0)
        goto abort;

    // Load gizmo flow
    NuStrCpy(buf, world->config_file);
    NuStrCat(buf, ".git");
    world->giz_flow =
        (GIZFLOW_s *)LoadGizFlow(world, (GIZMOSYS_s *)world->gizmo_sys, buf, &world->giz_buffer, &world->unknown_0108);
    if (abort_load != 0)
        goto abort;

    InitSpecialSfx(world);
    LoadSpecialSfxFile(world);
    if (abort_load != 0)
        goto abort;

    // Level load function
    if (level->load_fn != NULL) {
        ((void (*)(WORLDINFO *, void *, void *))level->load_fn)(world, &world->giz_buffer, &world->unknown_0108);
    }

    // i32 reads into the progress_data region (0x260/0x25c).
    SetAreaPickupGravity(*(i32 *)((char *)world + 0x260), *(i32 *)((char *)world + 0x25c));
    world->loaded = 1;
    return;

abort:
    WorldInfo_Dump(world);
    WorldInfo_Reset(world, -1);
}

i32 WorldInfo_Reset(WORLDINFO *world, i32 level_idx) {
    if (world->loaded != 0) {
        SetLevelExBlowupFlags(0);
        if (world->level_idx == level_idx && (u8)LDataList[level_idx].area_index != 0xff && new_level_from_menu == 0) {
            return 0;
        }
        WorldInfo_Dump(world);
    } else {
        SetLevelExBlowupFlags(0);
    }

    if (new_level_from_menu != 0) {
        WORLDINFO *other = world + 1;
        if (WORLD != world) {
            other = WORLD;
        }
        if (other->loaded != 0 && other->level_idx == level_idx) {
            WorldInfo_Dump(other);
        }
    }

    // Save buffer cursors before clearing the world.
    void *bufStart = world->buffer_start;
    void *bufEnd = world->unknown_0108.void_ptr;

    TouchHacks::CleanupAllMechObjectInterfaces(world);

    memset(world, 0, 0x51b0);

    // Restore the buffer cursors: 0x108, then 0x100, then 0x104 (giz_buffer).
    world->unknown_0108.void_ptr = bufEnd;
    world->buffer_start = bufStart;
    world->giz_buffer.void_ptr = bufStart;
    if ((char *)bufEnd > (char *)bufStart) {
        memset(bufStart, 0, (usize)((char *)bufEnd - (char *)bufStart));
    }

    // Set level info
    world->level_idx = level_idx;
    world->level_sub_id = -1;
    world->area_sub_id = -1;

    if (level_idx != -1) {
        LEVELDATA *levelData = &LDataList[level_idx];
        world->current_level = levelData;
        i32 areaIdx = (i8)levelData->area_index;
        world->level_sub_id = areaIdx;
        if (areaIdx != -1) {
            world->area = &ADataList[areaIdx];
            world->area_sub_id = (i8)ADataList[areaIdx].episode_index;
        }

        // Invalidate every page handle (0x2a98 .. 0x2aac).
        world->page_pp = -1;
        world->page_part = -1;
        world->page_anim = -1;
        world->page_grass = -1;
        world->page_bridge = -1;

        i32 progress_index = (i8)levelData->area_level_index;
        if (progress_index > 0xb || progress_index == -1) {
            world->level_progress = NULL;
        } else {
            world->level_progress = (LEVEL_PROGRESS_s *)((char *)LevelProgressData + progress_index * 0x2e24);
        }

        // Build config file path
        NuStrCpy(world->name, "levels\\");
        NuStrCat(world->name, levelData->dir);
        NuStrCpy(world->config_file, world->name);
        NuStrCat(world->config_file, "\\");
        NuStrCat(world->config_file, levelData->name);
        ResetLevSfx(world);
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

    if (LWORLD->loaded != 0) {
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

    if (other->loaded != 0) {
        return other->level_idx;
    }

    return -1;
}

void WorldInfo_Register(WORLDINFO *world) {
    edbitsRegisterBaseScene(world->current_gscn);
    edanimRegisterBaseScene(world->current_gscn);
    edbitsRegisterBaseTerrain(world->terrain);
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
    if (world->page_anim == -1) {
        char path[256];
        sprintf(path, "%s.anm", world->config_file);
        if (NuFileExists(path)) {
            world->page_anim = edanimLoadPage(path, world->current_gscn);
        }
    }
}

void WorldInfo_DrawScene(WORLDINFO *world) {
    if (world->current_gscn == NULL) {
        return;
    }
    if (CUTCAM == 0 && world->current_gscn->display_list == NULL) {
        NuPortalInit();
        NuPortalVisibility(world->current_gscn);
        WorldInfo_UpdateRoomVisibility(world, 0);
    }
    NuGScnRndr3(world->current_gscn);
    if (world->current_gscn->display_list != NULL) {
        WorldInfo_UpdateRoomVisibility(world, 0);
    }
}

void WorldInfo_UpdateRoomVisibility(WORLDINFO *world, i32 param) {
    // NOTE: using the named `rooms_visible` field (vs a raw byte offset) shifts
    // GCC 4.7's register allocation for the 0x100-byte memset alignment path,
    // dropping the fuzzy match (~70% -> ~65%) with identical instructions.
    u8 *visBuf = world->rooms_visible;
    world->room_visibility_flag = 1;
    world->rooms_visible_ptr = visBuf;
    memset(visBuf, 0, 0x100);

    if (param == 0 && world->current_gscn != NULL && world->current_gscn->field5_0x8 > 0) {
        u8 *portalData = (u8 *)world->current_gscn->portals;
        u8 *end = visBuf + world->current_gscn->field5_0x8;
        while (visBuf != end) {
            *visBuf++ = (u8)((portalData[0x10] >> 2) & 1);
            portalData += 0x18;
        }
    }
}

void WorldInfo_ReArrangeBuffers(i32 area1, i32 area2) {
    VARIPTR *bufferEnd = (VARIPTR *)&WorldInfo[1].unknown_0108;

    if (area1 == area2 || area1 == -1) {
        if (area1 != -1 && (ADataList[area1].flags & AREAFLAG_SINGLE_BUFFER) != 0) {
            return;
        }
    } else if ((ADataList[area1].flags & AREAFLAG_SINGLE_BUFFER) != 0) {
        LWORLD = &WorldInfo[0];
        WORLD = &WorldInfo[0];
        if (WorldInfo[0].unknown_0108.addr < bufferEnd->addr) {
            return;
        }
        bufferEnd->addr = WorldInfo[0].unknown_0108.addr;
        WorldInfo[0].unknown_0108.addr = bufferEnd->addr - EditBufferEndSize;
        return;
    }

    if (WorldInfo[0].unknown_0108.addr < bufferEnd->addr) {
        return;
    }
    usize end = WorldInfo[0].unknown_0108.addr;
    WorldInfo[0].unknown_0108.addr = bufferEnd->addr;
    bufferEnd->addr = end + EditBufferEndSize;
}

extern "C" {
    i32 Collection_Got(i32);
    i32 InModelList(APICHARACTERMODELLIST_s *, i32, i32 *);
    extern i16 id_DARTHVADER;
    extern i16 id_THEEMPEROR;
    extern i16 id_GRANDMOFFTARKIN;
    extern i16 id_IMPERIALOFFICER;
    extern i16 id_IMPERIALSHUTTLEPILOT;
}

i32 InModelListDataFlags(APICHARACTERMODELLIST_s *, u32, u32, i32, i32);
i32 RandomIDFromFlags(u32, u32, i32, APICHARACTERMODELLIST_s *, i32);
void Collection_GetIDList(COLLECTION_s *, u32, u32, i16 *, i32 *, i32 *, i32);

void MakeFreePlayModelList(i32 model1, i32 model2, i32 area, i32 level, i32 param5) {
    i32 flags = 0;
    if (WORLD != NULL && WORLD->area != NULL && WORLD->area == HUB_ADATA && bonusmodearcade != 0)
        flags = Arcade_Mode[ArcadeItem.field_c_0xc * 3].field8_0x8;

    FreePlayModelCount = 0;
    FreePlayResidentCount = 0;
    FreePlayBonusCount = 0;
    if (model1 == -1)
        model2 = -1;

    AREADATA *ad = &ADataList[area];

    if ((ad->flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) == (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) {
        i32 count = 0;
        i32 models[2] = {model1, model2};
        for (i32 i = 0; i < 2; i++) {
            i32 m = models[i];
            if (m == -1)
                break;
            if (count > 0x2f)
                continue;
            if (count > 1) {
                if (FreePlayModelList[0].model_id == m)
                    continue;
                i32 j;
                for (j = 1; j < count; j++) {
                    if (FreePlayModelList[j].model_id == m)
                        break;
                }
                if (j != count)
                    continue;
            }
            FreePlayModelList[count].model_id = m;
            FreePlayModelList[count].count = 1;
            count++;
            FreePlayModelList[count].model_id = -1;
        }
        FreePlayModelCount = count;
    } else {
        i32 count = 0;
        i32 models[2] = {model1, model2};
        for (i32 i = 0; i < 2; i++) {
            i32 m = models[i];
            if (m == -1)
                break;
            if (count > 0x2f)
                continue;
            if (count > 1) {
                if (FreePlayModelList[0].model_id == m)
                    continue;
                i32 j;
                for (j = 1; j < count; j++) {
                    if (FreePlayModelList[j].model_id == m)
                        break;
                }
                if (j != count)
                    continue;
            }
            FreePlayModelList[count].model_id = m;
            FreePlayModelList[count].count = 1;
            count++;
            FreePlayModelList[count].model_id = -1;
        }
        FreePlayModelCount = count;
    }
}
