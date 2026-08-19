#include "decomp.h"
#include "legoapi/legoapi_types.h"

void GizBuildItMul(GameObject_s *) {
}

void GizBuildIt_Find(WORLDINFO_s *, char *) {
}

void GizBuildIt_AtEnd(GIZBUILDIT_s *) {
}

void GizBuildIt_Finish(GIZBUILDIT_s *) {
}

void GizBuildIt_AtStart(GIZBUILDIT_s *) {
}

void GizBuildIt_TurnOff(GIZBUILDIT_s *) {
}

void GizBuildIt_SetToEnd(GIZBUILDIT_s *) {
}

void GizBuildIt_KillParts(GIZBUILDIT_s *) {
}

void GizBuildIt_SetToStart(GIZBUILDIT_s *, i32, i32) {
}

void GizBuildIt_AnyReacting(WORLDINFO_s *) {
}

void GizBuildIt_FindNearest(WORLDINFO_s *, GameObject_s *, BUILDIT_FIND_ENUM, i32) {
}

void GizBuildIt_SetStepTime(GIZBUILDIT_s *, GameObject_s *) {
}

void GizBuildIts_TotalScore(void *) {
}

void GizBuildIts_UpdateHint(HINT_s *) {
}

void GizBuildIt_SetHeadTarget(GIZBUILDIT_s *, GameObject_s *) {
}

void GizBuildItPushAwayFromEnd(GameObject_s *) {
}

void GizBuildItPushAwayFromStart(GameObject_s *, GIZBUILDIT_s *) {
}

void GIZBUILDIT_s::ClearMechObjectInterface() {
}

void GIZBUILDIT_s::GetMechObjectInterface() {
}

// Static build-it helper callbacks. Moved from gizmisc_stubs.cpp.

static __used__ void GizBuildIt_FinishFn_Game(GIZBUILDIT_s *) {
}

static __used__ bool GizBuildIt_CanStartBuildingFn_Game(GIZBUILDIT_s *, GameObject_s *) {
    return false;
}

static __used__ int GizBuildit_AutoBuildPos_Game(void *, nuvec_s *, nuvec_s *, u16 *) {
    return 0;
}
