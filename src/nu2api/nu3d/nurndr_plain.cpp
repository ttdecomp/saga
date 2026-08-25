
#include <string.h>

#include "decomp.h"
#include "globals.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nuandroid/ios_graphics.h"

// Current render scene being built (0x218 bytes), in bss like the original
// remaining.c binding. Referenced as a plain object by all render/present code.
extern "C" struct nudisplayscene_s currentScene = {0};

// Deferred render-scene ring: completed scenes are copied here by NuRndrEndScene
// and consumed by the render thread. Scene stride is 0x218; 16 ring slots in the
// original (0x2180 bytes total).
extern "C" i32 sceneParametersCount = 0;
extern "C" struct nudisplayscene_s sceneParameters[16] = {0};

// Immediate-mode 2D primitive stream state (globals shared with nuprim.cpp).
extern u8 *g_NuPrim_StreamBufferPtr;
i32 g_NuPrim_VertexCount;

// File-static primitive bookkeeping.
static u16 *g_NuPrim_VertexCountPtr;
static u16 g_NuPrim_CurrentPrimType;

extern "C" {
    void NuVpGetPosition2(i32 *, i32 *);
    void NuVpGetSize2(i32 *, i32 *);
    i32 NuDisplayListAddRenderScene(void);
    i32 NuDynamicLightIsEnabled(i32);
    void NuDynamicLightAddRenderScene(i32, i32, i32);

    void NuGScnFixupPS(void) {
    }
    void NuGScnFixupTIDsPS(void) {
    }
    void NuGScnFromVideoMem(void) {
    }
    void NuGScnGetSpecial(void) {
    }
    void NuGScnNumSpecials(void) {
    }
    void NuGScnReadForMultiRender(void) {
    }
    void NuGScnRemove(void) {
    }
    void NuGScnRestoreTIDsPS(void) {
    }
    void NuGScnRndr(void) {
    }
    void NuGScnToVideoMem(void) {
    }
    void NuMtlAnimate(void) {
    }
    void NuMtlAnimateSetMask(void) {
    }
    void NuMtlAnimateSetSpeedScale(void) {
    }
    void NuMtlAnimateShaderMtlTextures(void) {
    }
    void NuMtlCopy(void) {
    }
    void NuMtlCreate3D(void) {
    }
    void NuMtlCreateBuff(void) {
    }
    void NuMtlCreateBuff3D(void) {
    }
    void NuMtlCreateEx(void) {
    }
    void NuMtlCreateEx3D(void) {
    }
    void NuMtlDestroy(void) {
    }
    void NuMtlFindVariantMtl(void) {
    }
    void NuMtlFindVariantMtlFromDesc(void) {
    }
    void NuMtlInitOverride(void) {
    }
    void NuMtlReadEventSetHandler(void) {
    }
    void NuMtlRegisterForOverride(void) {
    }
    void NuMtlSetCurrentRenderPlane(void) {
    }
    void NuMtlSetRenderPlane(void) {
    }
    void NuMtlSetRenderStatesPS(void) {
    }
    void NuMtlSpecialSetUV(void) {
    }
    void NuPrim2DAddXYZ(float, float, float) {
    }
    void NuPrim2DBegin(i32, i32, NUMTL *) {
    }
    void NuPrim2DEnd(void) {
        u16 *cnt_ptr = g_NuPrim_VertexCountPtr;
        *cnt_ptr = (u16)g_NuPrim_VertexCount;
        g_NuPrim_VertexCount = 0;
    }
    void NuPrim3DEnd(void) {
        u16 *cnt_ptr = g_NuPrim_VertexCountPtr;
        *cnt_ptr = (u16)g_NuPrim_VertexCount;
        g_NuPrim_VertexCount = 0;
    }
    void NuRndr3dLine(void) {
    }
    void NuRndrAddFootPrint(void) {
    }
    void NuRndrAddShadow(void) {
    }
    void NuRndrAddShadowPrims(void) {
    }
    void NuRndrAnglesZX(void) {
    }
    void NuRndrAxes(void) {
    }
    void NuRndrAxisArrows(void) {
    }
    void NuRndrAxisArrowsMtx(void) {
    }
    void NuRndrAxisBright(void) {
    }
    i32 NuRndrBeginScene(i32 begin_flags) {
        currentScene.unknown_4c = 0;
        currentScene.render_scene_id = 0xffffffff;
        currentScene.clear_flags = 0;
        currentScene.state_ptr = NULL;
        currentScene.unknown_24 = NULL;
        currentScene.unknown_28 = 0;
        currentScene.unknown_58 = 0;
        currentScene.unknown_38 = 0;
        currentScene.unknown_3c = 0;
        currentScene.unknown_40 = 0xffffffff;
        currentScene.unknown_48 = 0;
        currentScene.unknown_ac = 0;
        currentScene.unknown_c4 = 0;
        currentScene.unknown_e4 = 0;
        currentScene.unknown_e0 = 0;
        currentScene.unknown_178 = 0;
        currentScene.unknown_174 = 0;
        currentScene.unknown_188 = 0;
        currentScene.unknown_214 = 0;
        return 1;
    }
    i32 NuRndrBeginSceneEx(i32 begin_flags) {
        return NuRndrBeginScene(begin_flags);
    }
    void NuRndrBoundingBox(void) {
    }
    void NuRndrBurstObjAdd(void) {
    }
    void NuRndrBurstObjAddNoClip(void) {
    }
    void NuRndrBurstObjEnd(void) {
    }
    void NuRndrCircle(void) {
    }
    void NuRndrClear(i32 clear_flags, i32 bg_colour, f32 alpha) {
        struct nudisplayscene_s *scene = &currentScene;

        if (NuIOS_IsLowEndDevice() && g_BackgroundUsedFogColour) {
            bg_colour = g_BackgroundColour;
        }

        scene->clear_alpha = alpha;
        scene->clear_flags |= clear_flags;
        scene->bg_colour = bg_colour;
        NuVpGetPosition2(&scene->vp_x, &scene->vp_y);
        NuVpGetSize2(&scene->vp_w, &scene->vp_h);
    }
    void NuRndrCreateBlendShapeDeformerWeightsArray(void) {
    }
    void NuRndrDither(void) {
    }
    void NuRndrEndReflectionRender(void) {
    }
    void NuRndrEndScene(void) {
        i32 cnt;
        struct nudisplayscene_s *scn = &currentScene;

        scn->render_scene_id = NuDisplayListAddRenderScene();

        if (scn->unknown_38 != 0 && scn->render_scene_id != -1) {
            if (NuDynamicLightIsEnabled(scn->unknown_3c)) {
                NuDynamicLightAddRenderScene(scn->unknown_3c, scn->unknown_40, scn->render_scene_id);
            }
            scn->unknown_38 = 0;
            scn->render_scene_id = -1;
        }

        cnt = sceneParametersCount;
        sceneParametersCount = cnt + 1;
        memcpy(&sceneParameters[cnt], scn, 0x218);
    }
    void NuRndrEndSceneEx(void) {
        NuRndrEndScene();
    }
    void NuRndrEndShadowReceiveRender(void) {
    }
    void NuRndrFootPrints(void) {
    }
    void NuRndrFx(void) {
    }
    void NuRndrGetCullDebug(void) {
    }
    void NuRndrGlobalFrameCount(void) {
    }
    void NuRndrGlobalFrameCountPause(void) {
    }
    void NuRndrGradClear(i32 a, i32 b, i32 c, f32 d) {
        NuRndrClear(a, b, d);
    }
    void NuRndrGradRect2di(void) {
    }
    void NuRndrGradRectUV2di(void) {
    }
    void NuRndrGrid(void) {
    }
    void NuRndrHighResScreenGrab(void) {
    }
    void NuRndrLine2d(void) {
    }
    void NuRndrLine2di(void) {
    }
    void NuRndrLine3d(void) {
    }
    void NuRndrLine3dDbg(void) {
    }
    void NuRndrLine3dDbgFlush(void) {
    }
    void NuRndrLineRect2di(void) {
    }
    void NuRndrLineStrip2d(void) {
    }
    void NuRndrLineStrip2di(void) {
    }
    void NuRndrParticleGroup(void) {
    }
    void NuRndrPspDraw(void) {
    }
    void NuRndrRect(void) {
    }
    void NuRndrRect2d(void) {
    }
    void NuRndrRect2di(void) {
    }
    void NuRndrRectUV2di(void) {
    }
    void NuRndrScreenGrabTileBegin(void) {
    }
    void NuRndrScreenGrabTileDeInit(void) {
    }
    void NuRndrScreenGrabTileEnd(void) {
    }
    void NuRndrScreenGrabTileInit(void) {
    }
    void NuRndrSetAmbientLightPS(void) {
    }
    void NuRndrSetAmbientLightSpecular(void) {
    }
    void NuRndrSetBlendData(void) {
    }
    void NuRndrSetCullDebug(void) {
    }
    void NuRndrSetDebBaseRange(void) {
    }
    void NuRndrSetDebBox(void) {
    }
    void NuRndrSetDirectionalLightsPS(void) {
    }
    void NuRndrSetFxMtx(void) {
    }
    void NuRndrSetGlobalMinMipLevel(void) {
    }
    void NuRndrSetGlobalMipMapBias(void) {
    }
    void NuRndrSetParticleRotation(void) {
    }
    void NuRndrSetSpecularLightPS(void) {
    }
    void NuRndrSetWind(void) {
    }
    void NuRndrShadPolys(void) {
    }
    void NuRndrShadowDirCol(void) {
    }
    void NuRndrShadowInit(void) {
    }
    void NuRndrShadowOnOff(void) {
    }
    void NuRndrSolidTri(void) {
    }
    void NuRndrSphere(void) {
    }
    void NuRndrSphereEx(void) {
    }
    void NuRndrSphereMtx(void) {
    }
    void NuRndrSphereTRS(void) {
    }
    void NuRndrStartReflectionRender(void) {
    }
    void NuRndrStartShadowReceiveRender(void) {
    }
    void NuRndrStateGetFogEnabled(void) {
    }
    void NuRndrStateInit(void) {
    }
    void NuRndrStateSetFogEnabled(void) {
    }
    void NuRndrStateSetFogState(void) {
    }
    void NuRndrStateSetSpecularLight(void) {
    }
    void NuRndrStateSetSpecularLightEx(void) {
    }
    void NuRndrStateUpdateCameraState(void) {
    }
    void NuRndrStrip3d(void) {
    }
    void NuRndrSwapScreen(void) {
    }
    void NuRndrSwapScreenEx(void) {
    }
    void NuRndrSwapStreamBuffers(void) {
    }
    void NuRndrTrailEx(void) {
    }
    void NuRndrTri3dClip(void) {
    }
    void NuRndrTriStrip2di(void) {
    }
    void NuRndrTriStrip3dClip(void) {
    }
    void NuRndrWasDrawnUnreflectedGobj(void) {
    }
    void NuRndrWireTri(void) {
    }
    void NuShaderGetDirtyMask(void) {
    }
    void NuShaderManagerBindShader(void) {
    }
    void NuShaderManagerDestroy(void) {
    }
    void NuShaderManagerDestroyShaders(void) {
    }
    void NuShaderManagerForceShader(void) {
    }
    void NuShaderManagerGetCurrentShader(void) {
    }
    void NuShaderManagerGetInstance(void) {
    }
    void NuShaderManagerGetShaderById(void) {
    }
    void NuShaderManagerGetShininessFactor(void) {
    }
    void NuShaderManagerInit(void) {
    }
    void NuShaderManagerLoadCompiledShaders(void) {
    }
    void NuShaderManagerReleaseShader(void) {
    }
    void NuShaderManagerRetrieveShader(void) {
    }
    void NuShaderManagerRetrieveShaderVariant(void) {
    }
    void NuShaderManagerSetCurrentShader(void) {
    }
    void NuShaderManagerSetElementfv(void) {
    }
    void NuShaderManagerSetElementsfv(void) {
    }
    void NuShaderManagerSetElementsfv_transpose(void) {
    }
    void NuShaderManagerSetShaderSaveFolder(void) {
    }
    void NuShaderManagerSetShininessFactor(void) {
    }
    void NuShaderManagerSetfv(void) {
    }
    void NuShaderObjectBaseUpdateWaterTable(void) {
    }
    void NuShaderObjectGLSLAllocateParameter(void) {
    }
    void NuShaderObjectGLSLProbeSemantics(void) {
    }
    void NuShaderObjectGLSLSetupMaterial(void) {
    }
    void NuShaderObjectGLSLSetupTextureStates(void) {
    }
    void NuShaderObjectInit(void) {
    }
    void NuShaderObjectKeyGenerate2(void) {
    }
    void NuShaderObjectKeyGenerate3(void) {
    }
    void NuShaderObjectKeyGenerate4(void) {
    }
    void NuShaderObjectKeySetUberShaderHash(void) {
    }
    void NuShaderObjectLoadFromFile(void) {
    }
    void NuShaderObjectSetElementsfv(void) {
    }
    void NuShaderObjectSetElementsfv_transpose(void) {
    }
    void NuShaderObjectUnserialize(void) {
    }
    void NuShaderProgramCreateIOS(void) {
    }
    void NuShaderUniformGetByString(void) {
    }
    void NuTexAnimAddList(void) {
    }
    void NuTexAnimCreate(void) {
    }
    void NuTexAnimDestroy(void) {
    }
    void NuTexAnimEnvCreate(void) {
    }
    void NuTexAnimEnvDestroy(void) {
    }
    void NuTexAnimEnvProc(void) {
    }
    void NuTexAnimEnvReset(void) {
    }
    void NuTexAnimFind(void) {
    }
    void NuTexAnimProcess(void) {
    }
    void NuTexAnimProcessEx(void) {
    }
    void NuTexAnimProcessList(void) {
    }
    void NuTexAnimProgAssembleEnd(void) {
    }
    void NuTexAnimProgCreate(void) {
    }
    void NuTexAnimProgDestroy(void) {
    }
    void NuTexAnimProgFind(void) {
    }
    void NuTexAnimProgRead(void) {
    }
    void NuTexAnimProgReadCFG(void) {
    }
    void NuTexAnimProgReadScript(void) {
    }
    void NuTexAnimProgRelease(void) {
    }
    void NuTexAnimProgSysInit(void) {
    }
    void NuTexAnimProgWrite(void) {
    }
    void NuTexAnimRemoveList(void) {
    }
    void NuTexAnimRestart(void) {
    }
    void NuTexAnimSetMask(void) {
    }
    void NuTexAnimSetSignals(void) {
    }
    void NuTexCleartid(void) {
    }
    void NuTexCreateEx(void) {
    }
    void NuTexCreateFramebufferCopy(void) {
    }
    void NuTexDestroy(void) {
    }
    void NuTexGenTexture(void) {
    }
    void NuTexReserveNative(void) {
    }
    void NuTexResolveReference(void) {
    }
    void NuTextureBlendEffect(void) {
    }
    void NuVertexStatesCreate(void) {
    }
    void NuVertexStatesSetGroupState(void) {
    }
}