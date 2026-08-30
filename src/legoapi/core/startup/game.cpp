#include "decomp.h"
#include "globals.h"
#include "gameapi/gui/apimenu.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
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
extern "C" i32 NewMode;

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
}

void PauseGame(i32) {
}
