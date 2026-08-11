#include "legoapi/world.h"

#include <stdio.h>
#include <string.h>

#include "gameapi/edtools/edstubs.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/area.h"
#include "legoapi/character.h"
#include "legoapi/level.h"
#include "legoapi/socksys.h"
#include "legoapi/timer.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"

extern "C" {
    void NuDisplaySceneRndr(void *scene) {
        (void)scene;
    }

    void NuPortalVisibility(NUGSCN *scene) {
        (void)scene;
    }
    void NuGScnRndr3(NUGSCN *scene) {
        NuDisplaySceneRndr(scene->display_list);
    }
}

// Debris and camera globals — accessed from DebrisSetThinningLevel etc.
f32 debris_thinning_level;
i32 forced_debris_thinning;
i32 debris_detail_level;
f32 CameraZoom;

// Graphics loading flags
i32 RemoveDirectionalMaps = 0;
i32 RemoveNormalMaps = 0;

extern "C" void DebrisSetThinningLevel(f32 level) {
    debris_thinning_level = level < 1.0f ? 1.0f : level;
}
extern "C" void DebrisSetForcedThinning(i32 forced) {
    forced_debris_thinning = forced;
}
extern "C" void DebrisSetDetailLevel(i32 level) {
    debris_detail_level = level;
}
void SetCameraZoom(f32 zoom) {
    CameraZoom = zoom;
}
i32 ActionFromQuiet(i32 idx) {
    static i16 ActionPairTab[14] = {-1};
    if (idx != -1) {
        i16 *pair = ActionPairTab;
        while (*pair != -1) {
            if (*pair == idx) {
                return pair[1];
            }
            pair += 14;
        }
    }
    return -1;
}
i32 AmbientFromQuiet(i32 idx) {
    static i16 AmbientPairTab[2] = {-1};
    if (idx != -1) {
        i16 *pair = AmbientPairTab;
        while (*pair != -1) {
            if (*pair == idx) {
                return pair[1];
            }
            pair += 2;
        }
    }
    return -1;
}

// Globals shared across world loading — defined here until moved to globals.cpp
TIMER *LevelTimer = NULL;
i32 abort_load = 0;
char *ConfigBuffer = NULL;
i32 numtl_force_mipmode = 0;
char *debris_name = NULL;
void *MiniKitCollection = NULL;
i32 GAMEDEMO = 0;
void *big_icon_scene = NULL;
void *area_scene = NULL;
void *things_scene = NULL;
LEVELDATA *PLATFORM_LDATA = NULL;
void *RETAKED_LDATA = NULL;
void *CREDITS_LDATA = NULL;
u32 Text_Language = 0;
nufpcomjmp_s *LevelConfigKeywords_AfterLoad = NULL;
i32 Grass_Available = 1;
i32 PDEBCOUNT = 0;
i32 CharScene_Area = 0;
void *PDebNameList = NULL;

// Stub implementations for functions not yet implemented in their respective modules.
// NOTE: Functions that have C++ linkage in the original libTTapp.so are kept
// outside extern "C" so their mangled names match.  Functions with true C
// linkage in the original are individually marked `extern "C"`.

extern "C" {
    void ResetSounds(void) {
    }
} // extern "C"

// --- C++ linkage stubs (original uses C++ mangling) ---

void SetLevelSfxBits(WORLDINFO *world) {
    (void)world;
}
void ResetLevSfx(WORLDINFO *world) {
    // SFX bit array and counter in the unknown_4670[] filler region.
    // TODO: these offsets must be replaced with typed struct fields.
    //   0x4720 → unknown_4670[0xb0]: SFX bit array (0x400 bytes, stride 0x10)
    //   0x4b14 → unknown_4670[0x4a4]: SFX counter
    i16 *sfx = (i16 *)&world->unknown_4670[0xb0];
    for (i32 i = 0; i < 0x40; i++) {
        sfx[i] = -1;
    }
    *(i32 *)&world->unknown_4670[0x4a4] = 0;
}

