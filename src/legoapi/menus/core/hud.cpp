#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/render/core/render.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nutrig.h"

struct spacelevel_s;

extern FadeSystem FadeSys;
extern f32 statstime;
extern f32 cointotaltime;

void CoinTotal_Draw(i32 total, f32 y, f32 scale, i32 remember_positions, f32 icon_phase, i32 red, i32 green, i32 blue);
void DrawSuperStoryTime(f32 x, f32 timer, f32 target, i32 flags, i32 draw_icon);
void Text_MakeScore(u32 score, char *text);

HudRadarPulse::HudRadarPulse(VuVec const &) {
}

void HudRadarPulse::IsFinished() {
}

void HudRadarPulse::Process(float) {
}

void HudRadarPulse::Render() {
}

static __used__ void RefreshUI() {
}

enum COIN_TOTAL_SOURCE {
    COIN_TOTAL_SAVED_GAME = 0,
    COIN_TOTAL_SUPER_STORY = 1,
    COIN_TOTAL_BONUS = 2,
};

static __used__ void DrawCoinTotal(i32 source, i32 hide_super_story_target) {
    if (FadeSys.fade != 0.0f || (WORLD->current_level->flags & LEVEL_GAMEPLAY) == 0) {
        return;
    }

    const f32 timer = source == COIN_TOTAL_BONUS ? statstime : cointotaltime;
    const i32 angle = static_cast<i32>(timer * static_cast<f32>(NUANG_90DEG));
    const f32 y = NuTrigTable[(angle >> 1) & 0x7fff] * (STATSPOSY - STATSPOS2Y) + STATSPOS2Y + COINTOTAL_SCOREDY;

    i32 total;
    i32 red = 255;
    i32 green = 191;
    i32 blue = 0;

    if (source == COIN_TOTAL_SUPER_STORY) {
        const EPISODESAVE_s &episode = Game.episode_save[SuperStoryEpisode];
        DrawSuperStoryTime(-y, SuperStoryTimer[0], episode.superstory_time_limit, 0, 1);
        total = static_cast<i32>(SuperStoryScore);

        if (episode.superstory_score_target != 0) {
            if (hide_super_story_target == 0) {
                char target[64];
                char text[64];
                Text_MakeScore(static_cast<u32>(episode.superstory_score_target), target);
                NuStrCpy(text, const_cast<char *>("("));
                NuStrCat(text, target);
                NuStrCat(text, const_cast<char *>(")"));
                Text3DEx(text, 0.0f, y - 0.1f, 1.0f, 0.35f, 0.35f, 0.35f, 0, 255, 255, 255, 48);
            }
            if (SuperStoryScore < static_cast<u32>(episode.superstory_score_target)) {
                red = 63;
                green = 255;
                blue = 31;
            }
        }
    } else if (source == COIN_TOTAL_BONUS) {
        total = BonusCoinTotal;
    } else {
        total = static_cast<i32>(Game.coins);
    }

    CoinTotal_Draw(total, y, CoinTotalScale, 1, 1.0f, red, green, blue);
}

static __used__ void DrawPackButton(GAMEMESSAGE_s *, nuvec_s *, float) {
}

static __used__ void DrawSpaceLevel(spacelevel_s *) {
}

static __used__ void DrawEpisodesMenu(int, float) {
}

static __used__ void InitUI() {
}

static __used__ i32 SubItemMenu(MENU_s *) {
    return 0;
}

namespace {
    void _NuTimeBarSlotBegin(void) {
    }

    void _NuTimeBarSlotEnd(void) {
    }
} // namespace
