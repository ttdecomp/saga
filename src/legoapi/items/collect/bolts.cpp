#include "legoapi/legoapi_types.h"
#include "decomp.h"
struct spacelevel_s;
struct quickboltinfo;

void Bolt_Alloc() {
}

void Bolt_Shoot(GameObject_s *, i32, i32) {
}

void Bolts_Draw(WORLDINFO_s *) {
}

void Bolts_Reset() {
}

void BoltSys_Init(BOLTSYS *) {
}

void Bolt_Reflect(nuvec_s *, nuvec_s *, nuvec_s *) {
}

void Bolts_Update(WORLDINFO_s *) {
}

void Bolt_HitParts(BOLT_s *, nuvec_s *, nuvec_s *, nuvec_s *, float, i32) {
}

void BoltTypes_Reset(WORLDINFO_s *) {
}

void Bolt_Debris_LSW(BOLT_s *, nuvec_s *, i32, nuvec_s *, i32) {
}

void Bolt_PlayHitSfx(BOLT_s *) {
}

void Bolt_HitPartMode(BOLT_s *) {
}

void Bolt_HitPart_LSW(BOLT_s *, PART_s *) {
}

void BoltType_FindByID(i32, WORLDINFO_s *) {
}

void Bolt_HitGameObject(BOLT_s *, GameObject_s *, nuvec_s *, nuvec_s *, nuvec_s *, float, unsigned char *) {
}

void Bolt_HitPlatFn_LSW(BOLT_s *) {
}

void Bolt_HitGameObjects(BOLT_s *, nuvec_s *, nuvec_s *, nuvec_s *, float, unsigned char *) {
}

void Bolt_HitCustomFn_LSW(BOLT_s *, nuvec_s *) {
}

void Bolt_HitGameObjectRC(NetMessage &) {
}

void Bolt_AddDeflectedBolt(BOLT_s *, nuvec_s *, nuvec_s *, unsigned char *) {
}

void Bolt_AlternateFire_LSW(GameObject_s *, i32) {
}

void Bolt_ObjTargetPosYAdjust(GameObject_s *) {
}

void BoltType_FindIDByCreature(GameObject_s *, i32) {
}

void Bolt_Add(GameObject_s *, nuvec_s *, numtx_s *, i32, i32) {
}

void Bolt_End(BOLT_s *, i32) {
}

void Bolt_Find(i32, nuvec_s *, GameObject_s *) {
}

void Bolt_Free(BOLT_s *) {
}

void Bolt_Init(void *, NetMessage &) {
}

static __used__ void UpdateBolt_Geonosian(BOLT_s *) {
}

static __used__ bool Bolt_RayCast(BOLT_s*, nuvec_s*, nuvec_s*, float) { return {}; }

static __used__ void Bolt_Debris_Default(BOLT_s*, nuvec_s*, int, nuvec_s*, int) {}

static __used__ void Bolt_GetShootOrigin_Default(GameObject_s*, nuvec_s*) {}

static __used__ void Bolt_GetShootDirection_Default(GameObject_s*, nuvec_s*) {}

static __used__ unsigned int Batarang_GetTargetPos(BATARANG_s*, int, nuvec_s*) { return {}; }

static __used__ void CollideBoltStarFighter(BOLT_s*, starfighter_s*, _vuv_s*, _vuv_s*) {}

static __used__ void DrawLightningBolts(GameObject_s*, GameObject_s*, int) {}

static __used__ void EndBolt_EwokTorpedo(BOLT_s *) {
}

static __used__ void ProcessSpaceLevel(spacelevel_s *) {
}

static __used__ void ResetSpaceLevel(WORLDINFO_s *, spacelevel_s *) {
}

static __used__ void ProcessStarFighter(starfighter_s *, quickboltinfo *) {
}

static __used__ void StarFighterAlign(starfighter_s *, _vuv_s *, f32, i32) {
}

static __used__ void TrooperTeamSetStateCode(minitrooperteam_s *) {
}

static __used__ unsigned int BoltInitSfx_LSW(GameObject_s*) { return {}; }

void BoltTypes_Init(WORLDINFO_s *world) {
    (void)world;
}

void BoltTypes_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}

extern "C" {

    void HitParts(void) {
    }

} // extern "C"
