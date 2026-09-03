#include "decomp.h"
#include "legoapi/legoapi_types.h"

void GizmoPickup_InBox(WORLDINFO_s *, i32, nuvec_s *, nuvec_s *) {
}

void GizmoPickups_Collide(WORLDINFO_s *, GameObject_s *, i32) {
}

void GizmoPickups_SetOnOff() {
}

void GizmoPickup_FindByName(WORLDINFO_s *, char *) {
}

void GizmoPickup_CollectCoin(WORLDINFO_s *, nuvec_s *, i32, i32, GameObject_s *, i32) {
}

void GizmoPickup_FindNearest(WORLDINFO_s *, nuvec_s *, float *) {
}

void GizmoPickups_TotalScore(void *) {
}

void GizmoPickup_BeenTurnedOn(GIZMOPICKUP_s *) {
}

void GizmoPickup_NumberOfType(WORLDINFO_s *, i32, char) {
}

void GizmoPickup_TurnOnPickup(GIZMOPICKUP_s *) {
}

// Static pickup collision helpers. Moved from gizmisc_stubs.cpp.

static __used__ float GizmoPickups_Collide2D(GameObject_s *) {
    return 0;
}

static __used__ GIZMOPICKUP_s *GizmoPickups_CollideList(GameObject_s *, GIZMOPICKUP_s *, int) {
    return nullptr;
}

static __used__ void Pup_CollectHeart(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectCharKit(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectMinikit(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectPowerUp(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectRedBrick(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_UpdatePowerUp(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

static __used__ void Pup_UpdateBlueCoin(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

static __used__ void Pup_UpdatePurpleCoin(WORLDINFO_s *, GIZMOPICKUP_s *) {
}
