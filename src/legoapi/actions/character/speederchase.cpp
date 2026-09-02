#include "decomp.h"
#include "globals.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

static i32 PodRaceKey[8] __attribute__((aligned(16))) = {-1, -1, -1, -1, -1, -1, -1, -1};

u8 troopercannons_beenReset = 0;
i32 players_going_forward = 0;

void getPodRoll(i32) {
}

void ObjOpponent(GameObject_s *, float, float, i32, i32, i32) {
}

void PodKeyReset() {
    for (i32 i = 0; i < 8; i++) {
        PodRaceKey[i] = -1;
    }
}

void PodLoseSpeed(GameObject_s *, i32, i32) {
}

void InitBikeParts() {
}

void SpeederBlowupHack(GIZMOBLOWUP_s *, i32) {
}

void FindPodHoverHeight(GameObject_s *) {
}

void GetVehicleSpeedMul(GameObject_s *, float) {
}

void ObjIsTargetSpeeder(GameObject_s *) {
}

void PodSeekSubCutSound() {
}

void SpeederChaseA_Init(WORLDINFO_s *) {
}

void PodSeekMushCutSound() {
}

void ProcessCurrentSpeed(WORLDINFO_s *, speedup_s *) {
}

void SpeederChaseA_Panel(WORLDINFO_s *) {
}

void SpeederChaseA_Reset(WORLDINFO_s *) {
    troopercannons_beenReset = 0;
    InitBikeParts();
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "SpeedersToKill", NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, "BeenKilled", NULL);
    LevAIMessage[2] = CheckGizAIMessage(gizaimessagesys, "Stage", NULL);
    LevAIMessage[3] = CheckGizAIMessage(gizaimessagesys, "SpeedersKilled", NULL);
    LevAIMessage[4] = CheckGizAIMessage(gizaimessagesys, "KeepPlayersOnSpeeders", NULL);
    LevAIMessage[5] = CheckGizAIMessage(gizaimessagesys, "SpeederMode", NULL);
    players_going_forward = 1;
}

void SpeedersDroppedBack() {
}

void SpeederChaseA_Update(WORLDINFO_s *) {
}

void GetVehicleHoverHeight(GameObject_s *, float *) {
}

void KillParts_SpeederBike(ADDPART_s *, i32, i32, GameObject_s *) {
}

void ObjOpponentStillThere(GameObject_s *, GameObject_s *, float) {
}

void PodSeekTuskanCutSound() {
}

void FindGunshipHoverHeight(GameObject_s *) {
}

void SpeederChaseATATInOutMul(nuvec_s *, nuvec_s *) {
}

f32 GetVehicleAreaRememberSpeed() {
    if (bonusmodearcade == 0) {
        return 0.0f;
    }

    f32 speed = 0.0f;
    f32 player_count = 0.0f;
    if (Player[0] != NULL && (Player[0]->apiobj.field_0x1f8 & 0x80) != 0) {
        speed += Player[0]->field_0xdc8;
        player_count = 1.0f;
    }
    if (Player[1] != NULL && (Player[1]->apiobj.field_0x1f8 & 0x80) != 0) {
        speed += Player[1]->field_0xdc8;
        player_count = 2.0f;
    }
    if (player_count > 1.0f) {
        speed /= player_count;
    }
    if (speed < 0.25f) {
        speed = 0.25f;
    }
    return speed;
}

void SpeederChase_DrawMeleeTargets(i16 *, char *, i32) {
}

void SpeederChase_ObjIsAGroundTroop(GameObject_s *) {
}

extern "C" {

    void cbSetAutoSpeed(void) {
    }

} // extern "C"
