#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void ResetLevel(WORLDINFO_s *, char *, i32) {
}

void ClearLevData() {
}

void SetLevelHack(i32) {
}

void Areas_OpenAll(i32) {
}

void Area_Configure(i32, i32, EXTRAMODEL *, i16 *) {
}

void ClearUpAreaData() {
}

i32 GetTableLocator(void) {
    return 0;
}

void getSpawnLocator(float, char *) {
}

void KillBossNewLevel(i32, i32, float, i32) {
}

void NewLevelFromMenu(LEVELDATA_s *, i32, i32, i32) {
}

void ClearAreaProgress(i32, i32) {
}

i32 GetCounterLocator(i32) {
    return 0;
}

void LevObj_FindByPlatID(WORLDINFO_s *, i32) {
}

void KillBossPlayCutScene(i32, i32, float, char *) {
}

void KillBossCompleteLevel(i32, i32, float) {
}

void LevelStreaming_Update(WORLDINFO_s *) {
}

void Areas_ConfigureResidents(variptr_u *, variptr_u *) {
}

void LevelObjects_InitForGame(LEVELOBJECT *, variptr_u *, variptr_u *, i32, i32) {
}

void Areas_CompleteAllBuildUps(AREASAVE_s *) {
}

void LevelStreaming_DoorOverride(WORLDINFO_s *, LEVELDATA_s *, float, float *) {
}

void LevelProgress_ReserveBufferSpace(variptr_u *, variptr_u) {
}

void NewArea() {
}

void OffPlat(i32) {
}

void KillBoss(i32, i32, float) {
}

void LevObj_FixUpPlatIDs(WORLDINFO_s *world) {
    (void)world;
}
