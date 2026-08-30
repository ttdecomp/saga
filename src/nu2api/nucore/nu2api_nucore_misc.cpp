#include "nu2api_nucore_types.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuandroid/ios_graphics.h"

#include <GLES2/gl2.h>
#include <string.h>

extern "C" void BeginCriticalSectionGL(const char *, i32);
extern "C" void EndCriticalSectionGL(const char *, i32);

extern "C" f32 *NuAnimCurveExtractAllNodeCurves_3(ani3_animheader_s *, i32, f32, char *);
extern "C" void NuAnimData2CalcTime(nuanimdata2_s *, f32, nuanimtime_s *);
extern "C" f32 NuAnimCurve2CalcValEx(nuanimcurve2_s *, nuanimtime_s *, u32);
void NuGCutRigidCalcMtx_3(NUGCUTRIGID_s *, f32, numtx_s *);
extern void *globalbuffer;
extern i32 MaxAnimJoints;

void NuMemAlloc(i32) {
}

void NuHGobjRead(variptr_u *, char *) {
}

static void NuHtmlFlush(i32) {
}

void NuHtmlTitle(char *) {
}

void NuMemDumpFn(i32) {
}

void NuDDSGetSize(char const *) {
}

void NuErrorPrint(char *) {
}

void NuFntFindEnd(nutex_s *, i32 *, i32 *, i32, i32) {
}

void NuMemFlushFn() {
}

void NuBridgeAlloc() {
}

void NuWindFreeGrp(NuWindGType *) {
}

extern "C" void *NuAnimBuffCreate(i32 max_joints, variptr_u *buf);

void NuAnimBuffInit(i32 max_joints, variptr_u *buf, variptr_u) {
    MaxAnimJoints = max_joints;
    globalbuffer = NuAnimBuffCreate(max_joints, buf);
}

void NuFntFindStart(nutex_s *, i32 *, i32 *, i32, i32) {
}

void NuFntPrintChar(char) {
}

void NuQFntSetMtx2d(void *, numtx_s *) {
}

void NuRainDrawDrop(i32, i32, float, float, i32) {
}

void NuWarningPrint(char *) {
}

void NuASin_Accurate(float) {
}

void NuBez3EvaluateX(nuvec4_s *, float) {
}

void NuDebugMsgPrint(char *) {
}

void NuIOSDLLightmap(void *) {
}

void NuLgtArcLaserEx(i32, nuvec_s *, nuvec_s *, nuvec_s *, float, float, float, float, i32, i32) {
}

void NuMemValidateFn() {
}

void NuRainDrawShape(float) {
}

void NuVpSetDestRect(float, float, float, float) {
}

void NuBrdigeDrawRope(numtl_s *, nuvec_s *, nuvec_s *, i32, i32 *, i32) {
}

void NuDDSGetMipLevel(i32, i32, i32, NUTEXFORMAT, i32, bool, i32, i32, i32 &, i32 &, i32 &) {
}

void NuFadeObjFreeGrp(NuFadeObjGType *) {
}

void NuHtmlGraphArray(char **) {
}

void NuLgtSetArcMatEx(i32, numtl_s *, float, float, float, float) {
}

void NuMemGetExternal() {
}

void NuCameraClipHGobj(nugscn_s *, numtx_s *, numtx_s *) {
}

void NuFadeObjFreeMtxs(numtx_s *, i32) {
}

void NuFntDumpReadable(nufnt_s *, char *) {
}

// NuIOS_SetCullMode is transcribed in android/nuiosdl_gl.cpp (original 0x29c110).

void NuLgtArcLaserDraw(i32) {
}

void NuVpSetSourceRect(float, float, float, float) {
}

void NuWindAllocateGrp() {
}

void NuFadeObjAllocData(i32) {
}

void NuFadeObjAllocMtxs(i32) {
}

void NuFrameEndBgLoadPS(i32) {
}

