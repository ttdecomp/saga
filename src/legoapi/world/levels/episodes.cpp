#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

// Episode-global logic and helpers shared across episodes: completion/
// episode bookkeeping, super-story, boss/cutscene-adjacent helpers. The
// per-episode level *handlers* live in the matching episodeI..VI.cpp files.

// ===========================================================================
// Episode bookkeeping
// ===========================================================================

void BossKilled(i32) {
}

void CountOpenEpisodes() {
}

void Episode_IsComplete(EPISODEDATA *, i32 *) {
}

void Episodes_Completed() {
}

void Episodes_CompleteAllSuperStories() {
}

void Episode_FindFromArea(i32) {
}

void Episode_CountOpenAreas(i32, i32, AREASAVE_s *) {
}

void InitSuperStory(i32) {
}

i32 InStory() {
    return 0;
}

// ===========================================================================
// HUD / score helpers
// ===========================================================================

void CoinTotal_Draw(i32, float, float, i32, float, i32, i32, i32) {
}

void DoubleScoreAlpha() {
}

// ===========================================================================
// Shared gameplay helpers
// ===========================================================================

void TrooperShoot(WORLDINFO_s *, minitrooperteam_s *, minisnowtrooper_s *, u16 *, i32) {
}

void NewTerrStoreAnyInfo() {
}

void SetBobaRocketTarget(MechObjectInterface *) {
}

void FireBountyHunterRocket(GameObject_s *) {
}

void ResetTrooperCannons(WORLDINFO_s *, i32) {
}

void UpdateTrooperCannons(WORLDINFO_s *) {
}

void UpdateMiniSnowTroopers(WORLDINFO_s *) {
}

void SetLevelExBlowupFunc(i32 (*)(GIZMOBLOWUP_s *, i32)) {
}

static __used__ void seed_chase(f32 *, i32, abi_long) {
}
