#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/core/input/qrand.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" i32 NewMode;
extern "C" i32 reset_load;

u16 hub_iconang[4] = {};
static f32 hub_icontime[4] = {};

void NewGameMode() {
    NewMode = 1;
    reset_load = 1;
}

void UpdateStats() {
}

void AddStatusStage(STATUSPACKET_s *, i32, i32) {
}

void SetBonusWinner(i32) {
}

void FindStatusStage(i32) {
}

void NextStatusStage(STATUSPACKET_s *) {
}

void Prompt_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void ResetIconWibble() {
    hub_iconang[0] = static_cast<u16>(qrand());
    hub_icontime[0] = 0.0f;
    hub_iconang[1] = static_cast<u16>(qrand());
    hub_icontime[1] = 0.0f;
    hub_iconang[2] = static_cast<u16>(qrand());
    hub_icontime[2] = 0.0f;
    hub_iconang[3] = static_cast<u16>(qrand());
    hub_icontime[3] = 0.0f;
}

void Save_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void InitStatusScreen(WORLDINFO_s *) {
}

void SetDrawGoldBrick(STATUSPACKET_s *, i32) {
}

void StatusIconsOnOff(float) {
}

void UpdateIconWibble() {
}

void Prompt_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void RedBrick_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void RedBrick_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void StatusPacketReset(STATUSPACKET_s *) {
}

void StatusStage_Reset(STATUS_STAGE_s *) {
}

void TrueHero_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void TrueHero_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void UpdateAchievements(STATUSPACKET_s *) {
}

void UpdateStatusScreen(WORLDINFO_s *) {
}

void RedBrick_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void TrueHero_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void InitStatusScreen_LSW(WORLDINFO_s *, STATUSPACKET_s *) {
}

void RegisterStatusScreen(STATUS_STAGE_s *, i32 *, REGISTERSTATUSPACKET_s *) {
}

void ResetStatusPacket_LSW(STATUSPACKET_s *) {
}

void Status_DrawPromptMenu(STATUSPACKET_s *, i32, float) {
}

void getFinishedStatusAlpha(STATUSPACKET_s *) {
}

void SuperStoryTime_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void SuperStoryTime_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void LSW_registerStatusScreen() {
}

void SuperStoryScore_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void SuperStoryScore_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void SuperStoryTime_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void SuperStoryScore_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void Coins_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}
void Coins_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}
void Coins_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}
void BonusWin_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}
void BonusWin_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}
void BonusTime_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}
void BonusTime_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}
void ChallangeCash_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}
void ChallangeCash_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}
void BonusWin_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}
void BonusTime_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}
void ChallangeCash_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}
void BonusComplete_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}
void BonusComplete_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}
void BonusComplete_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}