void NuGCutRigidCalcMtx(NUGCUTRIGID_s *rigid, float frame, numtx_s *mtx) {
    if (rigid->animation == NULL) {
        *mtx = rigid->base_matrix;
        return;
    }
    if (*reinterpret_cast<u32 *>(rigid->animation) + 0xbeb1b6ccU < 2) {
        NuGCutRigidCalcMtx_3(rigid, frame, mtx);
        return;
    }

    nuanimdata2_s *animation = rigid->animation;
    nuanimcurve2_s *curves = animation->curves;
    u8 *curve_types = animation->curve_types;
    u8 flags = *animation->node_flags;
    nuanimtime_s time;
    NuAnimData2CalcTime(animation, frame, &time);

    auto evaluate = [&](u32 curve) {
        u8 type = curve_types[curve];
        return type == 0 ? curves[curve].data.constant : NuAnimCurve2CalcValEx(&curves[curve], &time, type);
    };

    if ((flags & 1) == 0) {
        NuMtxSetIdentity(mtx);
    } else {
        NUANGVEC angles = {
            static_cast<NUANG>(evaluate(3) * 10430.378f),
            static_cast<NUANG>(evaluate(4) * 10430.378f),
            static_cast<NUANG>(evaluate(5) * 10430.378f),
        };
        NuMtxSetRotateXYZ(mtx, &angles);
    }
    if ((flags & 8) != 0) {
        NUVEC scale = {evaluate(6), evaluate(7), evaluate(8)};
        NuMtxPreScale(mtx, &scale);
    }
    NUVEC translation = {evaluate(0), evaluate(1), evaluate(2)};
    NuMtxTranslate(mtx, &translation);
    mtx->m02 = -mtx->m02;
    mtx->m12 = -mtx->m12;
    mtx->m20 = -mtx->m20;
    mtx->m21 = -mtx->m21;
    mtx->m23 = -mtx->m23;
    mtx->m32 = -mtx->m32;
    NuMtxTranslate(mtx, reinterpret_cast<NUVEC *>(&rigid->base_matrix.m30));
}

void NuIOSDLFogCallback(void *) {
}

void NuIOSDLLightmapOld(void *) {
}

// NuIOSDLMtlCallback is transcribed in android/nuiosdl_gl.cpp (original 0x29c480).

// original 0x2f87d0
void NuDisplayListCreate(nudisplayscene_s *raw_scene, variptr_u *buffer, variptr_u, i32 item_count, i32 material_count,
                         i32, i32, i32 sort_priority_count, i32, i32 allocate_materials) {
    NUDLDLISTSCENE *scene = reinterpret_cast<NUDLDLISTSCENE *>(raw_scene);
    scene->nitems = item_count;
    scene->nmtls = material_count;

    scene->items = reinterpret_cast<NUDISPLAYLISTITEM *>(ALIGN(buffer->addr, 0x10));
    buffer->addr = reinterpret_cast<usize>(scene->items + item_count);

    scene->mtls = reinterpret_cast<NUMTL **>(buffer->void_ptr);
    buffer->addr += material_count * sizeof(NUMTL *);
    scene->dlist_mtls = reinterpret_cast<NUDISPLAYLIST **>(ALIGN(buffer->addr, 0x10));
    buffer->addr = reinterpret_cast<usize>(scene->dlist_mtls + material_count);
    memset(scene->mtls, 0, material_count * sizeof(NUMTL *));
    memset(scene->dlist_mtls, 0, material_count * sizeof(NUDISPLAYLIST *));

    if (allocate_materials != 0) {
        NUMTL *materials = reinterpret_cast<NUMTL *>(ALIGN(buffer->addr, 0x10));
        buffer->addr = reinterpret_cast<usize>(materials + material_count);
        for (i32 i = 0; i < material_count; ++i) {
            scene->mtls[i] = &materials[i];
        }
    }

    NUDISPLAYLIST *display_lists = reinterpret_cast<NUDISPLAYLIST *>(ALIGN(buffer->addr, 0x10));
    buffer->addr = reinterpret_cast<usize>(display_lists + material_count);
    memset(display_lists, 0, material_count * sizeof(NUDISPLAYLIST));

    NUDISPLAYLISTITEM *first_items = reinterpret_cast<NUDISPLAYLISTITEM *>(ALIGN(buffer->addr, 0x10));
    buffer->addr = reinterpret_cast<usize>(first_items + material_count);
    memset(first_items, 0, material_count * sizeof(NUDISPLAYLISTITEM));

    for (i32 i = 0; i < material_count; ++i) {
        NUDISPLAYLIST *display_list = &display_lists[i];
        scene->dlist_mtls[i] = display_list;
        display_list->state = reinterpret_cast<NURNDRSTATE *>(ALIGN(buffer->addr, 4));
        buffer->addr = reinterpret_cast<usize>(display_list->state + 1);
        display_list->first = &first_items[i];

        display_list->state->mtl = NULL;
        display_list->state->tex_id = -1;
        display_list->state->global_id = -1;
        display_list->state->lights_id = -1;
        display_list->state->camera_id = -1;
        display_list->state->fog_id = -1;
        display_list->state->konst_id = -1;
        display_list->state->reflection_id = -1;

        display_list->first->type = 0x8d;
        display_list->first->id = 1;
        display_list->first->next = NULL;
    }
    scene->local_state = NULL;

    i32 used_size = ((((material_count + 7) >> 3) & ~0xf) + 0x10);
    scene->mtl_used[0] = reinterpret_cast<u8 *>(ALIGN(buffer->addr, 0x10));
    scene->mtl_used[1] = scene->mtl_used[0] + used_size;
    scene->nsort_pris = sort_priority_count;
    scene->sort_pris = reinterpret_cast<NUSORTPRI *>(scene->mtl_used[1] + used_size);
    buffer->addr = reinterpret_cast<usize>(scene->sort_pris + sort_priority_count);
    memset(scene->sort_pris, 0, sort_priority_count * sizeof(NUSORTPRI));

    for (i32 i = 0; i < sort_priority_count; ++i) {
        scene->sort_pris[i].display_scene = scene;
        scene->sort_pris[i].nmtls = 0;
        scene->sort_pris[i].mtl_first = 0;
    }

    for (i32 i = 0; i < material_count; ++i) {
        NUDISPLAYLIST *display_list = scene->dlist_mtls[i];
        display_list->scene_buffer = 0;
        display_list->mtl_last = display_list->first;
        display_list->scene_next = display_list->scene_first[0];
    }
    scene->flags &= 0xf1;
}

