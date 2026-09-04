#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Batarangs_CheckLostData(void *);
void Detonate(nuvec_s *, u16);
void Detonator_Detonate(DETONATOR_s *);

struct DetonatorHitData {
    u8 field_0x00[0xc];
    f32 field_0x0c;
    u8 field_0x10[0x10];
    f32 field_0x20;
};

void Detonators_Draw() {
}

void Detonators_Reset() {
    memset(Detonator, 0, sizeof(Detonator));
}

void Detonators_Update(void) __attribute__((optimize("O3")));
void Detonators_Update() {
    for (i32 i = 0; i < 10; ++i) {
        DETONATOR_s *detonator = &Detonator[i];
        if (detonator->active == 0) {
            continue;
        }
        detonator->timer += FRAMETIME;
        NuCameraTransformScreenClip(&detonator->field_0x18, &detonator->field_0x0c, 1, NULL);
        if (detonator->timer >= 10.7f) {
            Detonator_Detonate(detonator);
        } else if (detonator->field_0x34 != NULL) {
            DetonatorHitData *hit_data = static_cast<DetonatorHitData *>(detonator->field_0x34);
            if (detonator->timer >= 10.0f || (detonator->object != NULL && detonator->object->apiobj.field_0x287 == 0 &&
                                              detonator->object->field_0xde0 >= 0.3f)) {
                hit_data->field_0x0c = 0.75f;
            } else {
                hit_data->field_0x0c = 0.08f;
            }
        }
    }
}

void Detonator_Detonate(DETONATOR_s *detonator) {
    Batarangs_CheckLostData(detonator);
    Detonate(reinterpret_cast<nuvec_s *>(&detonator->field_0x0c), 0);
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
