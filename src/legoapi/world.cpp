#include "legoapi/world.h"
#include "legoapi/world_shared.h"

#include <stdio.h>
#include <string.h>

#include "gameapi/edtools/edstubs.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/area.h"
#include "legoapi/character.h"
#include "legoapi/level.h"
#include "legoapi/players.h"
#include "legoapi/socksys.h"
#include "legoapi/timer.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"

// Globals shared across world loading — defined here until moved to globals.cpp
TIMER *LevelTimer = NULL;
i32 abort_load = 0;
char *ConfigBuffer = NULL;
i32 numtl_force_mipmode = 0;
i32 GAMEDEMO = 0;
void *big_icon_scene = NULL;
void *area_scene = NULL;
void *things_scene = NULL;
LEVELDATA *PLATFORM_LDATA = NULL;
void *RETAKED_LDATA = NULL;
void *CREDITS_LDATA = NULL;
u32 Text_Language = 0;
nufpcomjmp_s *LevelConfigKeywords_AfterLoad = NULL;

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
i32 PlayerID[2];
i16 Area_PlayerIDList[8];
i16 Area_StoryModelList[8];
PLAYERPROGRESS PlayerProgress[8];
i32 Hub_UsePlayerList = 0;
i32 LevelChangesInArea = 0;
i32 bonusmodearcade = 0;
i32 VehicleArea = 0;
SOCKPOSITION OldPlrSPos[8];
char Batarang[8 * 0xb4];
void *PlayerSuit[8];
u8 PlayerTorpedoCount[8];
COINPACKET COINPACKETS[2];
COINPACKET *CoinPacket = COINPACKETS;
u32 BackUpPlayers[0x872];
void *GizForceLOSInfo;
u8 DEFAULT_PLAYERHITPOINTS = 100;
u16 LEGOOBJ_DEFAULTLASTCOIN = 50;

PLAYERDATA *apicharsys;

TORPEDOPACKET TorpedoPackets[16];

// --- World-module helpers (kept with the WorldInfo API) ---

void SetAreaPickupGravity(i32 area, i32 level) {
    (void)area;
    (void)level;
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
    (void)gscn;
}
void CalculateWorldSize(WORLDINFO *world) {
    (void)world;
}

// Placeholder for LEVEL_PROGRESS structure
typedef struct LEVEL_PROGRESS_s {
    char data[0x2800];
    i32 flags;
} LEVEL_PROGRESS_s;

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
    memset(WorldInfo, 0, 0xa360);
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
    i32 *page_handles = (i32 *)&world->unknown_0140[0x2958];
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
    void *terrain_cur = *(void **)&world->unknown_0140[0x281c];
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
        i32 *src = (i32 *)&world->filler0[0x5c];
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
    SetPartRTLSet(*(i32 *)&world->unknown_0140[0x2974]);

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
        ResetTimer(LevelTimer, 0.0f);
    }

    if ((world->current_level->flags & (LEVEL_OUTRO | LEVEL_MIDTRO | LEVEL_INTRO)) == 0) {
        WorldInfo_LoadObjectAnimFile(world);
    }

    reset_restart = 1;
    *(i32 *)&world->filler1[0] = 1; // field_0x114

    InitGameObjectLights();

    *(i32 *)&world->unknown_0140[0x5034] = 1; // field_0x5174

    // Init last function
    Game_WorldInfo_InitLast(world);
}

