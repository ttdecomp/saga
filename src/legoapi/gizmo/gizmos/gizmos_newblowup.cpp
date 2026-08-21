#include "decomp.h"
#include "legoapi/legoapi_types.h"

void GizmoBlowUp_Hit(GameObject_s *, nuvec_s *, i32, float, nuvec_s *, nuvec_s *, BOLT_s *, u32, unsigned char *) {
}

void GizmoBlowUp_Sfx(GIZMOBLOWUP_s *, nuvec_s *) {
}

void GizmoBlowUp_Target(GameObject_s *, nuvec_s *, nuvec_s *, float, float, i32, i32, i32) {
}

void GizmoBlowUpOpponent(GameObject_s *, float, float, float, i32, u32, u32, u32) {
}

void GizmoBlowUpTypeBlowUp(WORLDINFO_s *, i32, nuvec_s *) {
}

void GizmoBlowUp_AddEffects(nuvec_s *, GIZMOBLOWUP_s *, i32, i32, GameObject_s *) {
}

GIZMOBLOWUP_s *GizmoBlowUp_FindByName(WORLDINFO_s *, char *) {
    return NULL;
}

void GizmoBlowUp_FindFromPlatID(WORLDINFO_s *, i32) {
}

void GIZMOBLOWUP_s::ClearMechObjectInterface() {
}

void GIZMOBLOWUP_s::GetMechObjectInterface() {
}

// Static blowup no-target check. Moved from gizmisc_stubs.cpp.

static __used__ bool GizmoBlowUp_NoTarget(WORLDINFO_s *, GameObject_s *) {
    return false;
}
