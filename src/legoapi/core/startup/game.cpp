#include "decomp.h"
#include "globals.h"

#include <string.h>

#include "gameapi/gui/apimenu.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/levels/episode.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern f32 GetVehicleAreaRememberSpeed(void);
extern void AveragePlayerCurrentSpeedMul(void);
extern void SetPlayer(void);
extern void ResetPlayer(GameObject_s *, i32, nuvec_s *, i32);
extern void GameFog_Reset(void);
extern void ConfigureComplexShadow(GameObject_s *);
extern void ResetAdaptiveDifficulty(void);
extern void Cheats_TurnOff(i32);
extern void Hint_ClearHintsAndDoneFlags(void);
extern void GamePad_InitButtons(void);
extern void Customiser_CopyDefaultPiecesToSave(CUSTOMISER *, CUSTOMISESAVE *);
extern void FinishWeirdoNames(i32);
extern void Store_UnlockPack(i32, bool);
extern void ReCalculateCompletionPoints(void);
extern i16 id_DEFAULTCHARACTER[2];
extern volatile u8 LSW_HintConditions[4];
extern "C" i32 NewMode;

namespace {

    constexpr f32 kSuperStoryTimeLimit = 3600.0f;
    constexpr i32 kSuperStoryScoreTarget = 100000;
    constexpr i32 kPrimaryCustomNameTextId = 0xcc;
    constexpr i32 kSecondaryCustomNameTextId = 0xcd;

} // namespace

struct AREAGAMEMODESTATE {
    i32 field_0x00;
    i32 field_0x04;
    i32 field_0x08;
    i32 field_0x0c;
    i32 field_0x10;
    i32 field_0x14;
    i32 field_0x18;
    i32 field_0x1c;
    i32 field_0x20;
    i32 field_0x24;
    i32 field_0x28;
    i32 field_0x2c;
    i32 field_0x30;
};

DECOMP_ASSERT(sizeof(AREAGAMEMODESTATE) == 0x34, "AREAGAMEMODESTATE size");

void ClearPause() {
}

void ResumeGame(i32, i32) {
}

void InitGameMode() {
    if ((WORLD->current_level->flags & LEVEL_GAMEPLAY) != 0) {
        MenuReset();
    }

    for (GameObject_s *player : Player) {
        if (player != NULL && (player->field_0xeff & 1) == 0) {
            ResetPlayer(player, 1, NULL, 1);
        }
    }

    f32 remember_speed = GetVehicleAreaRememberSpeed();
    if (VehicleAreaRememberSpeed < remember_speed) {
        VehicleAreaRememberSpeed = remember_speed;
    }
    AveragePlayerCurrentSpeedMul();
    SetPlayer();

    AREAGAMEMODESTATE *area_state = reinterpret_cast<AREAGAMEMODESTATE *>(AreaGlobals);
    area_state->field_0x14 = area_state->field_0x0c;
    area_state->field_0x20 = area_state->field_0x1c;
    NewLData = NULL;
    NewMode = 0;

    for (i32 i = 0; i < HIGHGAMEOBJECT; i++) {
        GameObject_s *object = &Obj[i];
        if ((object->apiobj.field_0x1f8 & 0x1001) == 0x1001) {
            object->field_0xefc |= 0x80;
        }
    }

    GameFog_Reset();
    ConfigureComplexShadow(NULL);
}

void IncreaseScore(u32 *, u64, i32) {
}

void RegisterHelpers() {
}

