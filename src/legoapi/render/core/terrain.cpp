#include "legoapi/world/world_shared.h"

#include <stdio.h>

#include "gameapi/edtools/edstubs.h"
#include "legoapi/world/level.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"

struct tertype;
struct terrsitu_s;
struct PLATSKININFO;
struct GAMECAMERA_s;
struct pushblock_s;

// Debris and terrain globals — accessed from DebrisSetThinningLevel etc.
f32 debris_thinning_level;
i32 forced_debris_thinning;
i32 debris_detail_level;
// char *debris_name[147] @0x61e860 (.data): the static debris effect names,
// seeded into every world's debris system by InitGameDebris.
char *debris_name[147] = {
    "BULLET_SPARK",   "SABER_RED",     "SABER_GREEN",   "SABER_BLUE",    "SABER_PURPLE",   "EXPLD_1A",
    "JS_THRUST",      "S1_THRUST",     "COIN_BLUE",     "COIN_GOLD",     "COIN_SILVER",    "R2D2THRUSTER",
    "R2Q5THRUSTER",   "SPLASH",        "RIPPLE",        "HEART",         "HEART_BIG",      "EXPLOSION",
    "MINI_01",        "MINI_02",       "MINI_03",       "TER_SPARK",     "TER_SPARK2",     "EXPOL_02",
    "SPEEDERDUST",    "POD_DUST",      "CAVE_DUST",     "JANGOTHRUSTER", "EXPLO_DROID",    "EXPLO_NAB",
    "NAB_SMOKE",      "NAB_BOOST",     "MIKESMOKE",     "EXPLO_05",      "EXPLO_06",       "EXPLO_07",
    "EXPLO_11",       "MIKESTALL",     "TRAINING_01",   "TAG_BLUE",      "TAG_GREEN",      "ZIP_TARGET",
    "DOOKU_BALL",     "WALKER_01",     "WALKER_02",     "ENG_BLOW",      "DogImpact",      "DogEngine",
    "DogSmoke",       "DogTrail",      "YODA_HOVER_01", "K_Bolt",        "DUK_P2",         "PodHaze",
    "PodDust10",      "rock_pop1",     "BlueTrail",     "PurpleTrail",   "LavaDie",        "ComboRed",
    "ComboGreen",     "ComboBlue",     "ComboPurple",   "REPAIR",        "SABER_RED1",     "SABER_GREEN1",
    "SABER_BLUE1",    "SABER_PURPLE1", "XWING_1",       "MOUSE_POP",     "PHOTON",         "PHOTON_EXPLO",
    "PHOTON_EXPLO_S", "EXPLO_ORAN_2A", "EXPLO_ORAN_2B", "EXPLO_ORAN_2C", "BUILD_IT1",      "BUILD_IT2",
    "BUILD_IT3",      "BUILD_IT4",     "BUILD_IT5",     "BUILD_IT6",     "DRAG_POP_1",     "DRAG_POP_2",
    "DRAG_POP_4",     "GenoGun",       "SNOW_SPEEDER1", "LEVER_SPARK",   "ZAPPER_1",       "V_BOLT_RED",
    "V_BOLT_GREEN",   "CHAR_SMOKE1",   "CHAR_SMOKE2",   "POWER_P_1",     "POWER_P_2",      "POWER_P_3",
    "TIE_HIT1",       "BRICK_01",      "BRICK_02",      "BRICK_03",      "BUMP_01",        "BUMP_02",
    "LAND_S_DUST1",   "LAND_S_DUST2",  "LAND_S_THRUST", "LAND_S_HAZE",   "EXPLO_06",       "EXPLO_06b",
    "BUMP_03",        "FORCE_BLUE",    "FORCE_RED",     "SCAN3",         "REPULSOR",       "WEE_POP",
    "EXIT_FIRE_1",    "EXIT_FIRE_2",   "EWOK_POP1",     "EXPLO_02",      "ATAT_POP_1",     "ATAT_POP_2",
    "ATAT_POP_3",     "FIRE_1",        "RAIN_1",        "CHAR_BUBBLE",   "CHAR_GHOST",     "DEATH_1",
    "DEATH_2",        "BOULDER_2",     "DISH_1",        "DISH_2",        "SNOW_POP_1",     "SNOW_POP_2",
    "TREE_POP_1",     "TREE_POP_4",    "SPEEDER_HIT",   "FALCONGLOW",    "FALCONTHRUSTER", "SPEED_SPARK",
    "PUNCH_1",        "PUNCH_2",       "GUNSHIP_01",    "GUNSHIP_02",    "SandBlast",      "BULLET1",
    "BULLET2",        "BULLET_HIT",    "POD_SPARK",
};
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
void LoadTerrainFile(WORLDINFO *world) {
    char path[256];
    world->terrain = NULL;
    if ((world->current_level->flags & LEVEL_TERRAIN) != 0) {
        NuStrCpy(path, world->config_file);
        LEVELDATA *level = world->current_level;
        if (level == (LEVELDATA *)PLATFORM_LDATA) {
            NuStrCpy(path, "levels\\episode_v\\cloudcityescape\\cloudcityescape_b\\cloudcityescape_b");
            level = world->current_level;
        }
        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
        // i32 read into the progress_data region (offset 0x25c).
        void *terrain = TerrainInitEx(*(i32 *)((char *)world + 0x25c), &world->giz_buffer, &world->unknown_0108, 0,
                                      path, world->current_gscn, 0, (u32)(u16)level->max_ter_groups,
                                      (u32)(u16)level->max_ter_groups, (u32)(u16)level->max_ter_platforms);
        world->terrain = terrain;
    }
}
void LoadGrassFile(WORLDINFO *world) {
    char path[268];
    world->page_grass = -1;
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
            i32 page = edgraLoadPage(path, world->current_gscn, *(i32 *)&world->terrain, &world->giz_buffer,
                                     &world->unknown_0108);
            world->page_grass = page;
        }
    }
}
void LoadBridgeFile(WORLDINFO *world) {
    char path[256];
    world->page_bridge = -1;
    sprintf(path, "%s.bri", world->config_file);
    if (NuFileExists(path)) {
        i32 page = edbriLoadPage(path, world->current_gscn);
        world->page_bridge = page;
    }
}
void LoadPartFile(WORLDINFO *world) {
    char path[256];
    world->page_part = -1;
    edpartSetParticlePage(world->page_pp);

    if ((world->current_level->flags & (LEVEL_OUTRO | LEVEL_MIDTRO | LEVEL_INTRO)) == 0) {
        sprintf(path, "%s.par", world->config_file);
        i32 page = -1;
        if (NuFileExists(path)) {
            page = edpartLoadPage(path, 1, world->current_gscn);
            world->page_part = page;
        }
        world->part_debris_sys = static_cast<PARTDEBSYS_s *>(
            InitPartDebris(&world->giz_buffer, &world->unknown_0108, 0x40, PDEBCOUNT, (char **)PDebNameList, page));
    }
}
#ifdef __EMSCRIPTEN__
i32 ScanTerrId(void *) {
    return 0;
}
#else
void ScanTerrId(void *) {
}
#endif
i32 UnderWater(GameObject_s *object) {
    if ((object->apiobj.field_0x27f & static_cast<u8>(~8u)) != 1) {
        return 0;
    }
    return object->apiobj.field_0x218 >= object->apiobj.pos_y;
}
void ScanTerrain(i32, i32, i32) {
}
void TerrainSkin(PLATSKININFO *, nuvec_s *, float, i32) {
}
void TerrDrawPlat(tertype *, i16) {
}
void TerrDrawSitu(tertype *, terrsitu_s *) {
}
void RotateTerrain(tertype *) {
}
void TerrainImpact(nuvec_s *, nuvec_s *, unsigned char *) {
}
void TerrainPlayer(GameObject_s *) {
}
void MakePlayPlanes(GAMECAMERA_s *) {
}
void TerrDrawPlatCol(tertype *, i16, i32) {
}
void TerrShowCamTerr() {
}
void GetIndGrassClump(i32, i32) {
}
void TerrainSideClamp(nuvec_s *, nuvec_s *) {
}
void TerrainStaticMtx(PLATSKININFO *, nuvec_s *, i32) {
}
void ScanTerrainHandel(i32, i16 *) {
}
#ifdef __EMSCRIPTEN__
i32 TerrShapeSideStep(nuvec_s *, nuvec_s *, unsigned char *) {
    return 0;
}
#else
void TerrShapeSideStep(nuvec_s *, nuvec_s *, unsigned char *) {
}
#endif
void TerrainImpactNorm() {
}
void ScanTerrainPlatform(i32, i32) {
}
void TerrainBlockOnBlock(WORLDINFO_s *, pushblock_s *, nuvec_s *, float *) {
}
void TerrainSkinAllocate(terrsitu_s *) {
}
void ScanTerrIDRemovePlat(i32) {
}
void ScanWallSplineTerrain(i32, i32, i32) {
}
void TerrainImpactPlatform(unsigned char *) {
}
void TerrainMoveImpactData() {
}
#ifdef __EMSCRIPTEN__
i32 TerrainPlatformEmbedded(nuvec_s *) {
    return 0;
}
#else
void TerrainPlatformEmbedded(nuvec_s *) {
}
#endif
void TerrainPlatformMoveCheck(nuvec_s *, nuvec_s *, i32, i32, i32) {
}
void TerrDraw(tertype *, i16) {
}
void TerrFlush() {
}
