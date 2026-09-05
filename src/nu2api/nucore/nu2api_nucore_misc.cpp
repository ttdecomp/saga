#include "nu2api_nucore_types.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nuanim3.h"

#include <GLES2/gl2.h>
#include <string.h>

extern "C" void BeginCriticalSectionGL(const char *, i32);
extern "C" void EndCriticalSectionGL(const char *, i32);
extern i32 bgProcIsBgThread(void);

extern "C" f32 *NuAnimCurveExtractAllNodeCurves_3(ani3_animheader_s *, i32, f32, char *);
extern "C" void NuAnimData2CalcTime(nuanimdata2_s *, f32, nuanimtime_s *);
extern "C" f32 NuAnimCurve2CalcValEx(nuanimcurve2_s *, nuanimtime_s *, u32);
void NuGCutRigidCalcMtx_3(NUGCUTRIGID_s *, f32, numtx_s *);
extern void *globalbuffer;
extern i32 MaxAnimJoints;

extern "C" void NuAnimBuffCreateScratch(nuanimbuff_s *buffer);
extern "C" void NuAnimBuffDestroyScratch(nuanimbuff_s *buffer);
extern "C" void NuAnimBuffAccumulate_3(nuanimbuff_s *buffer, ani3_animheader_s *animation, f32 time, i32 overwrite,
                                       f32 blend, i32 first_joint, nuhgobj_s *object, NUVEC *root_translation);
extern "C" void NuAnimBuffEvaluate_3(nuanimbuff_s *buffer, nuhgobj_s *object, NUMTX *matrices,
                                     ani3_animheader_s *animation, NUHGOBJROOTFN root_fn, NUVEC *root_translation,
                                     void *root_data);

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

void NuWarningPrint(char *) {
}

void NuASin_Accurate(float) {
}

void NuBez3EvaluateX(nuvec4_s *, float) {
}

void NuDebugMsgPrint(char *) {
}

// original 0x2955ee -- lightmap display-list packet.  Mode 1 installs one
// lightmap; mode 2 walks the packet's three lightmap ids.  The latter really
// does select texture unit zero for each entry in the target binary.
__attribute__((optimize("O0"))) void NuIOSDLLightmap(void *arg) {
    i32 *packet = static_cast<i32 *>(arg);
    const i32 mode = packet[0];

    if (mode == 1) {
        const i32 texture_id = packet[1] > 0 ? packet[1] : 1;
        NUNATIVETEX *texture = NuTexGetNative(texture_id);
        glActiveTexture(GL_TEXTURE0);
        g_currentTexUnit = 0;
        glBindTexture(GL_TEXTURE_2D, texture->platform.gl_tex != 0 ? texture->platform.gl_tex : g_whiteTexture);
    } else if (mode == 2) {
        for (i32 index = 0; index < 3; ++index) {
            const i32 texture_id = packet[index + 2] > 0 ? packet[index + 2] : 1;
            NUNATIVETEX *texture = NuTexGetNative(texture_id);
            glActiveTexture(GL_TEXTURE0);
            g_currentTexUnit = 0;
            glBindTexture(GL_TEXTURE_2D, texture->platform.gl_tex != 0 ? texture->platform.gl_tex : g_whiteTexture);
        }
    } else {
        return;
    }

    const f32 *offset = reinterpret_cast<const f32 *>(packet + 5);
    const f32 shader_offset[4] = {offset[0], -offset[1], 0.0f, 0.0f};
    NuShaderManagerSetfv(0x58, shader_offset);
}

void NuLgtArcLaserEx(i32, nuvec_s *, nuvec_s *, nuvec_s *, float, float, float, float, i32, i32) {
}

void NuMemValidateFn() {
}

void NuVpSetDestRect(float, float, float, float) {
}

void NuBrdigeDrawRope(numtl_s *, nuvec_s *, nuvec_s *, i32, i32 *, i32) {
}