extern "C" void noterraininit(void) {
}
extern "C" void TerrainSetCur(void *terrain) {
    (void)terrain;
}
extern "C" void TerrSetPlatScanDist(f32 dist) {
    (void)dist;
}
extern "C" void TerrainPlatformOldUpdate(void) {
}
extern "C" void TerrainPlatformNewUpdate(void) {
}
extern "C" void TerrainSetWallDeflectYScale(f32 scale) {
    (void)scale;
}

// --- More C++ linkage stubs ---

void LevObj_FixUpPlatIDs(WORLDINFO *world) {
    (void)world;
}
extern "C" void NuRndrInitWorld(void); // Defined in nurndr_android.c

extern "C" void NuGScnUpdate(NUGSCN *gscn, i32 param) {
    (void)gscn;
    (void)param;
}

void Doors_Init(WORLDINFO *world) {
    (void)world;
}
void Players_InitPositions(WORLDINFO *world) {
    (void)world;
}
void ClearGameObjects(void *api_object_sys) {
    (void)api_object_sys;
}
void PlayerItemTypes_Reset(WORLDINFO *world) {
    (void)world;
}
void Players_Init(void) {
}

extern "C" void rtlResetDynamic(void) {
}
extern "C" void SetPartRTLSet(i32 rtl_set) {
    (void)rtl_set;
}

void InitGameObjectLights(void) {
}
void NewMenu(i32 menu_id, i32 menu_y, i32 param3) {
    (void)menu_id;
    (void)menu_y;
    (void)param3;
}

// Global variables needed by loading functions