void NuFadeSetFxCodeMtls(nugeom_s *, unsigned char *) {
}

void NuInitHardwareFirst(i32, variptr_u *, i32 *, i32) {
}

void NuPortalSetOverride(nuvec_s *) {
}

void NuSpecialGetAnimPos(nuhspecial_s *) {
}

void NuSpecialReflection(i32) {
}

void NuTerminateHardware() {
}

void NuDisplaySceneUnclip(nudisplayscene_s *) {
}

void NuFadeObjAllocateGrp() {
}

void NuGCutRigidCalcMtx_3(NUGCUTRIGID_s *rigid, float frame, numtx_s *mtx) {
    ani3_animheader_s *animation = reinterpret_cast<ani3_animheader_s *>(rigid->animation);
    f32 *values = NuAnimCurveExtractAllNodeCurves_3(animation, 0, frame, NULL);
    u8 flags = *animation->node_flags;
    if ((flags & 1) == 0) {
        NuMtxSetIdentity(mtx);
    } else {
        NUANGVEC angles;
        angles.x = static_cast<NUANG>(values[3] * 10430.378f);
        angles.y = static_cast<NUANG>(values[4] * 10430.378f);
        angles.z = static_cast<NUANG>(values[5] * 10430.378f);
        NuMtxSetRotateXYZ(mtx, &angles);
    }
    if ((flags & 8) != 0) {
        NUVEC scale = {values[6], values[7], values[8]};
        NuMtxPreScale(mtx, &scale);
    }
    NUVEC translation = {values[0], values[1], values[2]};
    NuMtxTranslate(mtx, &translation);
    mtx->m02 = -mtx->m02;
    mtx->m12 = -mtx->m12;
    mtx->m20 = -mtx->m20;
    mtx->m21 = -mtx->m21;
    mtx->m23 = -mtx->m23;
    mtx->m32 = -mtx->m32;
    NuMtxTranslate(mtx, reinterpret_cast<NUVEC *>(&rigid->base_matrix.m30));
}

void NuLightBurnoutEffect(i32, float, float, float) {
}

void NuAnimDataChunkCreate(i32) {
}

