#include "decomp.h"
#include "legoapi_types.h"

// Static character/global-character and gameplay helpers. Stubbed to satisfy
// the symbol baseline.

static __used__ char *GlobalCharacterName(int) {
    return nullptr;
}

static __used__ void *GlobalCharacterHGobj(int) {
    return nullptr;
}

static __used__ void GlobalCharacterRender(nuvec_s *, i16, int, int, EDCREATURE_s *) {
}

static __used__ int GlobalCharacterTypeID(char *) {
    return 0;
}

static __used__ float GetViewRange(int) {
    return 0;
}

static __used__ float GetHearDistance(int) {
    return 0;
}

static __used__ float GetMaxViewHeight(int) {
    return 0;
}

static __used__ float GetMinViewHeight(int) {
    return 0;
}

static __used__ bool IsWearingBackPack_Game(GameObject_s *) {
    return false;
}

static __used__ void ExtraDieSfx_LSW(GameObject_s *) {
}

static __used__ void ExtraHurtSfx_LSW(GameObject_s *) {
}

static __used__ int GameAudio_CheckReverb_LSW() {
    return 0;
}

static __used__ int GameAudio_OverrideFootStep_LSW(GameObject_s *, int) {
    return 0;
}

static __used__ float GetCharacterGoalSpeed(APIOBJECT_s *) {
    return 0;
}

static __used__ bool IsAFallAnim(int) {
    return false;
}

static __used__ int IsGrabbable(GameObject_s *) {
    return 0;
}

static __used__ void ForcePushed_MoveCode(GameObject_s *) {
}

static __used__ int Fighting_WeaponInAction_Game(GameObject_s *) {
    return 0;
}

static __used__ int Fighting_WeaponOutAction_Game(GameObject_s *) {
    return 0;
}

static __used__ void GameObjectForceApart2D(APIOBJECT_s *, APIOBJECT_s *) {
}

static __used__ bool FindSlamOrigin_UseCPos(GameObject_s *) {
    return false;
}
