#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void MiniKits_Init(variptr_u *, variptr_u *) {
}

void CollectMinikit(nuvec_s *, char *, i32) {
}

void AllMiniKitsDone(AREASAVE_s *) {
}

void MiniKitDetector(nuvec_s *) {
}

void CharMiniKit_Draw(i32, numtx_s *, i32, float, float) {
}

void MiniKit_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void MiniKit_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void UpdateNewMiniKits(STATUSPACKET_s *, STATUS_STAGE_s *) {
}

void CollectAllMiniKits(AREASAVE_s *) {
}

void MiniKit_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void MiniKit_GameMsg_End(GAMEMESSAGE_s *) {
}

void ResetMinikitCounter() {
}

void AllMiniKits_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void AllMiniKits_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void SpecialMiniKits_Draw(WORLDINFO_s *) {
}

void AddStatusMiniKitParts() {
}

void SpecialMiniKits_Reset(WORLDINFO_s *) {
}

void AllMiniKits_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void CharacterMiniKits_Dump(WORLDINFO_s *) {
}

void MiniKit_GameMsg_Update(GAMEMESSAGE_s *) {
}

void EffectOffProgress_Reset(LEVEL_PROGRESS_s *) {
}

void IncrementMinikitCounter(GameObject_s *) {
}

void EffectOffProgress_Update(LEVEL_PROGRESS_s *, char *, i32) {
}

void SpecialMiniKits_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}
