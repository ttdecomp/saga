#include "decomp.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/startup/game.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern void RestoreOptions(void);
extern void *CutStopInfo;
extern "C" {
    extern i32 Paused;
    extern i32 NewMode;
    extern i32 CutSceneWaiting;
    extern i32 editor_active;
    extern i32 (*GamePads_IgnoreInputFn)(void);
    extern GAMEPAD_s GamePad[64];
    extern FadeSystem FadeSys;
}

void ClearStill() {
}

void CircleLevel(LEVELDATA_s *) {
}

void CurrentStart(GameObject_s *, i32, i32) {
}

void DoSomeChecks(GameObject_s &, AIPATH_s &, AIPATHCNX_s &, i32) {
}

void CDataFromName(char *) {
}

float CalcValue1648(char *data, i32 quarter, i32 stride, float fraction, ani3_scalemin_s *scale_min) {
    u16 *next = reinterpret_cast<u16 *>(data + stride);
    u16 *keys = reinterpret_cast<u16 *>(data);
    float value;

    if (quarter == 0) {
        float tangent =
            static_cast<float>((keys[2] & 0xfff) - (keys[1] & 0xfff)) * fraction + static_cast<float>(keys[1] & 0xfff);
        value = (static_cast<float>(static_cast<i32>(*next) - static_cast<i32>(keys[0])) * tangent / 4095.0f +
                 static_cast<float>(keys[0]));
    } else if (quarter == 1) {
        float tangent =
            static_cast<float>((keys[3] & 0xfff) - (keys[2] & 0xfff)) * fraction + static_cast<float>(keys[2] & 0xfff);
        value = (static_cast<float>(static_cast<i32>(*next) - static_cast<i32>(keys[0])) * tangent / 4095.0f +
                 static_cast<float>(keys[0]));
    } else if (quarter == 2) {
        u32 start = keys[3] & 0xfff;
        u32 end = ((keys[2] & 0xf000) >> 8) | ((keys[3] & 0xf000) >> 4) | (keys[1] >> 12);
        float tangent =
            static_cast<float>(static_cast<i32>(end) - static_cast<i32>(start)) * fraction + static_cast<float>(start);
        value = (static_cast<float>(static_cast<i32>(*next) - static_cast<i32>(keys[0])) * tangent / 4095.0f +
                 static_cast<float>(keys[0]));
    } else if (quarter == 3) {
        u32 next_value = *next;
        u32 tangent = ((keys[2] & 0xf000) >> 8) | ((keys[3] & 0xf000) >> 4) | (keys[1] >> 12);
        float a = static_cast<float>(static_cast<i32>(next_value) - static_cast<i32>(keys[0])) *
                      static_cast<float>(tangent) / 4095.0f +
                  static_cast<float>(keys[0]);
        float b =
            static_cast<float>(static_cast<i32>(reinterpret_cast<u16 *>(data)[stride]) - static_cast<i32>(next_value)) *
                static_cast<float>(next[1] & 0xfff) / 4095.0f +
            static_cast<float>(next_value);
        value = (b - a) * fraction + a;
    } else {
        value = 0.0f;
    }

    return value * scale_min->scale + scale_min->minimum;
}

void ConstantRumble(GameObject_s *, float, float) {
}

extern i32 AllMiniKitsDone(AREASAVE_s *save);

COLLECTID *CollectIDUnlocked(i32 id) {
    i32 index = InCollectList_Index(id, CollectList, CollectCount);
    if (index == -1) {
        return NULL;
    }

    COLLECTID *entry = &CollectList[index];
    if (Game_CharacterSave != NULL && (Game_CharacterSave[id] & 2) != 0) {
        return entry;
    }

    switch (entry->type) {
        case 0:
            return entry;
        case 2:
            if (static_cast<i8>(entry->field2_0x3) == -1 || Game_AreaSave == NULL) {
                return NULL;
            }
            return Game_AreaSave[entry->field2_0x3].area_complete != 0 ? entry : NULL;
        case 3:
            if (Episodes_Completed() != EPISODECOUNT) {
                return NULL;
            }
            return Game_100PercentComplete() != 0 ? entry : NULL;
        case 4:
            return AllMiniKitsDone(Game_AreaSave) != 0 ? entry : NULL;
        case 6:
            if (StatusCollectList.ptr == NULL || StatusCollectList.ptr->gold_bricks < entry->field6_0xa) {
                return NULL;
            }
            return entry;
        case 7:
            return Game_100PercentComplete() != 0 ? entry : NULL;
        case 8:
            return Store_IsPackUnlocked(static_cast<i8>(entry->field2_0x3)) != 0 ? entry : NULL;
        default:
            return NULL;
    }
}

void ClearLastSafeTakeOver(GameObject_s *) {
}

void ClearTakeOverObjectSys() {
}

void CalcValue1648Get2Values(char *, i32, i32, ani3_scalemin_s *, float *, float *) {
}

void GetNativeTextureFormatName(NUTEXFORMAT) {
}

void CatIToX(char *, i32) {
}

void DoInput(WORLDINFO_s *world) {
    if (world == NULL) {
        world = WorldInfo_CurrentlyActive();
    }

    const i32 player_0_input = ReadPad(0);
    const i32 player_1_input = ReadPad(1);
    const i32 player_state_changed = PlayersDropInOut();

    for (i32 player_index = 0; player_index < 2; ++player_index) {
        const i32 input_result = player_index == 0 ? player_0_input : player_1_input;
        if (GamePads_IgnoreInputFn != NULL && GamePads_IgnoreInputFn() != 0) {
            continue;
        }
        if (input_result <= 1 || player_state_changed != 0) {
            continue;
        }

        GameObject_s *player = Player[player_index];
        if (player == NULL || static_cast<i8>(player->apiobj.field_0x1f8) >= 0 ||
            (LEGOCONTEXT_DROPIN != -1 && static_cast<i8>(player->field_0x7a5) == LEGOCONTEXT_DROPIN) ||
            (GamePad[player_index].buttons_pressed & GAMEPAD_START) == 0) {
            continue;
        }
        if (NewMode != 0 || NewLData != NULL || FadeSys.fade != 0.0f || editor_active != 0 ||
            GameTimer.time_elapsed <= 0.0f || world == NULL || world->current_level == NULL ||
            world->current_level == TITLES_LDATA) {
            continue;
        }

        const bool player_can_resume = pause_i_pad == -1 || pause_i_pad == player_index;
        if (Paused != 0 || (GameMenu[GameMenuLevel].menu != -1 && NetPaused != 0)) {
            if (player_can_resume) {
                ResumeGame(1, 1);
                RestoreOptions();
            }
            continue;
        }
        if (GameMenu[GameMenuLevel].menu != -1 || CutSceneWaiting != 0 || MiniCutCam != 0 ||
            memcard_autosavestarted != 0 || memcard_autosavepostdelay > 0.0f || memcard_autosavepredelay > 0.0f ||
            GameTimer.update_count == 0) {
            continue;
        }
        if (CUTSTOPGAME != 0 && !CutScene_IsSkippable(static_cast<CUTINFO *>(CutStopInfo))) {
            continue;
        }

        PauseGame(static_cast<i32>(player->pad_gamepad - GamePad));
    }
}

void CatI64ToX(char *, i64) {
}

void DieRumble(GameObject_s *) {
}

void charToInt(char const *) {
}

static __used__ i32 _fseek64_wrap(__sFILE *, i64, i32) {
    return 0;
}