void NuFadeObjAngleTerrain(numtx_s *) {
}

void NuGCutSceneSysInitVfx(i32 (*)(char const *), i32 (*)(i32, VuMtx *), void (*)(i32), void (*)(i32, VuMtx *)) {
}

// NuIOSDLGeom2DCallback is transcribed in android/nuiosdl_gl.cpp (original 0x29d1a0).

void NuIOS_GetInAppProduct(i32, NuIOS_InAppProduct *) {
}

void NuMarkSceneAsZPrePass() {
}

void NuMemBlkCheckFreeList(numemblk_s *) {
}

void NuOnlineResetProfiles() {
}

void NuAnimDataChunkDestroy(nuanimdatachunk_s *) {
}

void NuAnimRelocatePtrsANI3(ani3_animheader_s *, i32) {
}

void NuGCutLocatorCalcMtx_3(NUGCUTLOCATOR_s *locator, numtx_s *mtx, float frame) {
    ani3_animheader_s *animation = reinterpret_cast<ani3_animheader_s *>(locator->animation);
    f32 *values = NuAnimCurveExtractAllNodeCurves_3(animation, 0, frame, NULL);
    if ((*animation->node_flags & 1) == 0) {
        NuMtxSetIdentity(mtx);
    } else {
        NUANGVEC angles = {
            static_cast<NUANG>(values[3] * 10430.378f),
            static_cast<NUANG>(values[4] * 10430.378f),
            static_cast<NUANG>(values[5] * 10430.378f),
        };
        NuMtxSetRotateXYZ(mtx, &angles);
    }
    NUVEC translation = {values[0], values[1], values[2]};
    NuMtxTranslate(mtx, &translation);
    mtx->m02 = -mtx->m02;
    mtx->m12 = -mtx->m12;
    mtx->m20 = -mtx->m20;
    mtx->m21 = -mtx->m21;
    mtx->m23 = -mtx->m23;
    mtx->m32 = -mtx->m32;
    NuMtxTranslate(mtx, reinterpret_cast<NUVEC *>(&locator->base_matrix.m30));
}

void NuIOSDLSkinMtxCallback(void *) {
}

void NuGCutCharAnimProcess_3(NUGCUTCHAR_s *, float, numtx_s *, i32 *, u32 *, float *, float *, float *, i32 *) {
}

i32 NuGCutLocatorIsVisble_3(NUGCUTLOCATOR_s *locator, float frame, float *scale, float *rate) {
    static char locator_scale_filter[] = {6, 7, 8, static_cast<char>(0xff)};
    static char locator_filter[] = {6, 7, static_cast<char>(0xff)};
    static char sfx_filter[] = {3, static_cast<char>(0xff)};
    ani3_animheader_s *animation = reinterpret_cast<ani3_animheader_s *>(locator->animation);
    char *filter = animation->curve_count == 4 ? sfx_filter
                                               : (animation->curve_count == 8 ? locator_filter : locator_scale_filter);
    f32 *values = NuAnimCurveExtractAllNodeCurves_3(animation, 0, frame, filter);
    if (animation->curve_count == 4) {
        f32 visible = values[3];
        return static_cast<i32>(visible < 0.0f ? visible - 0.5f : visible + 0.5f);
    }
    f32 visible = values[7];
    i32 result = static_cast<i32>(visible < 0.0f ? visible - 0.5f : visible + 0.5f);
    if (result != 0 && scale != NULL) {
        *scale = values[6];
    }
    if (rate != NULL) {
        *rate = animation->curve_count < 9 ? 1.0f : values[8];
    }
    return result;
}

