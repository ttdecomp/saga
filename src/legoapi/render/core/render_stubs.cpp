#include "legoapi/world/world_shared.h"
#include "decomp.h"
#include "nu2api/numath/numtx.h"

struct numtl_s;
typedef struct numtl_s NUMTL;

extern "C" {

    void AddColourPick(void) {
    }

    void BoundingBoxToLine(void) {
    }

    void BuildCamSpaceClipPlanes(void) {
    }

    void BuildWorldSpaceClipPlanes(void) {
    }

    void ChooseCorrectLOD(void) {
    }

    void DisplayListCreateFaceonTransformPS(void) {
    }

    void DisplayListCreateFxItemPS(void) {
    }

    void DisplayListCreateFxList(void) {
    }

    void *DisplayListCreateGeomTransformPS(VARIPTR *buffer, NUMTX *transform, NUMTL *mtl, void *next, void *tx) {
        (void)mtl;
        (void)next;
        (void)tx;

        buffer->addr = ALIGN(buffer->addr, 4);
        NUMTX *result = static_cast<NUMTX *>(buffer->void_ptr);
        NuMtxTranspose(result, transform);
        buffer->addr += sizeof(NUMTX);
        return result;
    }

    void DisplayListCreatePS(void) {
    }

    void DisplayListCreateRigidSkin2TransformPS(void) {
    }

    void DisplayListCreateRigidSkinTransformPS(void) {
    }

    void DisplayListCreateSkinTransformPS(void) {
    }

    void DisplayListDebugPS(void) {
    }

    void DisplayListDestroyFxItemPS(void) {
    }

    void DisplayListPrintItemPS(void) {
    }

    // DisplayListSetAlphaPS is fully transcribed in nu3d/nudlist.cpp (original 0x29b8c0 / 0x29b77e).
    void DisplayListSetFxItemParamPS(void) {
    }

    void DisplayListSetShadowCasterFlagPS(void) {
    }

    // DisplayListSwapBuffersPS is fully transcribed in nu3d/nudlist.cpp (original 0x29b8c0 / 0x29b77e).
    void DisplayListUpdateRenderStateShadow(void) {
    }

    void DisplayListUpdateSpecialTransformPS(void) {
    }

    void DisplaySceneRndrSpecials(void) {
    }

    void FmvTimePS(void) {
    }

    void GenericDebinfoDmaTypeUpdate(void) {
    }

    void Initialise_PS(void) {
    }

    void LinkDmaParticalSets(void) {
    }

    void LocaledbitsDrawCircleXY(void) {
    }

    void LocaledbitsDrawSolidCircleXY(void) {
    }

    void LocaledbitsDrawSolidEllipseXY(void) {
    }

    void MakeLayerList_Index(void) {
    }

    void PerspectMidPoint(void) {
    }

    void RndrCircleXZ(void) {
    }

    void RndrMaskScreen(void) {
    }

    void RndrOSphere(void) {
    }

    void RndrOSquare(void) {
    }

    void RndrStateBuildFogState(void) {
    }

    void RndrStateBuildLightState(void) {
    }

    void RndrStateBuildVertexGroupsStates(void) {
    }

    void RndrStateBuildVertexOffsetsStates(void) {
    }

    void RndrStateCopyGlobalState(void) {
    }

    void RndrStateResetGlobalState(void) {
    }

    void RndrStateResetSharedGlobalState(void) {
    }

    void RndrStateSetConstAlphaTint(void) {
    }

    void RndrStateSetReflection(void) {
    }

    void RndrStateUpdate(void) {
    }

    void RndrStateUpdateFx(void) {
    }

    void SetAiRndrCullDistance(void) {
    }

    void SetAllInstancesVisible(void) {
    }

    void ShadowDir(void) {
    }

    void ShadowInfo(void) {
    }

    void ShadowIntensityInfo(void) {
    }

    void ShadowRoofInfo(void) {
    }

    void SphereDraw(void) {
    }

    void SphereDrawEx(void) {
    }

    void clipRoomAgainstFrustrum(void) {
    }

    void clipTestBox(void) {
    }

    void clipTestSphere(void) {
    }

    void glDeleteVertexArraysOESC(void) {
    }

    void glGenVertexArraysOESC(void) {
    }

} // extern "C"
