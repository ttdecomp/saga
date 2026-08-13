#include "legoapi/world_shared.h"

#include <stdio.h>

#include "gameapi/edtools/edstubs.h"
#include "legoapi/level.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"

// Debris and terrain globals — accessed from DebrisSetThinningLevel etc.
f32 debris_thinning_level;
i32 forced_debris_thinning;
i32 debris_detail_level;
char *debris_name = NULL;
i32 Grass_Available = 1;
i32 PDEBCOUNT = 0;
void *PDebNameList = NULL;

extern "C" void DebrisSetThinningLevel(f32 level) {
    debris_thinning_level = level < 1.0f ? 1.0f : level;
}
extern "C" void DebrisSetForcedThinning(i32 forced) {
    forced_debris_thinning = forced;
}
extern "C" void DebrisSetDetailLevel(i32 level) {
    debris_detail_level = level;
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

// Stubs for WorldInfo_Load dependencies
extern "C" void *TerrainInitEx(i32 param1, void *buf, void *buf_end, i32 param2, char *path, void *gscn, i32 param3,
                               u32 param4, u32 param5, u32 param6) {
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
void *InitPartDebris(VARIPTR *buf, VARIPTR *buf_end, i32 param1, i32 param2, char **param3, i32 page) {
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
            InitPartDebris(&world->giz_buffer, &world->unknown_0108, 0x40, PDEBCOUNT, (char **)PDebNameList, page);
        *(void **)&world->unknown_0140[0x2960] = partDebrisSys;
    }
}
void Particles_Load(WORLDINFO *world, char **debris_name, i32 count, i32 flags) {
    (void)world;
    (void)debris_name;
    (void)count;
    (void)flags;
}
