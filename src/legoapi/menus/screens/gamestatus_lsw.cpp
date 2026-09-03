#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" i32 NewMode;
extern "C" i32 reset_load;
extern "C" i32 Paused;
extern FadeSystem FadeSys;
extern f32 statstime;
extern f32 cointotaltime;
extern i32 screendump;
extern i32 newgamecam;

i32 GetMenuID();
extern "C" i32 MenuInMemoryCard();

namespace {
    enum PANEL_BLOCKING_MENU {
        PANEL_MENU_EPISODE_I = 20,
        PANEL_MENU_EPISODE_II = 21,
        PANEL_MENU_EPISODE_III = 22,
        PANEL_MENU_EPISODE_IV = 23,
        PANEL_MENU_SAVE = 25,
        PANEL_MENU_LOAD = 26,
    };

    bool CoinTotalCanOpen() {
        if (FadeSys.fade != 0.0f || CUTSTOPGAME != 0) {
            return false;
        }
        if (Paused == 0 && NetPaused == 0 && DrawCoinTotalTime <= 0.0f) {
            return false;
        }
        if (screendump != 0 || MenuInMemoryCard() != 0) {
            return false;
        }

        const i32 menu = GetMenuID();
        return menu != PANEL_MENU_EPISODE_I && menu != PANEL_MENU_EPISODE_II && menu != PANEL_MENU_EPISODE_III &&
               menu != PANEL_MENU_EPISODE_IV && menu != PANEL_MENU_SAVE && menu != PANEL_MENU_LOAD;
    }
} // namespace

u16 hub_iconang[4] = {};
static f32 hub_icontime[4] = {};

void NewGameMode() {
    NewMode = 1;
    reset_load = 1;
}

void UpdateStats() {
    LEVELDATA *level = WORLD->current_level;
    if ((level->flags & LEVEL_GAMEPLAY) == 0) {
        return;
    }

    f32 stats_target = 0.0f;
    if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 && newgamecam == 0) {
        const bool hub_camera_hidden = HUB_ADATA != NULL && WORLD->area == HUB_ADATA && GameCam->mode == 4;
        const i32 menu = GetMenuID();
        if (!hub_camera_hidden && (menu < PANEL_MENU_EPISODE_I || menu > PANEL_MENU_EPISODE_IV)) {
            stats_target = 1.0f;
        }
    }
    statstime = SeekLinearF(statstime, stats_target, FRAMETIME);

    if ((level->flags & LEVEL_SHOW_COIN_TOTAL) != 0) {
        DrawCoinTotalTime = 1.0f;
    }
    if (DrawCoinTotalTime > 0.0f) {
        DrawCoinTotalTime -= FRAMETIME;
    }

    const f32 coin_total_target = CoinTotalCanOpen() ? 1.0f : 0.0f;
    cointotaltime = SeekLinearF(cointotaltime, coin_total_target, FRAMETIME);
    CoinTotalScale = SeekLinearF(CoinTotalScale, 1.0f, 3.0f * FRAMETIME);
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
