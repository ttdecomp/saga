#include "decomp.h"
#include "legoapi/legoapi_types.h"

void GizObstacle_Stop(GIZOBSTACLE_s *) {
}

void GizObstacles_Hit(void *, GIZOBSTACLE_s *, nuvec_s *, i32, i32) {
}

void GizObstacle_JumpToEnd(GIZOBSTACLE_s *) {
}

void GizObstacle_FindByName(GIZOBSTACLESYS_s *, char *) {
}

void GizObstacle_FindNearest(GIZOBSTACLESYS_s *, nuvec_s *, GameObject_s *, float *, i32) {
}

void GizObstacle_JumpToStart(GIZOBSTACLE_s *) {
}

void GizObstacles_AddTrigger(nuvec_s *) {
}

void GizObstacles_TotalScore(void *) {
}

void GizObstacle_PlayForwards(GIZOBSTACLE_s *) {
}

void GizObstacle_PlayBackwards(GIZOBSTACLE_s *) {
}

void GizObstacle_SetPushControlled(GIZOBSTACLE_s *, GameObject_s *, float) {
}

void GizObstacle_SetDefaultSFXFn_LSW(void *, GIZOBSTACLE_s *) {
}

void GizObstacle_SetTechnoControlled(GIZOBSTACLE_s *, float) {
}

void GizObstacle_CheckExcludeFlagsFn_LSW(GIZOBSTACLE_s *, GameObject_s *) {
}

void GizObstacle_EvalAveragePosAndRadius(GIZOBSTACLE_s *, i32) {
}

void GIZOBSTACLE_s::ClearMechObjectInterface() {
}

void GIZOBSTACLE_s::GetMechObjectInterface() {
}

// Static obstacle update/terrain-check helpers. Moved from gizmisc_stubs.cpp.

static __used__ void GizObstacleUpdate_PushOnly(GIZOBSTACLE_s *) {
}

static __used__ void GizObstacleUpdate_AutoStart(GIZOBSTACLE_s *) {
}

static __used__ void GizObstacleUpdate_NoTrigger(GIZOBSTACLE_s *) {
}

static __used__ void GizObstacleUpdate_Proximity(GIZOBSTACLE_s *) {
}

static __used__ void GizObstacleUpdate_TechnoOnly(GIZOBSTACLE_s *) {
}

static __used__ int GizObstacle_SatisfyingTerrainChecks(GIZOBSTACLE_s *, GameObject_s *) {
    return 0;
}

static __used__ void Gizobstacle_ReadAnimSetData(GAMEANIMOBJ_s *, unsigned char) {
}