// Stubs for WorldInfo_Load dependencies
void *TerrainInitEx(i32 param1, void *buf, void *buf_end, i32 param2, char *path, void *gscn, i32 param3, u32 param4,
                    u32 param5, u32 param6) {
    (void)param1;
    (void)buf;
    (void)buf_end;
    (void)param2;
    (void)path;
    (void)gscn;
    (void)param3;
    (void)param4;
    (void)param5;
    (void)param6;
    return NULL;
}
void *InitPartDebris(void *buf, void *buf_end, i32 param1, i32 param2, void *param3, i32 page) {
    (void)buf;
    (void)buf_end;
    (void)param1;
    (void)param2;
    (void)param3;
    (void)page;
    return NULL;
}
void LoadTerrainFile(WORLDINFO *world) {
    char path[256];
    *(i32 *)&world->unknown_0140[0x281c] = 0;
    if ((world->current_level->flags & LEVEL_UNKNOWN_FLAG_8) != 0) {
        NuStrCpy(path, world->config_file);
        LEVELDATA *level = world->current_level;
        if (level == (LEVELDATA *)PLATFORM_LDATA) {
            NuStrCpy(path, "levels\\episode_v\\cloudcityescape\\cloudcityescape_b\\cloudcityescape_b");
            level = world->current_level;
        }
        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
        void *terrain = TerrainInitEx(*(i32 *)&world->unknown_0140[0x011c], &world->giz_buffer, &world->unknown_0108, 0,
                                      path, world->current_gscn, 0, (u32)(u16)level->max_ter_groups,
                                      (u32)(u16)level->max_ter_groups, (u32)(u16)level->max_ter_platforms);
        *(void **)&world->unknown_0140[0x281c] = terrain;
    }
}
void LoadGrassFile(WORLDINFO *world) {
    char path[268];
    *(i32 *)&world->unknown_0140[0x2964] = -1;
    Grass_Available = 1;
    char *config = world->config_file;

    if (g_isLowEndDevice != 0) {
        char *found = NuStrIStr(config, "Gungan");
        if (found == NULL)
            found = NuStrIStr(config, "SpeederChase");
        if (found == NULL)
            found = NuStrIStr(config, "EndorBattle");
        if (found == NULL)
            found = NuStrIStr(config, "Retake");

        if (found != NULL) {
            Grass_Available = 0;
        } else {
            Grass_Available = 1;
        }
    }

    if (g_isMidRangeDevice != 0 && NuStrIStr(config, "GunGan_A") != NULL) {
        Grass_Available = 0;
        return;
    }

    char *found = NuStrIStr(config, "SpeederChase");
    if (found != NULL) {
        Grass_Available = 0;
    }

    if (Grass_Available != 0) {
        sprintf(path, "%s.gra", config);
        if (NuFileExists(path)) {
            i32 page = edgraLoadPage(path, world->current_gscn, *(i32 *)&world->unknown_0140[0x281c],
                                     &world->giz_buffer, &world->unknown_0108);
            *(i32 *)&world->unknown_0140[0x2964] = page;
        }
    }
}
void LoadBridgeFile(WORLDINFO *world) {
    char path[256];
    *(i32 *)&world->unknown_0140[0x2968] = -1;
    sprintf(path, "%s.bri", world->config_file);
    if (NuFileExists(path)) {
        i32 page = edbriLoadPage(path, world->current_gscn);
        *(i32 *)&world->unknown_0140[0x2968] = page;
    }
}
void LoadPartFile(WORLDINFO *world) {
    char path[256];
    *(i32 *)&world->unknown_0140[0x295c] = -1;
    edpartSetParticlePage(*(i32 *)&world->unknown_0140[0x2958]);

    if ((world->current_level->flags & (LEVEL_OUTRO | LEVEL_MIDTRO | LEVEL_INTRO)) == 0) {
        sprintf(path, "%s.par", world->config_file);
        i32 page = -1;
        if (NuFileExists(path)) {
            page = edpartLoadPage(path, 1, world->current_gscn);
            *(i32 *)&world->unknown_0140[0x295c] = page;
        }
        void *partDebrisSys =
            InitPartDebris(&world->giz_buffer, &world->unknown_0108, 0x40, PDEBCOUNT, PDebNameList, page);
        *(void **)&world->unknown_0140[0x2960] = partDebrisSys;
    }
}
void Particles_Load(WORLDINFO *world, char **debris_name, i32 count, i32 flags) {
    (void)world;
    (void)debris_name;
    (void)count;
    (void)flags;
}
void *CreateGizmoSys(void *world, void *buf, void *buf_end) {
    (void)world;
    (void)buf;
    (void)buf_end;
    return NULL;
}
void LoadGizmoSys(void *gizmo_sys, void *world, char *config_file) {
    (void)gizmo_sys;
    (void)world;
    (void)config_file;
}
void LoadEditorSplines(char *path, void *buf, void *buf_end) {
    (void)path;
    (void)buf;
    (void)buf_end;
}
void GizmoBlowupResetNameTable(void) {
}
void Hub_LoadAndFixUpMiniKits(WORLDINFO *world, void *buf, void *buf_end) {
    (void)world;
    (void)buf;
    (void)buf_end;
}
void MiniKit_Load(void *minikit, i32 id, void *buf, void *buf_end, void *param) {
    (void)minikit;
    (void)id;
    (void)buf;
    (void)buf_end;
    (void)param;
}
void MiniKit_InitPieces(void *minikit, i32 count, void *buf) {
    (void)minikit;
    (void)count;
    (void)buf;
}
struct CUTINFO_LOAD {
    void *cutscene;
    void *instance;
    char data[0x13c];
};

struct CUTSYS_LOAD {
    void *entries;
    i32 count;
    void *end;
};

i32 CUTCOUNT = 0;
CUTINFO_LOAD *CutList = NULL;
i32 ACTIVECUTCOUNT = 0;
i32 CS_area = 0;
CUTSYS_LOAD *CS_cutsys = NULL;
WORLDINFO *CS_worldinfo = NULL;
f32 CutSceneScale = 1.0f;
i32 CUTCAM = 0;
extern i32 InStory(void) {
}

// These routines are implemented by the cutscene subsystem.  Keep the
// declarations here rather than hiding the calls behind local no-ops:
// CutScenes_Load is part of the original loader and its ABI calls these
// entry points directly.
extern "C" {
    extern void *NuGCutSceneLoad(char *name, NUGSCN *gscn1, NUGSCN *gscn2, i32 flags);
    extern void NuGCutSceneFixUp(void *cutscene, char *name, i32 flags, VARIPTR *end);
    extern void NuGCutSceneFixUpExtra(void *cutscene, i32 area);
    extern void *instNuGCutSceneCreate(void *cutscene, i32 flags, i32 param, char *name);
}

