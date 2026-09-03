#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Batarangs_CheckLostData(void *);
void Detonate(nuvec_s *, u16);

struct DetonatorHitData {
    u8 field_0x00[0x20];
    f32 field_0x20;
};

void Detonators_Draw() {
}

void Detonators_Reset() {
    memset(Detonator, 0, sizeof(Detonator));
}

void Detonators_Update() {
}

void Detonator_Detonate(DETONATOR_s *detonator) {
    Batarangs_CheckLostData(detonator);
    Detonate(reinterpret_cast<nuvec_s *>(&detonator->action_data), 0);
    DetonatorHitData *hit_data = static_cast<DetonatorHitData *>(detonator->field_0x34);
    detonator->active = 0;
    if (hit_data != NULL) {
        hit_data->field_0x20 = 2.0f;
        detonator->field_0x34 = NULL;
    }
}

void Detonator_MoveCode(GameObject_s *) {
}

void Detonator_FindNearest(nuvec_s *, float, GameObject_s *) {
}
