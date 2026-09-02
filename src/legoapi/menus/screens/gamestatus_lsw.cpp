#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "legoapi/core/input/qrand.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

i16 hub_iconang[4];
static f32 hub_icontime[4] = {};

void NewGameMode() {
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
    hub_iconang[0] = (i16)qrand();
    hub_icontime[0] = 0.0f;
    hub_iconang[1] = (i16)qrand();
    hub_icontime[1] = 0.0f;
    hub_iconang[2] = (i16)qrand();
    hub_icontime[2] = 0.0f;
    hub_iconang[3] = (i16)qrand();
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

void StatusPacketReset(STATUSPACKET_s *packet) {
    const STATUSPACKET_LSW_s *lsw_packet = packet->lsw_packet;
    const i32 field_0x04 = packet->field_0x04;
    const i32 field_0x08 = packet->field_0x08;
    void (*reset_callback)(STATUSPACKET_s *) = packet->reset_callback;
    const i32 field_0x10 = packet->field_0x10;
    const f32 field_0x68 = packet->field_0x68;

    reset_callback(packet);
    memset(packet, 0, sizeof(*packet));

    packet->reset_callback = reset_callback;
    packet->lsw_packet = const_cast<STATUSPACKET_LSW_s *>(lsw_packet);
    packet->field_0x04 = field_0x04;
    packet->field_0x08 = field_0x08;
    packet->field_0x10 = field_0x10;
    packet->field_0x68 = field_0x68;
}

void StatusStage_Reset(STATUS_STAGE_s *stage) {
    if (stage != NULL) {
        stage->field_0x18 = 0;
        stage->field_0x1c = 1.0f;
        stage->field_0x14 = -1;
        stage->field_0x12 = 0;
    }
}

void TrueHero_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void TrueHero_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

i32 UpdateAchievements(STATUSPACKET_s *) {
    return 0;
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

void ResetStatusPacket_LSW(STATUSPACKET_s *packet) {
    packet->lsw_packet->field_0x00 = 0;
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