__attribute__((noinline)) static void CutScene_Configure_Load(CUTINFO_LOAD *cut, char *name, VARIPTR *buf,
                                                              VARIPTR *buf_end) {
    (void)buf;
    (void)buf_end;
    memset(cut, 0, sizeof(*cut));
    NuStrCpy((char *)cut->data, name);
}

void *CutScenes_Load(char *config, NUGSCN *gscn1, NUGSCN *gscn2, i32 param1, VARIPTR *buf, VARIPTR *buf_end, i32 param2,
                     i32 param3, WORLDINFO *world) {
    NUFPAR *fp;
    CUTSYS_LOAD *sys;
    void *initial;
    CUTINFO_LOAD *cut;
    char name[128];
    char path[128];
    char full_path[128];
    CUTINFO_LOAD *entries[32];

    if (CutList != NULL && CUTCOUNT > 0) {
        sys = (CUTSYS_LOAD *)ALIGN(buf->addr, 4);
        sys->entries = sys + 1;
        sys->count = CUTCOUNT;
        CS_area = param2;
        CS_worldinfo = world;
        CS_cutsys = sys;
        buf->void_ptr = (char *)sys->entries + CUTCOUNT * 0x144;
        return sys;
    }
    if (config == NULL) {
        return NULL;
    }

    fp = NuFParCreateMem((char *)"cutscenes", config, 0xffff);
    if (fp == NULL) {
        return NULL;
    }

    initial = buf->void_ptr;
    sys = (CUTSYS_LOAD *)ALIGN((usize)initial, 4);
    sys->entries = sys + 1;
    sys->count = 0;
    sys->end = (char *)sys + 0xc;
    CS_area = param2;
    CS_worldinfo = world;
    CS_cutsys = sys;
    buf->void_ptr = (char *)sys->entries + ((CHARCOUNT + 0x1f) >> 5) * 4;

    while (NuFParGetLine(fp) != 0) {
        if (NuFParGetWord(fp) == 0 || NuStrICmp(fp->word_buf, "cutscene") != 0 || sys->count > 0x1f ||
            NuFParGetWord(fp) == 0) {
            continue;
        }

        cut = (CUTINFO_LOAD *)buf->void_ptr;
        entries[sys->count] = cut;
        buf->void_ptr = (char *)cut + 0x144;
        NuStrCpy(name, fp->word_buf);
        for (char *lower = name; *lower != '\0'; ++lower) {
            *lower = (char)NuToLower((u8)*lower);
        }
        i32 len = NuStrLen(name);
        while (len > 0 && name[len - 1] != '.') {
            --len;
        }
        if (len > 0) {
            name[len - 1] = '\0';
        }

        if (name[0] == 'c' && name[1] == 'u' && name[2] == 't' && name[3] == '\\') {
            NuStrCpy(path, name);
        } else {
            NuStrCpy(path, "cut\\");
            NuStrCat(path, name);
            NuStrCat(path, ".cut");
        }
        NuStrCpy(full_path, "cutscenes");
        NuStrCat(full_path, "\\");
        NuStrCat(full_path, path);
        CutScene_Configure_Load(cut, full_path, buf, buf_end);

        if ((cut->data[0x51] & 8) == 0 && !InStory()) {
            continue;
        }
        buf->void_ptr = (char *)ALIGN(buf->addr, 0x40);
        NuStrCpy(full_path, "cutscenes");
        NuStrCat(full_path, "\\");
        NuStrCat(full_path, path);
        cut->cutscene = NuGCutSceneLoad(full_path, gscn1, gscn2, 0);
        if (cut->cutscene == NULL) {
            continue;
        }
        NuGCutSceneFixUp(cut->cutscene, name, 0, buf_end);
        NuGCutSceneFixUpExtra(cut->cutscene, param1);
        cut->instance = instNuGCutSceneCreate(cut->cutscene, 0, 0, name);
        if (cut->instance != NULL) {
            *(f32 *)((char *)cut->instance + 0x98) = *(f32 *)((char *)cut + 0x5c) * CutSceneScale;
            buf->void_ptr = (char *)ALIGN(buf->addr, 0x10);
        }
        sys->count++;
    }
    NuFParDestroy(fp);
    if (sys->count == 0) {
        buf->void_ptr = initial;
        return NULL;
    }
    buf->void_ptr = initial;
    memmove(initial, entries, sys->count * sizeof(void *));
    buf->void_ptr = (void *)ALIGN((usize)initial + sys->count * 4, 4);
    sys->entries = initial;
    return sys;
}
void CharScenes_LevelLoad(WORLDINFO *world) {
    if (CHARCOUNT <= 0) {
        return;
    }

    for (int i = 0; i < CHARCOUNT; i++) {
        void **entry = (void **)(*(i32 *)&world->char_scene_info.minikit.field_0x18 + i * 0x10);
        *entry = NULL;

        // Check if we should load this character scene
        if ((CharScene_Area == 0 || *(i32 *)(CharScene_Area + i * 0x10) == 0) && (CDataList[i].flags & 1) != 0 &&
            world->cutscene_sys != NULL) {
            // Check if this character is in a cutscene
            u32 *cutscene_flags = *(u32 **)((char *)world->cutscene_sys + 8);
            u32 flag = (cutscene_flags[i >> 5] >> (i & 0x1f)) & 1;
            if (flag != 0) {
                // Load the character scene
                char path[136];
                VARIPTR buf_end = world->unknown_0108;
                sprintf(path, "chars\\%s\\%s.gsc", CDataList[i].dir, CDataList[i].file);
                NUGSCN *scene = (NUGSCN *)NuGScnRead(&world->giz_buffer, buf_end, path);
                *entry = scene;
                if (scene != NULL) {
                    NuSpecialFind(scene, (void **)(entry + 1), CDataList[i].file);
                }
            }
        }
    }
}

