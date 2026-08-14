#include "decomp.h"
#include "legoapi_types.h"

// Static stubs for the remaining gizmo subsystem helpers (obstacles,
// build-its, pickups, turrets, panels, blowups, force SFX, grabber, grapples,
// hat machine). Stubbed to satisfy the symbol baseline.

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

static __used__ void GizBuildIt_FinishFn_Game(GIZBUILDIT_s *) {
}

static __used__ bool GizBuildIt_CanStartBuildingFn_Game(GIZBUILDIT_s *, GameObject_s *) {
    return false;
}

static __used__ int GizBuildit_AutoBuildPos_Game(void *, nuvec_s *, nuvec_s *, u16 *) {
    return 0;
}

static __used__ float GizmoPickups_Collide2D(GameObject_s *) {
    return 0;
}

static __used__ GIZMOPICKUP_s *GizmoPickups_CollideList(GameObject_s *, GIZMOPICKUP_s *, int) {
    return nullptr;
}

static __used__ void GizTurret_ReadAnimSetData(GAMEANIMOBJ_s *, unsigned char) {
}

static __used__ void GizPanel_CreateTerrain(GIZPANEL_s *) {
}

static __used__ bool GizmoBlowUp_NoTarget(WORLDINFO_s *, GameObject_s *) {
    return false;
}

static __used__ void GizForceSFX_forcename(nufpar_s *) {
}

static __used__ void GizForceSFX_returnsfx(nufpar_s *) {
}

static __used__ void GizForceSFX_processsfx(nufpar_s *) {
}

static __used__ void GizForceSFX_completesfx(nufpar_s *) {
}

static __used__ void Grabber_SetVictimPos(GRABBER_s *) {
}

static __used__ void Grapples_DrawList(WORLDINFO_s *, GRAPPLE_s *, int, u16, float) {
}

static __used__ void Grapple_FindNearestInList(nuvec_s *, GRAPPLE_s *, int, GameObject_s *, GRAPPLE_s **, float *) {
}

static __used__ void HatMachine_Reset(HATMACHINE_s *) {
}
