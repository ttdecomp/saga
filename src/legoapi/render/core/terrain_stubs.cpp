#include "legoapi/world/world_shared.h"
#include "decomp.h"

extern "C" void noterraininit(void) {
}
extern "C" void TerrainSetCur(void *terrain) {
    (void)terrain;
}
extern "C" void TerrSetPlatScanDist(f32 dist) {
    (void)dist;
}
extern "C" void TerrainPlatformOldUpdate(void) {
}
extern "C" void TerrainPlatformNewUpdate(void) {
}
extern "C" void TerrainSetWallDeflectYScale(f32 scale) {
    (void)scale;
}

void *InitPartDebris(VARIPTR *buf, VARIPTR *buf_end, i32 param1, i32 param2, char **param3, i32 page) {
    (void)buf;
    (void)buf_end;
    (void)param1;
    (void)param2;
    (void)param3;
    (void)page;
    return NULL;
}

void Particles_Load(WORLDINFO *world, char **debris_name, i32 count, i32 flags) {
    (void)world;
    (void)debris_name;
    (void)count;
    (void)flags;
}

extern "C" {

    void AITerrInit(void) {
    }

    void AITerrShadow(void) {
    }

    void AITerrShadowOnPlatform(void) {
    }

    void CheckForPlatInst(void) {
    }

    void CreateDmaPartEffectList(void) {
    }

    void CreateDmaParticleSet(void) {
    }

    void CreateDmaParticleSetGlass(void) {
    }

    void CreateScaledEffect(void) {
    }

    void CreateScaledPARTEffect(void) {
    }

    void CubeImpact(void) {
    }

    void DebFreeAllCreatedEffects(void) {
    }

    void DebFreeAllDMADebTablesInstantly(void) {
    }

    void DebFreeAllPanelEffects(void) {
    }

    void Debris(i32) {
    }

    void DebrisAllCollisionCheckScaleYFlag(void) {
    }

    void DebrisCollisionCheck(void) {
    }

    void DebrisCollisionCheckFlag(void) {
    }

    void DebrisCollisionCheckScaleY(void) {
    }

    void DebrisCollisionCheckScaleYFlag(void) {
    }

    void DebrisDraw(void) {
    }

    void DebrisDrawGlass(void) {
    }

    void DebrisDrawGlassEx(void) {
    }

    void DebrisEmitterMomentum(void) {
    }

    void DebrisEmitterOrientation(void) {
    }

    void DebrisEmitterOrientationMtx(void) {
    }

    void DebrisEmitterPos(void) {
    }

    void DebrisFindAllOfType(void) {
    }

    void DebrisGetConeProperties(void) {
    }

    void DebrisGetDuration(void) {
    }

    void DebrisGetName(void) {
    }

    void DebrisGetParticleCount(void) {
    }

    void DebrisGetRingProperties(void) {
    }

    void DebrisGetSeed(void) {
    }

    void DebrisGlassClose(void) {
    }

    void DebrisGlassInit(void) {
    }

    void DebrisGlassParticlesActive(void) {
    }

    void DebrisOrientation(void) {
    }

    void DebrisOrientationMtx(void) {
    }

    void DebrisParticleMomentum(void) {
    }

    void DebrisPopulateInstance(void) {
    }

    void DebrisPosOrientationMtx(void) {
    }

    void DebrisPreCheckCollisions(void) {
    }

    void DebrisProcessTimeSlip(void) {
    }

    void DebrisQueryPriority(void) {
    }

    void DebrisReScale(void) {
    }

    void DebrisReflectionOrientation(void) {
    }

    void DebrisRegisterCutoffCameraVec(void) {
    }

    void DebrisReserveTrashableSpace(void) {
    }

    void DebrisResetTimers(void) {
    }

    void DebrisSetCutSceneMode(void) {
    }

    void DebrisSetDetailLevels(void) {
    }

    void DebrisSetDrawFlag(void) {
    }

    void DebrisSetFacing(void) {
    }

    void DebrisSetGroupID(void) {
    }

    void DebrisSetPriority(void) {
    }

    void DebrisSetRenderGroup(void) {
    }

    void DebrisSetRoomID(void) {
    }

    void DebrisSetSeed(void) {
    }

    void DebrisSetTimeIncrement(void) {
    }

    void DebrisSetTrigger(void) {
    }

    void DebrisSetUserData(void) {
    }

    void DebrisSetup(void) {
    }

    void DebrisSetup2(void) {
    }

    void DebrisShift(void) {
    }

    void DebrisStartOffset(void) {
    }

    void DebrisStartOffsetEx(void) {
    }

    void DebrisStatusAlwaysOff(void) {
    }

    void DebrisStatusAlwaysOn(void) {
    }

    void DebrisStatusNormal(void) {
    }

    void DebrisTorusCollisionCheck(void) {
    }

    void DebrisTorusCollisionCheckFlag(void) {
    }

    void DebrisTorusCollisionCheckScaleY(void) {
    }

    void DebrisTorusCollisionCheckScaleYFlag(void) {
    }

    void DebrisTrashableSetup(void) {
    }

    void DebrisTypeStatusAlwaysOff(void) {
    }

    void DebrisTypeStatusAlwaysOn(void) {
    }

    void DebrisTypeStatusNormal(void) {
    }

    void DeletePlatinst(void) {
    }

    void DrawHitTerrain(void) {
    }

    void DrawPlatform(void) {
    }

    void FindPlatInst(void) {
    }

    void NewMSituTerrEx(void) {
    }

    void NewPlatInst(void) {
    }

    void NewPlatInstMSitu(void) {
    }

    void NewPlatPickupInst(void) {
    }

    void NewRayCast(void) {
    }

    void NewRayCastEx(void) {
    }

    void NewRayCastGetEmbedDist(void) {
    }

    void NewRayCastGetImpactNormal(void) {
    }

    void NewRayCastGetImpactTerrainType(void) {
    }

    void NewRayCastGetTOFI(void) {
    }

    void NewRayCastHitWallSpline(void) {
    }

    void NewRayCastMask(void) {
    }

    void NewRayCastPlatForm(void) {
    }

    void NewRayCastScaleY(void) {
    }

    void NewRayCastScaleYMask(void) {
    }

    void NewRayCastSet(void) {
    }

    void NewRayCastSetHandel(void) {
    }

    void NewRayCastSetMask(void) {
    }

    void NewRaySetDisablePalt(void) {
    }

    void NewScanHandel(void) {
    }

    void NewScanInit(void) {
    }

    void NewShadow(void) {
    }

    void NewShadowEx(void) {
    }

    void NewShadowHandelEx(void) {
    }

    void NewShadowOnMSitu(void) {
    }

    void NewShadowOnPlatform(void) {
    }

    void NewShapeInit(void) {
    }

    void NewTerrAxisFreedom(void) {
    }

    void NewTerrHitInfo(void) {
    }

    void NewTerrPlatformsOff(void) {
    }

    void NewTerrain(void) {
    }

    void NewTerrainOnAPlatform(void) {
    }

    void NewTerrainScaleY(void) {
    }

    void NewTerrainScaleYMask(void) {
    }

    void PartRayCast(void) {
    }

    void PartTerrInit(void) {
    }

    void PlatImpactInfo(void) {
    }

    void PlatInstBounce(void) {
    }

    void PlatInstCenter(void) {
    }

    void PlatInstGetHit(void) {
    }

    void PlatInstRotate(void) {
    }

    void PlatInstSkinRegister(void) {
    }

    void PlatInstSkinRegisterEx(void) {
    }

    void PlatOnOff(void) {
    }

    void PlatSkinEndReigster(void) {
    }

    void PlatSkinMemReset(void) {
    }

    void PlatSkinMemRigister(void) {
    }

    void PlatformCrush(void) {
    }

    void PlatformRemoveCallback(void) {
    }

    void PlatformUpdateCallback(void) {
    }

    void QuickNewRayCast(void) {
    }

    void SortDebrisRenderStack(void) {
    }

    void TerrDrawImpactPol(void) {
    }

    void TerrErrorString(void) {
    }

    void TerrTempMemory(void) {
    }

    void TerrainAddWallSpline(void) {
    }

    void TerrainFreeId(void) {
    }

    void TerrainGetCur(void) {
    }

    void TerrainGetModelByInst(void) {
    }

    void TerrainInfo(void) {
    }

    void TerrainInfoExtra(void) {
    }

    void TerrainIntensityInfo(void) {
    }

    void TerrainPlatGetMtx(void) {
    }

    void TerrainPlatId(void) {
    }

    void TerrainPolyEdge(void) {
    }

    void TerrainRemoveWallSpline(void) {
    }

    void TerrainScanWallSpline(void) {
    }

    void TerrainSetImpactData(void) {
    }

    void TerrainSetPlatConnectTol(void) {
    }

    void TerrainTrackBack(void) {
    }

    void TerrainTrackFlush(void) {
    }

    void TerrainWallAng(void) {
    }

    void TerrainWallSideSlide(void) {
    }

    void UpdateDebrisRenderStackPriority(void) {
    }

    void terraininit(void) {
    }

    void terrainpickupinit(void) {
    }

    void UpdatePlatinst(void) {
    }

} // extern "C"
