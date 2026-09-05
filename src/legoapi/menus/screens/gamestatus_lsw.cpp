#include <string.h>

#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/nutrig.h"

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
extern STATUSPACKET_s StatusPacket;

i32 GetMenuID();
extern "C" i32 MenuInMemoryCard();
void ResetRumble(RUMBLEPACKET *packet);
void ReCalculateCompletionPoints();
void StatusStage_Reset(STATUS_STAGE_s *stage);
void GameCam_Blend(GAMECAMERA_s *camera, f32 duration, f32 curve, i32 mode);

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
STATUS_STAGE_s *StatusStages;
f32 iconalphaoverride;
f32 icon_y;
i32 draw_player_icons;
i32 status_prompt;
static i32 DrawGoldBrick_Stage;
static i32 DrawGoldBrick_Phase;

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

void AddStatusStage(STATUSPACKET_s *packet, i32 type, i32 gold_brick_enabled) {
    const u8 index = packet->stage_count;
    packet->stage_types[index] = static_cast<i8>(type);
    packet->gold_brick_enabled[index] = static_cast<u8>(gold_brick_enabled);
    packet->stage_count = index + 1;
}

void SetBonusWinner(i32 player) {
    BonusWinner = player;
    GameCam_Blend(GameCam, 1.5f, 0.0f, 1);
    LookAtBoth = 1;
}

STATUS_STAGE_s *FindStatusStage(i32 type) {
    for (STATUS_STAGE_s *stage = StatusStages; stage->type != -1; ++stage) {
        if (stage->type == type) {
            return stage;
        }
    }
    return NULL;
}

void NextStatusStage(STATUSPACKET_s *packet) {
    STATUS_STAGE_s *stage = packet->stage;
    if (stage != NULL) {
        if (stage->field_0x14 == -1 && packet->field_0x68 > stage->field_0x18) {
            packet->field_0xb0 |= 2;
            return;
        }
        stage->field_0x12 = 1;
    }

    packet->previous_stage_2 = packet->previous_stage;
    packet->previous_stage = stage;
    ++packet->current_gold_brick;
    packet->stage = packet->next_stage;
    packet->next_stage = FindStatusStage(packet->stage_types[packet->current_gold_brick + 1]);

    while (packet->stage == NULL || packet->next_stage == NULL) {
        if (packet->stage == NULL && packet->next_stage != NULL) {
            packet->stage = packet->next_stage;
            packet->next_stage = NULL;
        }

        ++packet->current_gold_brick;
        if (packet->current_gold_brick + 1 < packet->stage_count) {
            packet->next_stage = FindStatusStage(packet->stage_types[packet->current_gold_brick + 1]);
        } else {
            packet->next_stage = FindStatusStage(10);
        }
    }

    StatusStage_Reset(packet->stage);
    packet->field_0xb0 &= ~2;
    status_prompt = 0;

    if (packet->stage->type == 10 || packet->stage->type == 11) {
        ResetRumble(reinterpret_cast<RUMBLEPACKET *>(&packet->player0_rumble_amount));
        ResetRumble(reinterpret_cast<RUMBLEPACKET *>(&packet->player1_rumble_amount));
    }
    if (packet->stage->type == 11) {
        ReCalculateCompletionPoints();
    }
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

void SetDrawGoldBrick(STATUSPACKET_s *packet, i32) {
    const i32 stage = packet->current_gold_brick;
    if (packet->gold_brick_enabled[stage] != 0) {
        DrawGoldBrick_Stage = stage;
        DrawGoldBrick_Phase = packet->stage->field_0x14;
    }
}

void NewStatusRumbleBuzz(i32 player, float amount, float buzz, i32 priority) {
    if (!(amount > 0.0f)) {
        if (StatusPacket.player0_active != 0 && (player == 0 || player == -1)) {
            goto player0_buzz;
        }
        if (StatusPacket.player1_active != 0 && (player == 1 || player == -1)) {
            goto player1_buzz;
        }
        return;
    }

    if (StatusPacket.player0_active != 0 && (player == 0 || player == -1)) {
        if (StatusPacket.player0_rumble_time <= 0.0f || amount > StatusPacket.player0_rumble_time /
                                                                     StatusPacket.player0_rumble_duration *
                                                                     StatusPacket.player0_rumble_amount) {
            StatusPacket.player0_rumble_amount = amount;
            StatusPacket.player0_rumble_duration = amount;
            StatusPacket.player0_rumble_time = amount;
        }
    player0_buzz:
        if (buzz > StatusPacket.player0_buzz_amount) {
            StatusPacket.player0_buzz_amount = buzz;
        }
        if (priority > 0) {
            const i32 old_priority = StatusPacket.player0_rumble_priority;
            ++priority;
            if (priority > old_priority) {
                StatusPacket.player0_rumble_priority = static_cast<u8>(priority);
            }
        }
    }

    if (StatusPacket.player1_active != 0 && (player == 1 || player == -1)) {
        if (StatusPacket.player1_rumble_time <= 0.0f || amount > StatusPacket.player1_rumble_time /
                                                                     StatusPacket.player1_rumble_duration *
                                                                     StatusPacket.player1_rumble_amount) {
            StatusPacket.player1_rumble_amount = amount;
            StatusPacket.player1_rumble_duration = amount;
            StatusPacket.player1_rumble_time = amount;
        }
    player1_buzz:
        if (buzz > StatusPacket.player1_buzz_amount) {
            StatusPacket.player1_buzz_amount = buzz;
        }
        if (priority > 0) {
            const i32 old_priority = StatusPacket.player1_rumble_priority;
            ++priority;
            if (priority > old_priority) {
                StatusPacket.player1_rumble_priority = static_cast<u8>(priority);
            }
        }
    }
}

f32 StatusIconsOnOff(f32 progress) {
    return NuTrigTable[(static_cast<i32>(progress * 16384.0f) >> 1) & 0x7fff] * (STATSPOSY - STATSPOS2Y) + STATSPOS2Y;
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
    void (*draw_background_callback)(STATUSPACKET_s *) = packet->draw_background_callback;
    const f32 field_0x68 = packet->field_0x68;

    reset_callback(packet);
    memset(packet, 0, sizeof(*packet));

    packet->reset_callback = reset_callback;
    packet->lsw_packet = const_cast<STATUSPACKET_LSW_s *>(lsw_packet);
    packet->field_0x04 = field_0x04;
    packet->field_0x08 = field_0x08;
    packet->draw_background_callback = draw_background_callback;
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

f32 getFinishedStatusAlpha(STATUSPACKET_s *packet) {
    STATUS_STAGE_s *stage = packet->stage;
    f32 alpha = 1.0f;
    if (stage->type == 11 || stage->type == 12) {
        alpha = 0.0f;
    } else if (stage->type == 10) {
        alpha = stage->field_0x18 < 1.0f ? 1.0f - stage->field_0x18 : 0.0f;
    } else if (stage->type == 19 && stage->field_0x14 != 0) {
        if (stage->field_0x18 < 1.0f) {
            alpha = 1.0f - stage->field_0x18;
        } else {
            const f32 fade_start = stage->field_0x1c - 1.0f;
            alpha = stage->field_0x18 < fade_start
                        ? 0.0f
                        : (stage->field_0x18 - fade_start) / (stage->field_0x1c - fade_start);
        }
    }
    return alpha;
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