void NuDDSGetMipLevel(i32, i32, i32, NUTEXFORMAT, i32, bool, i32, i32, i32 &, i32 &, i32 &) {
}

void NuHtmlGraphArray(char **) {
}

void NuLgtSetArcMatEx(i32, numtl_s *, float, float, float, float) {
}

void NuMemGetExternal() {
}

extern "C" u8 CutSceneBoundingBoxTrackRoot;

// Original @0x2da1a0. The public type is nugscn_s in the mangled symbol, but
// GHG objects carry the hierarchy layout consumed here.
i32 NuCameraClipHGobj(nugscn_s *scene, numtx_s *world_matrix, numtx_s *root_matrix) {
    nuhgobj_s *object = reinterpret_cast<nuhgobj_s *>(scene);
    if (CutSceneBoundingBoxTrackRoot == 0) {
        return NuCameraClipTestExtents(&object->bounds_min, &object->bounds_max, world_matrix, 0.0f, 0);
    }

    NUVEC half_extents;
    NuVecSub(&half_extents, &object->bounds_max, &object->bounds_min);
    NuVecScale(&half_extents, &half_extents, 0.5f);
    NUVEC min;
    NuVecNeg(&min, &half_extents);

    NUMTX translated_world = *world_matrix;
    NuMtxPreTranslate(&translated_world, NUMTX_GET_ROW_VEC(root_matrix, 3));
    return NuCameraClipTestExtents(&min, &half_extents, &translated_world, 0.0f, 0);
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

// original 0x295420 -- legacy packet containing three texture ids.
__attribute__((optimize("O0"))) void NuIOSDLLightmapOld(void *arg) {
    const i32 *texture_ids = static_cast<const i32 *>(arg);
    for (i32 index = 0; index < 3; ++index) {
        const i32 texture_id = texture_ids[index] > 0 ? texture_ids[index] : 1;
        NUNATIVETEX *texture = NuTexGetNative(texture_id);
        glActiveTexture(GL_TEXTURE0 + index);
        g_currentTexUnit = index;
        glBindTexture(GL_TEXTURE_2D, texture->platform.gl_tex != 0 ? texture->platform.gl_tex : g_whiteTexture);
    }

    const f32 shader_offset[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    NuShaderManagerSetfv(0x58, shader_offset);
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

extern NUVEC *override_campos;

void NuPortalSetOverride(NUVEC *position) {
    override_campos = position;
}

void NuSpecialGetAnimPos(nuhspecial_s *) {
}

void NuSpecialReflection(i32) {
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

void NuMemBlkCheckFreeList(numemblk_s *) {
}

extern "C" i32 g_signedinUser;

void NuOnlineResetProfiles() {
    g_signedinUser = -1;
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

// Original 0x294764. The skin packet begins with the number of palette
// matrices followed by their contiguous 4x4 values.
void __attribute__((weak)) NuIOSDLSkinMtxCallback(void *data) {
    i32 *packet = static_cast<i32 *>(data);
    const i32 matrix_count = *packet++;
    NUSHADEROBJECT *shader = NuShaderManagerGetCurrentShader();
    if (shader != NULL) {
        NuShaderObjectSetElementsfv(shader, 0x5a, 0, matrix_count * 4, reinterpret_cast<const f32 *>(packet));
    }
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

extern i32 draw_portals;

void NuPortalEnableDebugDraw(i32 enabled) {
    draw_portals = enabled;
}

void NuTimeGetMicrosecondsPS(u32 *, u32 *) {
}

// original 0x2954f0 -- legacy three-lightmap packet followed by a UV offset.
__attribute__((optimize("O0"))) void NuIOSDLLightmapOffsetOld(void *arg) {
    const i32 *texture_ids = static_cast<const i32 *>(arg);
    for (i32 index = 0; index < 3; ++index) {
        const i32 texture_id = texture_ids[index] > 0 ? texture_ids[index] : 1;
        NUNATIVETEX *texture = NuTexGetNative(texture_id);
        glActiveTexture(GL_TEXTURE0 + index);
        g_currentTexUnit = index;
        glBindTexture(GL_TEXTURE_2D, texture->platform.gl_tex != 0 ? texture->platform.gl_tex : g_whiteTexture);
    }

    const f32 *offset = reinterpret_cast<const f32 *>(texture_ids + 3);
    const f32 shader_offset[4] = {offset[0], -offset[1], 0.0f, 0.0f};
    NuShaderManagerSetfv(0x58, shader_offset);
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

void NuIOS_GetNumInAppPurchases() {
}

void NuIOS_PurchaseInAppProduct(char *) {
}

// Original @0x2ce760.
void NuHGobjEvalAnimBlend2Root_3(nugscn_s *scene, ani3_animheader_s *animation_a, f32 time_a,
                                 ani3_animheader_s *animation_b, f32 time_b, f32 blend, i32 override_count,
                                 NUJOINTANIM_s *overrides, NUMTX *matrices, NUHGOBJROOTFN root_fn, void *root_data) {
    nuhgobj_s *object = reinterpret_cast<nuhgobj_s *>(scene);
    nuanimbuff_s buffer;
    NUVEC root_a = {0.0f, 0.0f, 0.0f};
    NUVEC root_b = {0.0f, 0.0f, 0.0f};
    NUVEC root_translation = {0.0f, 0.0f, 0.0f};

    NuAnimBuffCreateScratch(&buffer);
    NuAnimBuffAccumulate_3(&buffer, animation_a, time_a, 1, 0.0f, 0, object, &root_a);
    NuAnimBuffAccumulate_3(&buffer, animation_b, time_b, 0, blend, 0, object, &root_b);
    if (override_count != 0 && JointProcAnimFn != NULL) {
        JointProcAnimFn(&buffer, object, override_count, overrides);
    }
    NuAnimBuffEvaluate_3(&buffer, object, matrices, animation_a, root_fn, &root_translation, root_data);
    NuAnimBuffDestroyScratch(&buffer);
}

void NuIOSDLVertexGroupsCallback(void *) {
}

i32 NuIOS_CanMakeInAppPurchases() {
    return 0;
}

void NuIOS_RestoreInAppPurchases() {
}

void NuOnlineSetContextProfilePS(i32, i32, i32) {
}

void NuCameraTransformScissorClip(nuvec_s *, nuvec_s *, i32, numtx_s *) {
}

// NuDebrisRendererFlushBuffers is transcribed in android/nuptl_flush.cpp (original 0x296f35).

// Original 0x294d93. The packet stores a count followed by up to eight vec4
// vertex-offset entries for semantic 0x50.
void NuIOSDLVertexOffsetsCallback(void *arg) {
    const i32 *packet = static_cast<const i32 *>(arg);
    i32 count = packet[0];
    if (count > 8) {
        count = 8;
    }
    NuShaderManagerSetElementsfv(0x50, 0, count, reinterpret_cast<const f32 *>(packet + 1));
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

static i32 g_vaoRecordCount asm("_ZL16g_vaoRecordCount");

void NuIOS_ResetVAODuplicateFinder() {
    g_vaoRecordCount = 0;
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

i32 NuIOS_AreInAppPurchasesAvailable() {
    return 0;
}

GLuint NuIOS_CreateGLTexFromPVRInMemory(void *data, i32 *out_width, i32 *out_height) {
    extern i32 comeFromHash;
    extern i32 g_loadDefaultTexture;
    extern i32 g_loadingCharacterInHub;

    static const GLenum cube_faces[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };

    comeFromHash = 0;

    u8 *header = (u8 *)data;
    u8 *pixels = header + 0x34 + *(u32 *)(header + 0x30);
    const u32 pixel_format = *(u32 *)(header + 0x08);
    const u32 channel_bits = *(u32 *)(header + 0x0c);
    const i32 height = *(i32 *)(header + 0x18);
    const i32 width = *(i32 *)(header + 0x1c);
    const u32 depth = *(u32 *)(header + 0x20);
    const u32 surfaces = *(u32 *)(header + 0x24);
    const u32 faces = *(u32 *)(header + 0x28);
    const u32 mip_count = *(u32 *)(header + 0x2c);

    GLenum internal_format = 0;
    GLenum format = 0;
    GLenum type = 0;
    i32 bits_per_pixel = 0;
    bool compressed = false;

    if (channel_bits == 0) {
        static const GLenum compressed_formats[6] = {
            0x8c01, // GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
            0x8c03, // GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
            0x8c00, // GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
            0x8c02, // GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
            0x9137, // GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG
            0x9138, // GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG
        };
        if (pixel_format >= 6) {
            return 0;
        }
        internal_format = compressed_formats[pixel_format];
        bits_per_pixel = pixel_format < 2 ? 2 : 4;
        compressed = true;
    } else if (pixel_format == 0x61626772 && channel_bits == 0x08080808) { // "rgba", 8 bits each
        internal_format = GL_RGBA;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        bits_per_pixel = 32;
    } else if (pixel_format == 0x61626772 && channel_bits == 0x04040404) { // "rgba", 4 bits each
        internal_format = GL_RGBA;
        format = GL_RGBA;
        type = GL_UNSIGNED_SHORT_4_4_4_4;
        bits_per_pixel = 16;
    }

    if (out_width != NULL) {
        *out_width = width;
    }
    if (out_height != NULL) {
        *out_height = height;
    }

    const GLenum texture_target = faces > 1 ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
    GLuint texture = 0;
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 0x2e7);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    g_currentTexUnit = 0;
    glBindTexture(texture_target, texture);
    if (faces > 1) {
        g_lastBoundCubeTexIds[0] = texture;
    }
    glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, mip_count < 2 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (faces > 1) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 0x2fe);

    if (g_loadingCharacterInHub != 0 && bgProcIsBgThread() != 0) {
        NuIOS_YieldThread();
    }

    usize offset = 0;
    for (u32 mip = 0; mip < mip_count; ++mip) {
        const i32 mip_width = width >> mip;
        const i32 mip_height = height >> mip;
        usize mip_size = (usize)mip_width * (usize)mip_height * (usize)bits_per_pixel / 8;
        if (mip_size < 0x20) {
            mip_size = 0x20;
        }

        for (u32 surface = 0; surface < surfaces; ++surface) {
            for (u32 face = 0; face < faces; ++face) {
                const GLenum target = faces > 1 ? cube_faces[face] : GL_TEXTURE_2D;
                for (u32 z = 0; z < depth; ++z) {
                    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",
                                           0x31d);
                    glActiveTexture(GL_TEXTURE0);
                    g_currentTexUnit = 0;
                    glBindTexture(texture_target, texture);

                    if (g_loadDefaultTexture == 0) {
                        if (compressed) {
                            glCompressedTexImage2D(target, mip, internal_format, mip_width, mip_height, 0, mip_size,
                                                   pixels + offset);
                        } else {
                            glTexImage2D(target, mip, internal_format, mip_width, mip_height, 0, format, type,
                                         pixels + offset);
                        }
                    } else {
                        loadDefaultTexture(texture, mip, mip_width, texture_target, target);
                    }
                    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",
                                         0x341);

                    if (bgProcIsBgThread() != 0) {
                        NuIOS_YieldThread();
                    }
                    offset += mip_size;
                }
            }
        }
    }

    return texture;
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

static __used__ void NuErrorFunction(char *, ...) {
}

static __used__ void NuWarningFunction(char *, ...) {
}

static __used__ void NuDebugMsgFunction(char *, ...) {
}

static __used__ i32 NuDebugMsgFunctionTTY(i32, char *, ...) {
    return 0;
}
