#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Buck_Start(GameObject_s *, GameObject_s *) {
}

void DoBuckStart(GameObject_s *) {
}

void BlockInBlock(WORLDINFO_s *, pushblock_s *, i32, pushblock_s **) {
}

void Boulder_Kill(PART_s *, i32) {
}

void Boulder_Move(PART_s *, float) {
}

void Buck_MoveCode(GameObject_s *, i32) {
}

void FindNextBreak(unsigned char *, i32) {
}

void FindNearestBreak(unsigned char *, i32) {
}

void BuckStartExtra_LSW(GameObject_s *) {
}

void Buck_StartRiderJump(GameObject_s *, GameObject_s *) {
}

void SetEffectVisibility(char *, i32) {
}

void Conveyor_AdjustSpeed(nuvec_s *) {
}

void AddDevice(nufile_device_s *) {
}

// LevelObjects_InitForLevel @0x475630. Carves LEVELOBJECTCOUNT 0x10-byte
// level-object records from the world's particle bump buffer, zeroed. The
// registry loop that follows (NuSpecialFind per typed object) only runs when
// ObjTabList entries with kind > 6 exist; with an unpopulated registry the
// original returns right after the allocation.
void LevelObjects_InitForLevel(WORLDINFO_s *world) {
    u32 aligned = ((u32)world->giz_buffer.addr + 3) & ~3u;

    world->lev_objs = reinterpret_cast<LEVEL_OBJECT_RUNTIME *>(static_cast<usize>(aligned));
    world->giz_buffer.addr = (usize)(aligned + (u32)LEVELOBJECTCOUNT * 0x10);
    memset((void *)(usize)aligned, 0, (usize)LEVELOBJECTCOUNT * 0x10);
}

void EquivalentObjects_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}
