#include "legoapi/legoapi_types.h"

extern BATARANG_s Batarang[8];

void Batarangs_Draw() {
}

void Batarangs_Reset() {
    for (i32 i = 0; i < 8; ++i) {
        Batarang[i].field_0x7d = 0;
        Batarang[i].active = 0;
        Batarang[i].target_id = 0;
        Batarang[i].cooldown = 50;
    }
}

void Batarang_Release(GameObject_s *, i32) {
}

void Batarangs_Update() {
}

void Batarang_MoveCode(GameObject_s *) {
}

void Batarang_Ricochet(BATARANG_s *) {
}

void Batarang_GetSightInfo(i32, i32 *, i32 *, i32 *, char *) {
}

void Batarang_InitRicochet(BATARANG_s *, nuvec_s *) {
}

void Batarang_SeekToTarget(BATARANG_s *) {
}

void Batarangs_CheckLostData(void *) {
}

void Batarang_StartTargetting(GameObject_s *) {
}

void Batarang_StartThrowQuick(GameObject_s *) {
}

void Batarang_GetObjectFromCharID(i32) {
}

void GetShootDirection_Batman(GameObject_s *, nuvec_s *) {
}