void NewGame() {
    TempOptions = Game.options_save;
    memset(&Game, 0, sizeof(Game));
    Game.options_save = TempOptions;

    MenuLoadOccurred = 0;
    MenuSaveOccurred = 0;
    ResetAdaptiveDifficulty();
    Game.save_version = 5;

    if (EDataList != NULL) {
        Game.area_save[EDataList[0].area_ids[0]].complete = 1;
    } else {
        Game.area_save[0].complete = 1;
    }

    if (SENATE_ADATA != NULL) {
        Game.area_save[SENATE_ADATA->index].complete = 1;
    }
    if (UTAPAU_ADATA != NULL) {
        Game.area_save[UTAPAU_ADATA->index].complete = 1;
    }
    if (HOTH_ADATA != NULL) {
        Game.area_save[HOTH_ADATA->index].complete = 1;
    }
    if (BONUSDAGOBAH_ADATA != NULL) {
        Game.area_save[BONUSDAGOBAH_ADATA->index].complete = 1;
    }
    if (BONUSKAMINO_ADATA != NULL) {
        Game.area_save[BONUSKAMINO_ADATA->index].complete = 1;
    }
    if (BONUSKASHYYYK_ADATA != NULL) {
        Game.area_save[BONUSKASHYYYK_ADATA->index].complete = 1;
    }

    Cheats_TurnOff(0);
    Hint_ClearHintsAndDoneFlags();
    LSW_HintConditions[0] &= static_cast<u8>(~7);
    GamePad_InitButtons();
    Tag_DoneFirst = 0;
    Tag_DoneAny = 0;

    for (i32 i = 0; i < AREACOUNT; ++i) {
        Game.area_save[i].challenge_trial_time = static_cast<f32>(ADataList[i].challenge_trial_time);
    }

    Game.episode_save[0].superstory_time_limit = kSuperStoryTimeLimit;
    Game.episode_save[1].superstory_time_limit = kSuperStoryTimeLimit;
    Game.episode_save[2].superstory_time_limit = kSuperStoryTimeLimit;
    Game.episode_save[3].superstory_time_limit = kSuperStoryTimeLimit;
    Game.episode_save[4].superstory_time_limit = kSuperStoryTimeLimit;
    Game.episode_save[5].superstory_time_limit = kSuperStoryTimeLimit;
    Game.episode_save[0].superstory_score_target = kSuperStoryScoreTarget;
    Game.episode_save[1].superstory_score_target = kSuperStoryScoreTarget;
    Game.episode_save[2].superstory_score_target = kSuperStoryScoreTarget;
    Game.episode_save[3].superstory_score_target = kSuperStoryScoreTarget;
    Game.episode_save[4].superstory_score_target = kSuperStoryScoreTarget;
    Game.episode_save[5].superstory_score_target = kSuperStoryScoreTarget;

    Customiser_CopyDefaultPiecesToSave(CharacterCustomiser, &Game.customizer);
    if (TTab != NULL) {
        if (TTab[kPrimaryCustomNameTextId] != NULL) {
            NuStrCpy(Game.customizer.primary_name, TTab[kPrimaryCustomNameTextId]);
        }
        if (TTab[kSecondaryCustomNameTextId] != NULL) {
            NuStrCpy(Game.customizer.secondary_name, TTab[kSecondaryCustomNameTextId]);
        }
        FinishWeirdoNames(-1);
    }

    if (id_DEFAULTCHARACTER[0] != -1) {
        Game.character_save[id_DEFAULTCHARACTER[0]] |= 3;
    }
    if (id_DEFAULTCHARACTER[1] != -1) {
        Game.character_save[id_DEFAULTCHARACTER[1]] |= 3;
    }
    if (id_WEIRDO1 != -1) {
        Game.character_save[id_WEIRDO1] |= 3;
    }
    if (id_WEIRDO1 != -1 && CDataList != NULL) {
        CDataList[id_WEIRDO1].field0_0x0 = kPrimaryCustomNameTextId;
    }
    if (id_WEIRDO2 != -1) {
        Game.character_save[id_WEIRDO2] |= 3;
    }
    if (id_WEIRDO2 != -1 && CDataList != NULL) {
        CDataList[id_WEIRDO2].field0_0x0 = kSecondaryCustomNameTextId;
    }

    Game.customizer.primary_name_unlocked = 1;
    Game.customizer.secondary_name_unlocked = 1;

    u8 previous_group = 0;
    for (i32 i = 0; i < 10; ++i) {
        if (Suit[i].group != previous_group) {
            Game.initial_store_pack_flags |= 1u << i;
        }
        previous_group = Suit[i].group;
    }

    for (i32 i = 0; i < 11; ++i) {
        if ((static_cast<u16>(SuperOptions.field0_0x0) & (1u << i)) != 0) {
            Store_UnlockPack(i, true);
        }
    }

    ReCalculateCompletionPoints();
}

void PauseGame(i32) {
}