extern "C" i32 NuGCutLocatorCalcMtx(NUGCUTLOCATOR_s *locator, float frame, NUMTX *mtx, nuanimtime_s *time) {
    if (locator->animation == NULL) {
        *mtx = locator->base_matrix;
        return 0;
    }
    if (*reinterpret_cast<u32 *>(locator->animation) + 0xbeb1b6ccU < 2) {
        NuGCutLocatorCalcMtx_3(locator, mtx, frame);
        return 0;
    }
    nuanimdata2_s *animation = locator->animation;
    auto evaluate = [&](u32 curve) {
        u8 type = animation->curve_types[curve];
        return type == 0 ? animation->curves[curve].data.constant
                         : NuAnimCurve2CalcValEx(&animation->curves[curve], time, type);
    };
    if ((*animation->node_flags & 1) == 0) {
        NuMtxSetIdentity(mtx);
    } else {
        NUANGVEC angles = {
            static_cast<NUANG>(evaluate(3) * 10430.378f),
            static_cast<NUANG>(evaluate(4) * 10430.378f),
            static_cast<NUANG>(evaluate(5) * 10430.378f),
        };
        NuMtxSetRotateXYZ(mtx, &angles);
    }
    NUVEC translation = {evaluate(0), evaluate(1), evaluate(2)};
    NuMtxTranslate(mtx, &translation);
    mtx->m02 = -mtx->m02;
    mtx->m12 = -mtx->m12;
    mtx->m20 = -mtx->m20;
    mtx->m21 = -mtx->m21;
    mtx->m23 = -mtx->m23;
    mtx->m32 = -mtx->m32;
    NuMtxTranslate(mtx, reinterpret_cast<NUVEC *>(&locator->base_matrix.m30));
    return 0;
}

extern "C" i32 NuGCutLocatorIsVisble(NUGCUTLOCATOR_s *locator, float frame, nuanimtime_s *time, float *scale,
                                     float *rate) {
    if (locator->animation == NULL) {
        if (scale != NULL && (locator->flags & 8) != 0) {
            *scale = locator->locator_scale;
        }
        return locator->flags & 8;
    }
    if (*reinterpret_cast<u32 *>(locator->animation) + 0xbeb1b6ccU < 2) {
        return NuGCutLocatorIsVisble_3(locator, frame, scale, rate);
    }
    nuanimdata2_s *animation = locator->animation;
    auto evaluate = [&](u32 curve) {
        u8 type = animation->curve_types[curve];
        return type == 0 ? animation->curves[curve].data.constant
                         : NuAnimCurve2CalcValEx(&animation->curves[curve], time, type);
    };
    if (rate != NULL) {
        *rate = 1.0f;
    }
    if (animation->curve_count == 4) {
        return static_cast<i32>(evaluate(3));
    }
    i32 visible = static_cast<i32>(evaluate(7));
    if (visible != 0 && scale != NULL) {
        *scale = evaluate(6);
    }
    return visible;
}

void NuIOS_GetPurchaseResult() {
}

void NuLightMotionBlurEffect(i32, float) {
}

void NuPortalEnableDebugDraw(i32) {
}

void NuTimeGetMicrosecondsPS(u32 *, u32 *) {
}

void NuIOSDLLightmapOffsetOld(void *) {
}

void NuIOS_DisplaySystemAlert(char const *) {
}

void NuIOS_IsProductPurchased(char *) {
}

void NuAnimGetAnimDataSizeANI3(ani3_animheader_s *) {
}

void NuGCutRigidForceInstanced(NUGCUTSCENE_s *) {
}

void NuIOSDLReflectionCallback(void *) {
}

void NuIOS_GetInAppProductByID(char *, NuIOS_InAppProduct *) {
}

void NuIOS_GetShaderProgramKey(ShaderObjectKey const &) {
}

void NuSpecialFindByPlatformID(nugscn_s *, nuhspecial_s *, i32) {
}

void NuAnimBuffEvaluate_3_QuatB(numtx_s *, nuanimbuff_s *, nugscn_s *, numtx_s *, ani3_animheader_s *,
                                void (*)(numtx_s *, void *, nuvec_s *, nuvec_s *, nuvec_s *, float), nuvec_s *,
                                void *) {
}

void NuDDSSetTextureDescription(char *, NUTEXFORMAT, i32, i32, i32, i32, nutexturetype_e) {
}

void NuIOSDLDeferredMtlCallback(void *) {
}

void NuIOS_GetNumInAppPurchases() {
}

void NuIOS_PurchaseInAppProduct(char *) {
}

void NuHGobjEvalAnimBlend2Root_3(nugscn_s *, ani3_animheader_s *, float, ani3_animheader_s *, float, float, i32,
                                 NUJOINTANIM_s *, numtx_s *,
                                 void (*)(numtx_s *, void *, nuvec_s *, nuvec_s *, nuvec_s *, float), void *) {
}