void WorldInfo_Load(WORLDINFO *world) {
    char buf[268];
    char titles[64];
    LEVELDATA *level;
    void *cutscene_scene;
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

    if (abort_load != 0 || (world->unknown_010c > 0 && (LevelConfig_BeforeLoad(world->current_level, ConfigBuffer,
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
        world->current_gscn = (NUGSCN *)NuGScnRead(&world->giz_buffer, world->unknown_0108, buf);
        numtl_force_mipmode = 0;

        StoreSceneProgress(world->current_gscn, (SCENEPROGRESS_s *)&world->filler0[0x15c], 1);
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
        world->icons_gscn =
            (NUGSCN *)NuGScnRead(&world->giz_buffer, world->unknown_0108, "levels\\titles\\pictures.gsc");
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
    if (world->unknown_010c > 0) {
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
        world->icons_gscn =
            (NUGSCN *)NuGScnRead(&world->giz_buffer, world->unknown_0108, "stuff\\icons\\starwars_icons_all.gsc");
    }

    // Determine which scene to use for cutscenes
    cutscene_scene = area_scene;
    if (cutscene_scene == NULL || world->area == NULL || (world->area->flags & 0x400) == 0) {
        cutscene_scene = things_scene;
    }

    // Load cutscenes
    page_handles = (i32 *)&world->unknown_0140[0x2958];
    world->cutscene_sys = (CUTSYS *)CutScenes_Load(
        ConfigBuffer, world->current_gscn, (NUGSCN *)cutscene_scene, page_handles[0], &world->giz_buffer,
        &world->unknown_0108, *(i32 *)&world->unknown_0140[0x011c], *(i32 *)&world->unknown_0140[0x0120], world);
    if (abort_load != 0)
        goto abort;

    // Character scenes
    aligned_buf = ALIGN((i32)world->giz_buffer.addr, 4);
    *(i32 *)&world->minikit.field_0x18 = aligned_buf;
    world->giz_buffer.addr = (usize)(aligned_buf + CHARCOUNT * 0x10);
    CharScenes_LevelLoad(world);

    // SockSys for certain level types
    if ((level->flags & 0xe2) == 2) {
        world->sock_sys = SockSysInit(&world->giz_buffer, world->unknown_0108, world->current_gscn);
    }

    LevelSplines_InitForLevel(world);
    LevelObjects_InitForLevel(world);
    BoltTypes_Init(world);

    // Config-based subsystem initialization
    if (world->unknown_010c > 0) {
        LevelConfig_AfterLoad(world->current_level, ConfigBuffer, LevelConfigKeywords_AfterLoad);
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
        if (world->unknown_010c > 0) {
            SockSys_Configure(world->sock_sys, ConfigBuffer, 0, &world->giz_buffer, &world->unknown_0108,
                              world->current_gscn);
        }
        SockSys_GenerateData(world->sock_sys, &world->giz_buffer, &world->unknown_0108);
        if (abort_load != 0)
            goto abort;
    }

    // Terrain/grass/bridge loading
    if (world->area != NULL && (world->area->flags & 5) == 5 &&
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
    if ((level->flags & 0xe2) == 2 && level != (LEVELDATA *)PLATFORM_LDATA) {
        *(i32 *)&world->unknown_0140[0x29a4] = 0;
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
                                                                           (i32)(u8)level->unknown_103);
    } else {
        *(i32 *)&world->unknown_0140[0x29a4] = 1;
    }

    if (abort_load != 0)
        goto abort;

    // API object system
    world->api_object_sys = (APIOBJECTSYS_s *)APIObjectSysInit(0x10e4, &world->giz_buffer, &world->unknown_0108);
    if (abort_load != 0)
        goto abort;

    // Lights
    if ((level->flags & LEVEL_STATUS) == 0) {
        *(i32 *)&world->unknown_0140[0x2978] = -1;
        *(i32 *)&world->unknown_0140[0x297c] = 0;
    } else {
        light_path = world->config_file;
        LoadLights(world, light_path);
        rtl_id = rtlFindByUserId(*(i32 *)&world->unknown_0140[0x2974], 1);
        *(i32 *)&world->unknown_0140[0x2978] = rtl_id;
        if (rtl_id != -1) {
            rtlGetDirection(*(i32 *)&world->unknown_0140[0x2974], rtl_id, (void **)&world->unknown_0140[0x297c]);
        } else {
            *(i32 *)&world->unknown_0140[0x297c] = 0;
        }
    }

    if (abort_load != 0)
        goto abort;

    // More config-based subsystems
    if (world->unknown_010c > 0) {
        RippleEffects_Configure(world, ConfigBuffer);
        PortalDoors_Configure(world, ConfigBuffer);
        if (abort_load != 0)
            goto abort;
    }

    GizmoSysAddGizmos((GIZMOSYS_s *)world->gizmo_sys, (GIZFLOW_s *)*(void **)&world->unknown_0140[0x298c], world);
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

    SetAreaPickupGravity(*(i32 *)&world->unknown_0140[0x0120], *(i32 *)&world->unknown_0140[0x011c]);
    world->unknown_0110 = 1;
    return;

abort:
    WorldInfo_Dump(world);
    WorldInfo_Reset(world, -1);
}

i32 WorldInfo_Reset(WORLDINFO *world, i32 level_idx) {
    SetLevelExBlowupFlags(0);
    if (world->unknown_0110 != 0 && world->unknown_011c == level_idx && LDataList != NULL && level_idx >= 0 &&
        LDataList[level_idx].unknown_0af != -1 && new_level_from_menu == 0) {
        return 0;
    }

    if (world->unknown_0110 != 0) {
        WorldInfo_Dump(world);
    }
    if (new_level_from_menu != 0) {
        WORLDINFO *other = world == &WorldInfo[0] ? &WorldInfo[1] : &WorldInfo[0];
        if (other->unknown_0110 != 0 && other->unknown_011c == level_idx) {
            WorldInfo_Dump(other);
        }
    }

    // Save buffer pointers
    void *bufStart = *(void **)&world->filler0[0xFC];
    void *bufEnd = world->unknown_0108.void_ptr;

    // Clear the world and the portion of the streaming buffer owned by it.
    // The original keeps the two buffer cursors across a reset, but does not
    // leave stale level data in the newly selected world.
    if (bufStart != NULL && bufEnd != NULL && (char *)bufEnd > (char *)bufStart) {
        memset(bufStart, 0, (size_t)((char *)bufEnd - (char *)bufStart));
    }
    // The loader owns the prefix through 0x51b0.  The trailing processor and
    // timer storage is initialized separately and survives a level reset.
    memset(world, 0, 0x51b0);

    // Restore buffer pointers
    world->unknown_0108.void_ptr = bufEnd;
    *(void **)&world->filler0[0xFC] = bufStart;
    world->giz_buffer.void_ptr = bufStart;

    i32 *all_pages = (i32 *)&world->unknown_0140[0x2958];
    for (i32 i = 0; i != 6; ++i) {
        all_pages[i] = -1;
    }

    // Set level info
    world->unknown_011c = level_idx;
    world->unknown_0120 = -1;
    world->unknown_0124 = -1;

    // These page handles live in the level-owned portion of WORLDINFO.  A
    // reset must invalidate every page, otherwise the next load can mistake
    // a handle from the previous level for an already loaded resource.
    if (level_idx != -1) {
        LEVELDATA *levelData = &LDataList[level_idx];
        world->current_level = levelData;
        i32 areaIdx = (i8)levelData->unknown_0af;
        world->unknown_0120 = areaIdx;
        if (areaIdx != -1) {
            world->area = &ADataList[areaIdx];
            world->unknown_0124 = (i8)ADataList[areaIdx].episode_index;
        }

        i32 progress_index = (i8)levelData->unknown_0d4;
        if (progress_index >= 0 && progress_index < 12 && LevelProgressData != NULL) {
            world->level_progress = (LEVEL_PROGRESS_s *)((u8 *)LevelProgressData + progress_index * 0x2e24);
        }

        // Build config file path
        NuStrCpy(world->filler0, "levels\\");
        NuStrCat(world->filler0, levelData->dir);
        NuStrCat(world->filler0, "\\");
        NuStrCat(world->filler0, levelData->name);
        NuStrCpy(world->config_file, world->filler0);
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
    edbitsRegisterBaseScene(world->current_gscn);
    edanimRegisterBaseScene(world->current_gscn);
    edbitsRegisterBaseTerrain(*(void **)&world->unknown_0140[0x281c]);
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
    i32 *object_anim_page = (i32 *)&world->unknown_0140[0x2960];
    if (*object_anim_page == -1) {
        char path[256];
        strcpy(path, world->config_file);
        strcat(path, ".anm");
        if (NuFileExists(path)) {
            *object_anim_page = edanimLoadPage(path, world->current_gscn);
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
    u8 *visBuf = (u8 *)&world->unknown_0140[0x2851]; // field_0x2991
    world->unknown_0140[0x2850] = 1;                 // field_0x2990
    *(u8 **)&world->unknown_0140[0x2954] = visBuf;   // field_0x2a94
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
    VARIPTR *bufferEnd = (VARIPTR *)&WorldInfo[0].unknown_0140[0x5178];

    if (area1 == area2 || area1 == -1) {
        if (area1 != -1 && (ADataList[area1].flags & AREAFLAG_SINGLE_BUFFER) != 0) {
            return;
        }
    } else if ((ADataList[area1].flags & AREAFLAG_SINGLE_BUFFER) != 0) {
        LWORLD = &WorldInfo[0];
        WORLD = &WorldInfo[0];
        if (WorldInfo[0].unknown_0108.addr <= bufferEnd->addr) {
            return;
        }
        bufferEnd->addr = WorldInfo[0].unknown_0108.addr;
        WorldInfo[0].unknown_0108.addr = bufferEnd->addr - EditBufferEndSize;
        return;
    }

    if (WorldInfo[0].unknown_0108.addr <= bufferEnd->addr) {
        return;
    }
    usize end = WorldInfo[0].unknown_0108.addr;
    WorldInfo[0].unknown_0108.addr = bufferEnd->addr;
    bufferEnd->addr = end + EditBufferEndSize;
}

// ---------------------------------------------------------------------------
// World init/config functions (merged from levelstubs.cpp). The editor/level
// subsystems these belong to are not yet decompiled, so bodies are provisional
// empty stubs that keep the world-loading call graph linkable.
// ---------------------------------------------------------------------------
extern "C" void NuRndrInitWorld(void); // Defined in nurndr_android.c

void LevObj_FixUpPlatIDs(WORLDINFO *world) {
    (void)world;
}
void Doors_Init(WORLDINFO *world) {
    (void)world;
}
void LevelSplines_InitForLevel(WORLDINFO *world) {
    (void)world;
}
void LevelObjects_InitForLevel(WORLDINFO *world) {
    (void)world;
}
void BoltTypes_Init(WORLDINFO *world) {
    (void)world;
}
void BoltTypes_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void EquivalentObjects_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Teleports_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Doors_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Faders_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void CharPlatforms_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Grabber_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Pulses_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void TrafficAnimSys_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void SpecialMiniKits_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void GizForceSFX_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void RippleEffects_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void PortalDoors_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void LoadLights(WORLDINFO *world, char *path) {
    (void)world;
    (void)path;
}
void *GameAnimSys_Create(VARIPTR *buf, VARIPTR *buf_end) {
    (void)buf;
    (void)buf_end;
    return NULL;
}
void *GameAntnode_CreateSys(WORLDINFO *world, VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)world;
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}

// C-linkage symbol set (kept in TU; consolidation deferred).
// --- Extern "C": functions with C linkage in original ---
extern "C" {
    void SockSys_Configure(void *sock_sys, char *config, i32 param, void *buf, void *buf_end, void *gscn) {
        (void)sock_sys;
        (void)config;
        (void)param;
        (void)buf;
        (void)buf_end;
        (void)gscn;
    }
    void SockSys_GenerateData(void *sock_sys, void *buf, void *buf_end) {
        (void)sock_sys;
        (void)buf;
        (void)buf_end;
    }
    void rtlResetDynamic(void) {
    }
    void SetPartRTLSet(i32 rtl_set) {
        (void)rtl_set;
    }
    i32 rtlFindByUserId(i32 rtl_set, i32 user_id) {
        (void)rtl_set;
        (void)user_id;
        return -1;
    }
    void rtlGetDirection(i32 rtl_set, i32 id, void **out) {
        (void)rtl_set;
        (void)id;
        (void)out;
    }
    void NewMenu(i32 menu_id, i32 menu_y, i32 param3) {
        (void)menu_id;
        (void)menu_y;
        (void)param3;
    }
} // extern "C"