// --- Extern "C" block: functions with confirmed C linkage in original libTTapp.so ---
extern "C" {

    void *NuGCutSceneLoad(char *name, NUGSCN *gscn1, NUGSCN *gscn2, i32 flags) {
        (void)name;
        (void)gscn1;
        (void)gscn2;
        (void)flags;
        return NULL;
    }
    void NuGCutSceneFixUp(void *cutscene, char *name, i32 flags, VARIPTR *end) {
        (void)cutscene;
        (void)name;
        (void)flags;
        (void)end;
    }
    void NuGCutSceneFixUpExtra(void *cutscene, i32 area) {
        (void)cutscene;
        (void)area;
    }
    void *instNuGCutSceneCreate(void *cutscene, i32 flags, i32 param, char *name) {
        (void)cutscene;
        (void)flags;
        (void)param;
        (void)name;
        return NULL;
    }
} // extern "C"

// --- C++ linkage stubs (original uses C++ mangling for these) ---

void NuSpecialFind(NUGSCN *scene, void **dest, char *name) {
    (void)scene;
    (void)dest;
    (void)name;
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

// --- Extern "C": SockSys functions have C linkage in original ---
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
} // extern "C"

// --- More C++ linkage stubs ---

void CharacterMiniKits_Load(void *collection, WORLDINFO *world, void *buf, void *buf_end) {
    (void)collection;
    (void)world;
    (void)buf;
    (void)buf_end;
}
void *AISysLoad(void *buf, void *buf_end, i32 size, void *gscn, char *dir, char *name, char *param) {
    (void)buf;
    (void)buf_end;
    (void)size;
    (void)gscn;
    (void)dir;
    (void)name;
    (void)param;
    return NULL;
}
void *AIPathCnxControlSysCreate(void *buf, void *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
void *AIPathCnxHelperSysCreate(void *buf, void *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
void *AITriggerSetSysCreate(void *buf, void *buf_end) {
    (void)buf;
    (void)buf_end;
    return NULL;
}
void GameAIScriptAddLevelSfx(WORLDINFO *world, void *scripts) {
    (void)world;
    (void)scripts;
}
void *CreateClimbObjectSys(void *buf, void *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
void *APIObjectSysInit(i32 size, void *buf, void *buf_end) {
    (void)size;
    (void)buf;
    (void)buf_end;
    return NULL;
}
void LoadLights(WORLDINFO *world, char *path) {
    (void)world;
    (void)path;
}

// --- Extern "C": rtl functions have C linkage in original ---
extern "C" {
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
} // extern "C"

// --- More C++ linkage stubs ---

void RippleEffects_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void PortalDoors_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void GizmoSysAddGizmos(void *gizmo_sys, void *giz_flow, void *world) {
    (void)gizmo_sys;
    (void)giz_flow;
    (void)world;
}
void *LoadGizFlow(void *world, void *gizmo_sys, char *path, void *buf, void *buf_end) {
    (void)world;
    (void)gizmo_sys;
    (void)path;
    (void)buf;
    (void)buf_end;
    return NULL;
}
void InitSpecialSfx(WORLDINFO *world) {
    (void)world;
}
void LoadSpecialSfxFile(WORLDINFO *world) {
    (void)world;
}
void WorldInfo_Dump(WORLDINFO *world) {
    (void)world;
}
void StoreSceneProgress(void *gscn, void *progress, i32 param) {
    (void)gscn;
    (void)progress;
    (void)param;
}
void SaveSceneObjectAnimTFactors(void *gscn) {
    (void)gscn;
}
void CalculateWorldSize(WORLDINFO *world) {
    (void)world;
}
void *GameAnimSys_Create(void *buf, void *buf_end) {
    (void)buf;
    (void)buf_end;
    return NULL;
}
void *GameAntnode_CreateSys(WORLDINFO *world, void *buf, void *buf_end, i32 count) {
    (void)world;
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}

// --- Extern "C": NuGScn functions have C linkage in original ---
extern "C" {
    i32 NuGScnUploadGfxDataFromFilePS(NUFILE file, VARIPTR *buf, VARIPTR buf_end) {
        (void)file;
        (void)buf;
        (void)buf_end;
        return 0;
    }

    void *NuGScnRead(VARIPTR *buf, VARIPTR buf_end, char *path) {
        extern NUGSCN *NuReadGraphicsData(VARIPTR *, VARIPTR *, char *, NUGSCN *);
        RemoveDirectionalMaps = 1;
        RemoveNormalMaps = 1;
        NUGSCN *scene = NuReadGraphicsData(buf, &buf_end, path, NULL);
        RemoveNormalMaps = 0;
        RemoveDirectionalMaps = 0;
        return scene;
    }
    void NuGScnReadFromMemory(NUGSCN *scene) {
        extern NUGSCN *NuReadGraphicsData(VARIPTR *, VARIPTR *, char *, NUGSCN *);
        NuReadGraphicsData(NULL, NULL, NULL, scene);
    }
    void NuGHGFixup(NUGSCN *scene) {
        NuGScnReadFromMemory(scene);
    }

    NUGSCN *NuReadGraphicsData(VARIPTR *buf, VARIPTR *buf_end, char *path, NUGSCN *scene) {
        if (scene != NULL) {
            return scene;
        }

        char converted_path[1033];
        NuFileExtConvert(converted_path, path, 0x400);
        NUFILE file = NuFileOpen(converted_path, NUFILE_READ);
        if (file == 0) {
            return NULL;
        }

        i32 file_size = (i32)NuFileOpenSize(file);
        buf->addr = ALIGN(buf->addr, 0x20);
        i32 uploaded = NuGScnUploadGfxDataFromFilePS(file, buf, *buf_end);
        NUGSCN *loaded = (NUGSCN *)ALIGN(buf->addr, 0x20);
        buf->addr = (usize)((char *)loaded + file_size - uploaded);
        NuFileRead(file, loaded, file_size - uploaded);
        NuFileClose(file);
        return loaded;
    }
} // extern "C"
void SetAreaPickupGravity(i32 area, i32 level) {
    (void)area;
    (void)level;
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
    ClearGameObjects(world->ai_sys);
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

        StoreSceneProgress(world->current_gscn, (void *)&world->filler0[0x15c], 1);
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
        world->unknown_0140_ptr =
            (NUGSCN *)NuGScnRead(&world->giz_buffer, world->unknown_0108, "levels\\titles\\pictures.gsc");
        if (abort_load != 0)
            goto abort;
    }

    // Create game animation system
    world->game_anim_sys = GameAnimSys_Create(&world->giz_buffer, &world->unknown_0108);

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
    world->game_antinode_sys = GameAntnode_CreateSys(world, &world->giz_buffer, &world->unknown_0108,
                                                     (i32)(u16)world->current_level->max_antinodes);

    // Create gizmo system
    world->gizmo_sys = CreateGizmoSys(world, &world->giz_buffer, &world->unknown_0108);

    // Load editor splines
    sprintf(buf, "%s%s", world->config_file, ".spl");
    LoadEditorSplines(buf, &world->giz_buffer, &world->unknown_0108);

    GizmoBlowupResetNameTable();
    LoadGizmoSys(world->gizmo_sys, world, world->config_file);
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
        MiniKit_Load(&world->char_scene_info.minikit, (i32)(i16)world->area->minikit_id, &world->giz_buffer,
                     &world->unknown_0108, NULL);
        if (world->char_scene_info.minikit.gscn != NULL) {
            MiniKit_InitPieces(&world->char_scene_info.minikit, 10, &world->giz_buffer);
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
    world->cutscene_sys = CutScenes_Load(ConfigBuffer, world->current_gscn, (NUGSCN *)cutscene_scene, page_handles[0],
                                         &world->giz_buffer, &world->unknown_0108, *(i32 *)&world->unknown_0140[0x011c],
                                         *(i32 *)&world->unknown_0140[0x0120], world);
    if (abort_load != 0)
        goto abort;

    // Character scenes
    aligned_buf = ALIGN((i32)world->giz_buffer.addr, 4);
    *(i32 *)&world->char_scene_info.minikit.field_0x18 = aligned_buf;
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
        CharacterMiniKits_Load(MiniKitCollection, world, &world->giz_buffer, &world->unknown_0108);
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
        world->ai_path_cnx_control_sys = AIPathCnxControlSysCreate(&world->giz_buffer, &world->unknown_0108, 0x40);
        world->ai_path_cnx_helper_sys = AIPathCnxHelperSysCreate(&world->giz_buffer, &world->unknown_0108, 0x20);
        world->ai_trigger_set_sys = AITriggerSetSysCreate(&world->giz_buffer, &world->unknown_0108);

        if (world->ai_sys != NULL) {
            world->mech_auto_jump_manager = (void *)1; // placeholder
        }

        GameAIScriptAddLevelSfx(world, &global_aiscripts);
        GameAIScriptAddLevelSfx(world, &world->ai_sys->scripts);

        world->climb_object_sys =
            CreateClimbObjectSys(&world->giz_buffer, &world->unknown_0108, (i32)(u8)level->unknown_103);
    } else {
        *(i32 *)&world->unknown_0140[0x29a4] = 1;
    }

    if (abort_load != 0)
        goto abort;

    // API object system
    world->api_object_sys = APIObjectSysInit(0x10e4, &world->giz_buffer, &world->unknown_0108);
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

    GizmoSysAddGizmos(world->gizmo_sys, *(void **)&world->unknown_0140[0x298c], world);
    if (abort_load != 0)
        goto abort;

    // Load gizmo flow
    NuStrCpy(buf, world->config_file);
    NuStrCat(buf, ".git");
    world->giz_flow = LoadGizFlow(world, world->gizmo_sys, buf, &world->giz_buffer, &world->unknown_0108);
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
            world->level_progress = (u8 *)LevelProgressData + progress_index * 0x2e24;
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
