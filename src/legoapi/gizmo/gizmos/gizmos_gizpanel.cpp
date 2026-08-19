#include "decomp.h"
#include "legoapi/legoapi_types.h"

void GizPanel_Use(GameObject_s &, GIZPANEL_s &) {
}

void GizPanel_Reset(GIZPANEL_s *) {
}

void GizPanel_PlaySfx(char *, nuvec_s *, i32) {
}

void GizPanel_MoveCode(WORLDINFO_s *, GameObject_s *, i32) {
}

void GizPanel_BeingUsed(GIZPANEL_s *) {
}

void GizPanel_FindByName(WORLDINFO_s *, char *) {
}

void GizPanel_UpdateHint(HINT_s *) {
}

void GizPanel_CanUsePanel(GameObject_s *, GIZPANEL_s *) {
}

void GizPanel_FindNearest(WORLDINFO_s *, nuvec_s *, GameObject_s *, float *, i32) {
}

void GizPanel_InitTerrain(WORLDINFO_s *) {
}

void GizPanel_GetAbsPlayerPos(GIZPANEL_s *, nuvec_s *) {
}

void GizPanel_GetAbsTargetPos(GIZPANEL_s *, nuvec_s *, i32) {
}

void GIZPANEL_s::ClearMechObjectInterface() {
}

void GIZPANEL_s::GetMechObjectInterface() {
}

// Static panel terrain helper. Moved from gizmisc_stubs.cpp.

static __used__ void GizPanel_CreateTerrain(GIZPANEL_s *) {
}
