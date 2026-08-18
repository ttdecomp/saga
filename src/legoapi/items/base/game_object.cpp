#include "legoapi/world/world_shared.h"

#include "decomp.h"

// Forward declarations for local (static) game-object helper stubs.
struct GameObject_s;
struct nuvec_s;
struct WORLDINFO_s;
struct BOLT_s;
struct debinftype;

void ClearGameObjects(APIOBJECTSYS_s *api_object_sys) {
    (void)api_object_sys;
}

GameObject_s *AddGameObject(i32 id) {
    (void)id;
    UNIMPLEMENTED();
    return NULL;
}
i32 InitCreature(GameObject_s *obj, i32 id, i32 param) {
    (void)obj;
    (void)id;
    (void)param;
    UNIMPLEMENTED();
    return 0;
}

void InitGameObjectLights(void) {
}

// Local (static) game-object behaviour codes and per-object helpers. Stubbed
// as local `t` symbols matching res/libTTapp.so.

static __used__ void ShieldCode(GameObject_s *) {
}

static __used__ void TrenchMove(GameObject_s *) {
}

static __used__ void PooCode(GameObject_s *) {
}

static __used__ void ZapCode(GameObject_s *, i32, i32) {
}

static __used__ void PunchCode(GameObject_s *, i32, i32, i32, i32, f32) {
}

static __used__ void ShootCode(GameObject_s *, i32, i32, i32, i32, i32) {
}

static __used__ void SwipeCode(GameObject_s *, i32, i32) {
}

static __used__ void Punch_HitHold(GameObject_s *, GameObject_s *) {
}

static __used__ i32 Punch_GetDamage_LSW(GameObject_s *, GameObject_s *) {
    return 0;
}

static __used__ void Punch_HitExtraCode_LSW(GameObject_s *, nuvec_s *) {
}

static __used__ void TrenchKilledCallback(GameObject_s *) {
}





static __used__ void SetLayers_BOB(GameObject_s *) {
}

static __used__ void Tag_NoHiddenIcon(GameObject_s *) {
}

static __used__ void Tag_FindGameObject_TRANSFER(GameObject_s *) {
}

static __used__ void TightRope_Attach(GameObject_s *, WORLDINFO_s *) {
}

static __used__ void TightRope_MoveUpdate(GameObject_s *, i32) {
}

static __used__ i32 UsingExtraActions_Game(GameObject_s *) {
    return 0;
}


static __used__ void ZipUp_GetStartPoint(GameObject_s *, nuvec_s *) {
}

static __used__ void SurfaceInfo_ExtraReflect(GameObject_s *) {
}

static __used__ i32 Slam_GetDebris_Game(GameObject_s *, i32) {
    return 0;
}



static __used__ void PauseGame_ExtraCode() {
}

static __used__ void UpdateTotalPtls(debinftype *) {
}


static __used__ i32 SpecialObjectFilter(void *) {
    return 0;
}

static __used__ void KilledTrooperCannon(GameObject_s *) {
}

static __used__ void DeactivatedCode(GameObject_s*) {}

static __used__ void BlockCode(GameObject_s*, int, int, int, int) {}

static __used__ void DodgeCode(GameObject_s*, int, int) {}

static __used__ void FireCode(GameObject_s *, int, int, float, int) {
}
