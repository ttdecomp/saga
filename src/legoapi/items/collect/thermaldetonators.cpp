#include "decomp.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void AddPartDebris(PARTDEBSYS_s *, i32, nuvec_s *);
void AddExplosion(nuvec_s *, f32, f32, GameObject_s *, i32, i32);
void NewRumbleAllPlayers(f32, f32, i32, i32);
void GameCam_NewShake(GAMECAMERA_s *, f32, f32, f32);
void GameCam_Judder(GAMECAMERA_s *, f32, i32, nuvec_s *);
i32 qrand();

void ThermalDetonator_Throw(GameObject_s *) {
}

void PartDraw_ThermalDetonator(PART_s *) {
}

void PartKill_ThermalDetonator(PART_s *, i32) {
}

void ThermalDetonator_MoveCode(GameObject_s *) {
}

void ThermalDetonator_ThrowMom(GameObject_s *, nuvec_s *) {
}

void PartImpact_ThermalDetonator(PART_s *) {
}

void PartUpdate_ThermalDetonator(PART_s *) {
}

void Detonate(nuvec_s *position, u16 flags) {
    AddGameDebris(WORLD->debris_sys, 0x49, position);
    AddGameDebris(WORLD->debris_sys, 0x4a, position);
    AddGameDebris(WORLD->debris_sys, 0x4b, position);
    AddPartDebris(WORLD->part_debris_sys, 2, position);
    NewRumbleAllPlayers(1.0f, 0.1f, 0, 0);
    PlaySfx((char *)"exp_thermalDet", position);
    f32 amount_a;
    f32 amount_b;
    if ((flags & 0x1000) != 0) {
        f32 amount = qrand() < 0x8000 ? -2.0f : 2.0f;
        GameCam_Judder(GameCam, amount, 2, position);
        GameCam_NewShake(GameCam, 2.0f, 1.0f, 1.0f);
        PlaySfx((char *)"exp_thermalDet", position);
        amount_a = 1.3125f;
        amount_b = 0.875f;
    } else {
        GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
        amount_a = 0.75f;
        amount_b = 0.5f;
    }
    AddExplosion(position, amount_a, amount_b, NULL, -1, (flags & 0xffff) | 0x67);
}
