#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void GizmoBlowupDraw(void *, void *, float) {
}

void GizmoSortBlowups(WORLDINFO_s *) {
}

void GizmoSwapBlowups(GIZMOBLOWUP_s *, GIZMOBLOWUP_s *) {
}

void InitGizmoBlowups(WORLDINFO_s *) {
}

void GizBlowup_Respawn(GIZMOBLOWUP_s *) {
}

void GizmoBlowupBlowup(GIZMOBLOWUP_s *, i32, i32, i32, GameObject_s *, i32) {
}

void BlowupObjEmit_Stop(PART_s *) {
}

void GizmoBlowupTypeAdd(WORLDINFO_s *, nuhspecial_s *, i32, i32 *) {
}

void GizmoBlowup_FindType(char *, WORLDINFO_s *) {
}

void InitGizmoBlowupTypes(WORLDINFO_s *) {
}

void SetGizmoBlowUpTarget(GameObject_s *, GIZMOBLOWUP_s *) {
}

void GizBlowup_InitTerrain() {
}

void GizmoBlowupTypeRemove(GIZMOBLOWUPTYPE_s *, WORLDINFO_s *) {
}

void GizmoBlowup_HitBlowup(GameObject_s *, GIZMOBLOWUP_s *, i32, BOLT_s *, nuvec_s *, unsigned char *, u32, i32) {
}

void FindNearestGizmoBlowUp(WORLDINFO_s *, nuvec_s *, float) {
}

void GizmoBlowupCreateStuff(GIZMOBLOWUP_s *) {
}

void GizmoBlowupsFinalSetup(WORLDINFO_s *) {
}

void GizBlowup_DeleteTerrain() {
}

void GizmoBlowupTypeGetCount(WORLDINFO_s *) {
}

void GizmoBlowupUpdateMatrix(GIZMOBLOWUP_s *) {
}

void GizmoBlowups_TotalScore(void *) {
}

void GizmoBlowupTypeNameBlank(char *) {
}

void GizmoBlowupCheckProximity(WORLDINFO_s *, GameObject_s *) {
}

void GizmoBlowupGenDecalMatrix(GIZMOBLOWUP_s *, numtx_s *, i32) {
}

void GizmoBlowupGetNameTableId(char *) {
}

void InitGizmoBlowupsMtxBuffer(WORLDINFO_s *) {
}

void RemapTypeFlagToBlowupFlag(u32) {
}

void GizmoBlowupGenShadowMatrix(GIZMOBLOWUP_s *, numtx_s *) {
}

void GizBlowup_InitSingleTerrain(GIZMOBLOWUP_s *) {
}

void GizBlowup_DeleteSingleTerrain(GIZMOBLOWUP_s *) {
}

void GizmoBlowupVisibilityOverrides(WORLDINFO_s *) {
}

void GizmoBlowup_SetAutoSetReflectY(GIZMOBLOWUP_s *, nuvec_s *) {
}

void GizmoBlowup_TransformDraw_Game(GIZMOBLOWUP_s *) {
}

void RemapAllTypeFlagsToBlowupFlags(u32) {
}

void GizmoBlowupTypeGetNameFromIndex(WORLDINFO_s *, i32) {
}

void GizmoBlowupGetTypeFromNameTableId(WORLDINFO_s *, i32) {
}