void NuIOSDLVertexGroupsCallback(void *) {
}

void NuIOS_CanMakeInAppPurchases() {
}

void NuIOS_RestoreInAppPurchases() {
}

void NuOnlineSetContextProfilePS(i32, i32, i32) {
}

void NuCameraTransformScissorClip(nuvec_s *, nuvec_s *, i32, numtx_s *) {
}

// NuDebrisRendererFlushBuffers is transcribed in android/nuptl_flush.cpp (original 0x296f35).

void NuDisplayListSetInstSurfGeom(void *) {
}

void NuIOSDLVertexOffsetsCallback(void *) {
}

void NuOnlineSetPropertyProfilePS(i32, i32, i32, void *) {
}

void NuIOS_CopyBackbufferToTexture(nunativetex_s *texture, bool) {
    texture->width = g_backingWidth;
    texture->height = g_backingHeight;
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nurenderthread.cpp", 0x128);
    glActiveTexture(GL_TEXTURE0);
    g_currentTexUnit = 0;
    glBindTexture(GL_TEXTURE_2D, texture->platform.gl_tex);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, g_backingWidth, g_backingHeight, 0);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nurenderthread.cpp", 299);
}

void NuIOS_IsProductPurchasedByNum(i32) {
}

void NuIOS_ResetVAODuplicateFinder() {
}

void NuFadeObjGetAngleTerrainValues(nuvec_s *) {
}

void NuIOS_CateInAppPurchaseManager() {
}

void NuDynamicLightingGetParameterfv(nudeferredshadingenum_e, float *) {
}

void NuIOS_GetInAppProductIdentifier(i32, NuIOS_InAppProduct *) {
}

void NuIOS_PurchaseInAppProductByNum(i32) {
}

void NuIOSDLDeferredTransformCallback(void *) {
}

void NuIOS_AreInAppPurchasesAvailable() {
}

void NuIOS_CreateGLTexFromPVRInMemory(void *, i32 *, i32 *) {
}

void NuOnlineSetPresenceModeProfilePS(i32, i32) {
}

void NuDynamicLightTestShadowExtrusions(nudynamiclight_s *, _vuv_s const *, _vuv_s const *, i32 *) {
}

void NuOnlineAchievementAchievedProfile(i32, i32, void (*)(i32, i32)) {
}

void NuOnlineSetDefaultContextProfilePS(i32, i32, i32) {
}

void NuRenderContextForceSamplerStatePS(i32, d3dsamplerstate_u const *) {
}

void NuGCutSceneRemapFocusIdToLocaterNum(NUGCUTSCENE_s *, variptr_u *) {
}

void NuIOSDLDeferredTransformParamsCallback(void *) {
}

void NuOnlineSetDefaultPresenceModeProfilePS(i32, i32) {
}

void NuATanf(float) {
}

void NuATan2f(float, float) {
}

void NuFntSave(nufnt_s *, i32, char *) {
}

void NuLgtRand() {
}

void NuMemFree(void *) {
}

void NuStrCatC(char *, char) {
}

NuDynamicLight::RenderSet::RenderSet() {
}

void NuMemory::MemErrorHandler::CloseDump(NuMemoryManager *, u32) {
}

void NuMemory::MemErrorHandler::Dump(NuMemoryManager *, u32, char const *) {
}

void NuMemory::MemErrorHandler::HandleError(NuMemoryManager *, NuMemoryManager::ErrorCode, char const *) {
}

void NuMemory::MemErrorHandler::OpenDump(NuMemoryManager *, char const *, u32 &) {
}

void NuNetEmu::EmuPacket::AddPayload(void *, i32) {
}

NuNetEmu::EmuPacket::EmuPacket(nunetaddr_s *) {
}

NuNetEmu::EmuPacket::~EmuPacket() {
}

void NuNetEmu::PackStats::Draw(float, float, float, float, NetSmallStats::eInfo) const {
}

static void NuErrorFunction(char *, ...) {
}

static void NuWarningFunction(char *, ...) {
}

static void NuDebugMsgFunction(char *, ...) {
}

static i32 NuDebugMsgFunctionTTY(i32, char *, ...) {
    return 0;
}
