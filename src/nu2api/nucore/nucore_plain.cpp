// Nucore plain — C-linkage surface for the original libTTapp.so nucore TU.
//
// This file provides the C-callable export table that the original binary
// exposes from its single large nucore translation unit. Every symbol below
// is present in the ELF dynamic symbol table; the few with non-trivial
// bodies are faithful transcriptions (original addresses cited inline) while
// the remainder are pending-transcription stand-ins that preserve link
// compatibility until their real bodies land in a domain file.
//
// Faithfully transcribed in this TU:
//   NuDisplayListInit              @0x29ad60 — anchors the static 2D list
//   NuDisplayListLinkItems         @0x29ae31 — appends N items + NEXT term.
//   NuDisplayListLinkMtl           @0x2e8cc0 — minimal 2D-path mtl link
//   NuCameraSet                    via NuCameraSetEx(cam,0) (matrix work stubbed)
//   NuIOS_GetAspectRatio           inline ratio from nuapi screen dims
//   NuIOS_GetDeviceLanguage        @0xe3640  — exact locale ladder
//   NuFrameEnd                     @0x2e??? — frame pacing + swap + pad tick
//   NuStringFilterLoad             @0x???  — no-op stub (host loads elsewhere)
// Transcribed elsewhere (stub removed here, comment left as breadcrumb):
//   Nu360_dxClear                  → nuposteffect_plain.cpp  @0x317070
//   NuDisplayListSwapBuffers*      → nudlist.cpp             @0x2eb5d0/0x2eaef0
//   NuHtmlBegin                    → legoapi/misc/supportall @0x2d5ca0
//   NuIOS_SetVertexFormat          → nuiosdl_gl.cpp          @0x29c070
//   NuIOS_Wait/WakeRenderThread    → ios_graphics.cpp        @0xe34b0/0xe3590
//   NuRenderContextSetZFunc        → nuiosdl_gl.cpp          @0x2a3860
//
// The remaining ~350 symbols are grouped by subsystem below so a reader can
// tell at a glance which domain still needs decompilation. Each group is
// alphabetical; every stub is an empty body that matches the original linkage.

#include <string.h>

#include "decomp.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/numath/numtx.h"
#include "globals.h"

struct nuhspecial_s;

namespace {
    struct NuSpecialHandleLayout {
        NUGSCN *scene;
        void *special;
        void *display_special;
    };

    struct NuLegacySpecialLayout {
        u8 pad_00[0x40];
        u8 *instance;
        u32 flags;
    };

    struct NuDisplaySpecialLayout {
        NUMTX mtx;
        NUMTX draw_mtx;
        NUVEC min;
        f32 min_w;
        NUVEC max;
        f32 max_w;
        u8 pad_a0[0x10];
        NUCLIPOBJECT *clip_objects;
        char *name;
        u32 flags;
        f32 *clip_range;
        i32 instance_ix;
        NUMTX *draw_mtx_ptr;
        i16 wind_speed;
        i16 wind_scale;
        u32 pad_cc;
    };

    static i32 nuspecial_clip_state = -1;
} // namespace

extern "C" {
    i32 nuspecial_const_tint_enabled;
    NUCOLOUR3 nuspecial_const_tint = {1.0f, 1.0f, 1.0f};
    i32 nuspecial_const_alpha_enabled;
    f32 nuspecial_const_alpha = 1.0f;
    extern NUGLOBALRNDRSTATE render_state;
    void RndrStateSetConstAlphaTint(i32 alpha_enabled, i32 tint_enabled, f32 alpha, const NUCOLOUR3 *tint, NUMTL *mtl);
}

// C++-linkage helpers defined in sibling TUs.
void DisplayListCreateDynMtlList(VARIPTR *buf, VARIPTR buf_end); // supportall.cpp
void NuPadRecordEndFrame(void);                                  // nupad_interface.cpp
void bgSuspendMain(i32);                                         // main.cpp

extern "C" {

    // ---------------------------------------------------------------------------
    // Display-list bootstrap (original nucore TU file-statics)
    // ---------------------------------------------------------------------------

    void NuHasError(void);
    void NuMtlAnimate(void);
    void NuTexAnimProcess(void);
    void NuWindAnimate(void);
    void NuTimeBarSetRender(void);
    void NuRndrSwapScreenEx(i32 mode, void (*callback)(void));
    void NuShaderManagerSetfv(i32 semantic, const f32 *values);

    extern VARIPTR *display_list_buffer_end;
    extern VARIPTR rndrstream_free;
    extern VARIPTR rndrstream_end;

    i32 NuThreadCreateCriticalSection(void);

    // Shared with the nudlist TU (original file-static in this TU).
    static void nudlist_SetNext(nudisplaylistitem_s *item, void *next) {
        item->next = next;
    }

    void NuDisplayListResetBuffer(void);

    // The static 2D display list's stream-area base lives at manager+0x4C8
    // (nudisplaylist_s+0x10 of the embedded 2D list at manager+0x4B8) and points
    // at manager+0x4FC.
    static const usize NUDLIST_2D_STREAM_BASE_OFFSET = 0x4C8;
    static const usize NUDLIST_2D_STREAM_AREA_OFFSET = 0x4FC;
    static const usize NUDLIST_2D_CRITSEC_OFFSET = 0x5EC;

    // original 0x29ad60
    void NuDisplayListInit(VARIPTR *buf, VARIPTR *buf_end) {
        u8 *mgr = (u8 *)&global_dlist_manager;
        // The static 2D list starts anchored on the stream-head sentinel.
        // DisplayListCreateDynMtlList initialises the sentinel and mtl_last.
        *(u8 **)(mgr + NUDLIST_2D_STREAM_BASE_OFFSET) = mgr + NUDLIST_2D_STREAM_AREA_OFFSET;

        DisplayListCreateDynMtlList(buf, *buf_end);
        NuDisplayListResetBuffer();

        *(i32 *)(mgr + NUDLIST_2D_CRITSEC_OFFSET) = NuThreadCreateCriticalSection();
    }

    // ---------------------------------------------------------------------------
    // Redirected symbols — real bodies live elsewhere (kept as comments)
    // ---------------------------------------------------------------------------

    // Nu360_dxClear(u32,u32) is transcribed in nuposteffect_plain.cpp (original 0x317070).
    // NuDisplayListSwapBuffersBeginFrame / EndFrame are transcribed in full
    // in nudlist.cpp (originals 0x2eb5d0 / 0x2eaef0).
    // NuHtmlBegin(void*) is transcribed in legoapi/misc/supportall.cpp (original 0x2d5ca0).
    // NuIOS_SetVertexFormat is transcribed in nuiosdl_gl.cpp (original 0x29c070).
    // NuIOS_WaitForRenderThreadCompletion is transcribed in ios_graphics.cpp (original 0xe34b0).
    // NuIOS_WakeRenderThread is transcribed in ios_graphics.cpp (original 0xe3590).
    // NuRenderContextSetZFunc is transcribed in nuiosdl_gl.cpp (original 0x2a3860).

    // ---------------------------------------------------------------------------
    // Camera (thin wrappers; full matrix work is in NuCameraSetEx)
    // ---------------------------------------------------------------------------

    void NuCameraCalcAperture(void) {
    }
    void NuCameraCalcClipMtx(void) {
    }
    void NuCameraCalcRootFStop(void) {
    }
    void NuCameraClearStateBuffer(void) {
    }
    void NuCameraClipTestExtentsAxisAligned(void) {
    }
    void NuCameraClipTestExtentsGeneric(void) {
    }
    void NuCameraClipTestPointScissor(void) {
    }
    void NuCameraClipTestPointVport(void) {
    }
    void NuCameraClipTestPoints(void) {
    }
    void NuCameraFOVToFocalLen(void) {
    }
    void NuCameraFocalLenToFOV(void) {
    }
    void NuCameraForceFarclip(void) {
    }
    void NuCameraGetAxes(void) {
    }
    void NuCameraGetClippingMtx(void) {
    }
    void NuCameraGetClippingRatios(void) {
    }
    void NuCameraGetPCMtx(void) {
    }
    void NuCameraGetPCSMtx(void) {
    }
    void NuCameraGetVPCSMtx(void) {
    }
    void NuCameraIntersectsAABB(void) {
    }
    void NuCameraLock(void) {
    }
    void NuCameraMotionBlurEffect(void) {
    }
    void NuCameraMotionBlurParams(void) {
    }
    void NuCameraRelock(void) {
    }
    void NuCameraRestoreState(void) {
    }
    void NuCameraSaveState(void) {
    }
    void NuCameraSet(NUCAMERA *cam) {
        NuCameraSetEx(cam, 0);
    }
    void NuCameraSetAxes(void) {
    }
    void NuCameraSetEx(NUCAMERA *cam, i32 fast) {
        global_camera = *cam;

        if (fast == 0) {
            NuVpUpdate();
        }

        NuMtxInv(&vmtx, &global_camera.mtx);
        NuMtxScale(&vmtx, &global_camera.scale);

        if (fast == 0) {
            NuCameraSetProjectionMtx(&pmtx, global_camera.fov, global_camera.aspect, global_camera.near_clip,
                                     global_camera.far_clip);
            pmtx.m00 *= global_camera.unknown_58;
            pmtx.m11 *= global_camera.unknown_5c;
            pmtx.m20 += global_camera.unknown_50;
            pmtx.m21 += global_camera.unknown_54;
        }

        NuMtxMulH(&vpmtx, &vmtx, &pmtx);

        if (fast == 0) {
            i32 angle = static_cast<i32>(global_camera.fov * 0.5f * 10430.378f);
            clip_planes.m22 = NuTrigTable[angle >> 1 & 0x7fff];
            clip_planes.m12 = NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff];
            zy = clip_planes.m22 / clip_planes.m12;
            zx = zy / global_camera.aspect;

            // PS2_SREZ_W/H are 4096; PS2_REZ_W/H track the current render
            // dimensions. These ratios define the scissor frustum.
            zxs = nurndr_pixel_width != 0 ? 4096.0f * zx / static_cast<f32>(nurndr_pixel_width) : zx;
            zys = nurndr_pixel_height != 0 ? 4096.0f * zy / static_cast<f32>(nurndr_pixel_height) : zy;

            clip_planes.m13 = -clip_planes.m12;
            clip_planes.m02 = 0.0f;
            clip_planes.m03 = 0.0f;
            clip_planes.m32 = 0.0f;
            clip_planes.m33 = 0.0f;
            clip_planes.m23 = clip_planes.m22;

            f32 side = zy / cam->aspect;
            clip_planes.m01 = 1.0f / NuFsqrt(side * side + 1.0f);
            clip_planes.m00 = -clip_planes.m01;
            clip_planes.m10 = 0.0f;
            clip_planes.m11 = 0.0f;
            clip_planes.m20 = side * clip_planes.m01;
            clip_planes.m21 = clip_planes.m20;
            clip_planes.m30 = 0.0f;
            clip_planes.m31 = 0.0f;
            NuCameraBuildClipPlanes();
        }

        NuRndrSetViewMtx(&vpmtx, &vpc_vport_mtx, &vpc_sci_mtx);
        NuRndrStateUpdateCameraState();
    }
    void NuCameraSetProjectionMtx(NUMTX *mtx, f32 fov, f32 aspect, f32 near_clip, f32 far_clip) {
        if (near_clip < 0.1f) {
            near_clip = 0.1f;
        }
        i32 angle = (i32)(fov * 0.5f * 10430.378f);
        f32 cotangent = NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff] / NuTrigTable[angle >> 1 & 0x7fff];
        f32 depth = far_clip / (far_clip - near_clip);
        memset(mtx, 0, sizeof(*mtx));
        mtx->m00 = aspect * cotangent;
        mtx->m11 = cotangent;
        mtx->m22 = depth;
        mtx->m23 = 1.0f;
        mtx->m32 = -depth * near_clip;
    }
    void NuCameraSetReflect(void) {
    }
    void NuCameraSetScissorClipMtx(void) {
    }
    void NuCameraSetVPortClipMtx(void) {
    }
    void NuCameraTransformScreen(void) {
    }
    void NuCameraTransformScreenClip(void) {
    }
    void NuCameraTransformScreenVU0(void) {
    }
    void NuCameraTransformView(void) {
    }
    void NuCameraUnlock(void) {
    }

    // ---------------------------------------------------------------------------
    // Display list — faithful helpers + pending stubs
    // ---------------------------------------------------------------------------

    void NuDisplayListAnimateMtls(void) {
    }
    void NuDisplayListBeginCriticalSection(void) {
    }
    void NuDisplayListBurstRndrSpecial(void) {
    }
    void NuDisplayListCaptureBegin(void) {
    }
    void NuDisplayListCaptureEnd(void) {
    }
    void NuDisplayListClipSpecials(void) {
    }
    void NuDisplayListCreateFx(void) {
    }
    void NuDisplayListCreateMtl(void) {
    }
    void NuDisplayListDebugToFile(void) {
    }
    void NuDisplayListDestroyFx(void) {
    }
    void NuDisplayListDestroyMtl(void) {
    }
    void NuDisplayListDraw(void) {
    }
    void NuDisplayListDrawAll(void) {
    }
    void NuDisplayListEndCriticalSection(void) {
    }
    void NuDisplayListEndScene(void) {
    }
    VARIPTR *NuDisplayListLinkItemVP(nudisplaylist_s *list, u8 type, void *call_addr, VARIPTR *buf) {
        auto *call = reinterpret_cast<nudisplaylistitem_s *>(buf->void_ptr);
        list->mtl_last->next = call;
        call->type = type;
        call->id = 3;
        call->next = call_addr != nullptr ? call_addr : call + 2;

        auto *next = call + 1;
        next->type = 0x8d;
        next->id = 1;
        next->next = list->dyn_geom + 1;
        list->mtl_last = next;
        buf->addr += sizeof(nudisplaylistitem_s) * 2;
        return call_addr != nullptr ? nullptr : buf;
    }
    void NuDisplayListLinkItem(nudisplaylist_s *list, u8 type, void *call_addr) {
        NuDisplayListLinkItemVP(list, type, call_addr, NuDisplayListGetBuffer());
    }
    // Append `count` item slots from the shared stream buffer, then a NEXT
    // terminator. Original 0x29ae31.
    VARIPTR *NuDisplayListLinkItems(nudisplaylist_s *list, i32 count) {
        VARIPTR *buf = NuDisplayListGetBuffer();
        nudlist_SetNext(list->mtl_last, buf->void_ptr);
        list->items = reinterpret_cast<nudisplaylistitem_s *>(buf->void_ptr);
        buf->addr += count * sizeof(nudisplaylistitem_s);

        auto *terminator = reinterpret_cast<nudisplaylistitem_s *>(buf->void_ptr);
        terminator->type = 0x8d;
        terminator->id = 1;
        nudlist_SetNext(terminator, list->dyn_geom + 1);
        list->mtl_last = terminator;
        buf->addr += 0x10;
        return buf;
    }
} // extern "C"

// Local helpers matching original static display-list setters (t local symbols)

static __attribute__((used)) void NuDisplayListSetID(nudisplaylistitem_s *item, unsigned char id) {
    item->id = id;
}
static __attribute__((used)) void NuDisplayListAddItem(nudisplaylist_s *list, unsigned char id, void *item) {
    (void)list;
    (void)id;
    (void)item;
}
static __attribute__((used)) void NuDisplayListSetItem(nudisplaylistitem_s *item, unsigned char a, unsigned char b,
                                                       void *c) {
    (void)item;
    (void)a;
    (void)b;
    (void)c;
}
static __attribute__((used)) void NuDisplayListSetNext(nudisplaylistitem_s *item, void *next) {
    item->next = next;
}
static __attribute__((used)) void NuDisplayListSetID_CNT(nudisplaylistitem_s *item) {
    (void)item;
}
static __attribute__((used)) void NuDisplayListSetID_RET(nudisplaylistitem_s *item) {
    (void)item;
}
static __attribute__((used)) void NuDisplayListSetID_CALL(nudisplaylistitem_s *item) {
    (void)item;
}
static __attribute__((used)) void NuDisplayListSetID_NEXT(nudisplaylistitem_s *item) {
    (void)item;
}

extern "C" {
    // original 0x2e8cc0
    void NuDisplayListLinkMtl(nudisplaylist_s *list, NUMTL *mtl) {
        if (list->state->mtl == mtl) {
            return;
        }

        VARIPTR *buf = NuDisplayListGetBuffer();
        buf->addr = ALIGN(buf->addr, 0x10);
        list->mtl_last->next = buf->void_ptr;

        nudisplaylistitem_s *item;
        if (mtl->tex_id < 1 || mtl->tex_id == list->state->tex_id) {
            item = reinterpret_cast<nudisplaylistitem_s *>(buf->void_ptr);
            buf->addr += 0x40;
        } else {
            auto *bytes = reinterpret_cast<u8 *>(buf->void_ptr);
            buf->addr += 0x60;

            auto *nop = reinterpret_cast<nudisplaylistitem_s *>(bytes);
            nop[0].type = 0x87;
            nop[0].id = 0;
            nop[0].next = nullptr;
            nop[1].type = 0x87;
            nop[1].id = 0;
            nop[1].next = nullptr;
            item = nop + 2;
            list->state->tex_id = mtl->tex_id;
        }

        item[0].type = 0x80;
        item[0].id = 3;
        item[0].next = mtl;
        item[1].type = 0x87;
        item[1].id = 0;
        item[1].next = nullptr;
        item[2].type = 0x87;
        item[2].id = 0;
        item[2].next = nullptr;
        item[3].type = 0x8d;
        item[3].id = 1;
        item[3].next = nullptr;

        list->mtl_last = item + 3;
        list->state->mtl = mtl;
    }
    void NuDisplayListLinkList(void) {
    }
    void NuDisplayListPrepareFaceonPS(void) {
    }
    void *DisplayListCreateGeomTransformPS(VARIPTR *buffer, NUMTX *transform, NUMTL *mtl, void *next, void *tx);

    i32 NuDisplayListRndrSpecial(nuhspecial_s *special_handle, NUMTX *mtx, i32 skinned, void *skin_mtx,
                                 void *blend_values) {
        (void)skinned;
        (void)skin_mtx;
        (void)blend_values;

        if (special_handle == NULL || mtx == NULL) {
            return 0;
        }

        NuSpecialHandleLayout *handle = reinterpret_cast<NuSpecialHandleLayout *>(special_handle);
        NuDisplaySpecialLayout *special = static_cast<NuDisplaySpecialLayout *>(handle->display_special);
        if (handle->scene == NULL || special == NULL) {
            return 0;
        }

        NUDLDLISTSCENE *scene = reinterpret_cast<NUDLDLISTSCENE *>(handle->scene->display_list);
        i32 clip_state = nuspecial_clip_state;
        if (clip_state == -1) {
            clip_state = NuCameraClipTestExtents(&special->min, &special->max, mtx, 0.0f, 0);
        }
        if (clip_state == 0) {
            return 0;
        }

        NUCLIPOBJECT *clip_object = special->clip_objects;
        if (special->clip_range != NULL && special->clip_range[0] != 0.0f) {
            NUVEC center;
            center.x = (special->min.x + special->max.x) * 0.5f;
            center.y = (special->min.y + special->max.y) * 0.5f;
            center.z = (special->min.z + special->max.z) * 0.5f;
            NuVecMtxTransform(&center, &center, mtx);
            f32 distance = NuCameraDistSqr(&center);
            if (distance < 0.0f) {
                distance = 0.0f;
            }
            i32 lod = 0;
            while (distance < special->clip_range[lod]) {
                ++lod;
            }
            clip_object += lod;
        }

        f32 alpha = nuspecial_const_alpha_enabled != 0 ? nuspecial_const_alpha : 1.0f;
        for (u32 i = 0; i < *reinterpret_cast<u32 *>(clip_object); ++i) {
            u32 *material_indices = *reinterpret_cast<u32 **>(reinterpret_cast<u8 *>(clip_object) + 4);
            i32 *item_indices = *reinterpret_cast<i32 **>(reinterpret_cast<u8 *>(clip_object) + 8);
            u32 material_index = material_indices[i];
            NUDISPLAYLISTITEM *geometry = scene->items + item_indices[i];

            // A clip entry names the head of a material-variant chain. The
            // original submits every material linked through NUMTL::next.
            for (NUMTL *material = scene->mtls[material_index]; material != NULL; material = material->next) {
                NUDISPLAYLIST *list = material->display_list;
                if (list == NULL) {
                    continue;
                }

                scene->flags |= NUDL_SCENE_FLAG_CLIP_MATERIALS;
                const i32 used_material = list->mtl_id;
                u8 *used = scene->mtl_used[scene->render_buffer >> 7];
                used[used_material >> 3] |= static_cast<u8>(1U << (used_material & 7));

                RndrStateSetConstAlphaTint(nuspecial_const_alpha_enabled, nuspecial_const_tint_enabled,
                                           nuspecial_const_alpha, &nuspecial_const_tint, material);
                DisplayListUpdateRenderState(list, &render_state);

                VARIPTR *buffer = NuDisplayListLinkItems(list, 2);
                NUDISPLAYLISTITEM *items = list->items;
                void *transform = DisplayListCreateGeomTransformPS(buffer, mtx, material, geometry->next, NULL);
                items[0].type = 0x8c;
                items[0].id = 3;
                items[0].next = transform;
                items[1].type = 0x82;
                items[1].id = 3;
                items[1].next = geometry->next;
                list->items = items + 2;
                DisplayListSetAlphaPS(items, items + 1, alpha);
            }
        }
        RndrStateSetConstAlphaTint(0, 0, 0.0f, NULL, NULL);
        return clip_state;
    }
    void NuDisplayListSetFxParam(void) {
    }
    void NuDisplayListUpdateSpecial(void) {
    }

    // ---------------------------------------------------------------------------
    // Scene / render-scene
    // ---------------------------------------------------------------------------

    void NuDisplaySceneClone(void) {
    }
    void NuDisplaySceneClonePS(void) {
    }
    void NuDisplaySceneDebug(void) {
    }
    void NuDisplaySceneDestroy(void) {
    }
    void NuDisplaySceneDestroyPS(void) {
    }

    // ---------------------------------------------------------------------------
    // Frame lifecycle (NuFrameEnd is faithful; neighbours are stubs)
    // ---------------------------------------------------------------------------

    // Per-frame animation/pad update hooks (originals take the frame time).
    void NuOcclusionManagerEndFrame(void);
    void NuPad_Interface_Render(void);
    void NuPadUpdatePads(void);

    // Frame-end callbacks (original bss @0x6bdaec/0x6bdaf0/0x6bdad0).
    extern void (*preRenderFlashingHack)(void);
    extern void (*postRenderFlashingHack)(void);
    extern void (*nuapi_endframe_callbackfn)(void);

    static f32 NuFrameEnd_min_delay = 0;

    // Faithful transcription of the original frame-end pump. Waits for the
    // target frame interval, ticks material/tex/wind anims, swaps screens
    // via NuRndrSwapScreenEx, then advances nuapi clocks and pad state.
    f32 NuFrameEnd(void) {
        static i32 ShowingError = 0; // _ZZ10NuFrameEndE12ShowingError

        i32 done = 0;
        NuHasError(); // original records the result for the error dialog
        i32 has_error = 0;

        static int dbg_fe = 0;
        if (dbg_fe++ < 3 || dbg_fe % 1000 == 0) {
            LOG_INFO("NuFrameEnd #%d max_fps=%d time=%u.%u", dbg_fe, nuapi.max_fps, nuapi.time.high, nuapi.time.low);
        }

        if (nuapi.max_fps != 0) {
            // Wait until at least 1/max_fps seconds have elapsed since frame
            // begin (nuapi.time), then record the elapsed time.
            f32 target = 1.0f / (f32)nuapi.max_fps;

            NUTIME now;
            NUTIME delta;
            do {
                NuTimeGet(&now);
                NuTimeSub(&delta, &now, &nuapi.time);
            } while (target > NuTimeSeconds(&delta));

            nuapi.frametime = NuTimeSeconds(&delta);
        }

        NuMtlAnimate();     // original passes frametime
        NuTexAnimProcess(); // original passes frametime
        NuWindAnimate();    // original: (wind, frametime)
        NuOcclusionManagerEndFrame();
        NuPadRecordEndFrame();
        NuTimeBarSetRender(); // original passes -1
        NuPad_Interface_Render();

        done = 1;

        if (preRenderFlashingHack != NULL) {
            preRenderFlashingHack();
        }

        NuRndrSwapScreenEx(-1, nuapi_endframe_callbackfn);

        NUTIME end;
        NUTIME delta2;
        NuTimeGet(&end);
        NuTimeSub(&delta2, &end, &nuapi.time);
        nuapi.frametime = NuTimeSeconds(&delta2);
        nuapi.time = end;

        if (nuapi.frametime > 0.1f) {
            nuapi.frametime = 0.1f;
        }

        NuTimeGet(&nuapi.time2);

        if (done && NuFrameEnd_min_delay != 0) {
            bgSuspendMain((i32)NuFrameEnd_min_delay);
        }

        if (postRenderFlashingHack != NULL) {
            postRenderFlashingHack();
        }

        NuPadUpdatePads();
        nuapi.field19_0x3c++;
        nuapi.nuframe_begin_cnt--;

        if (ShowingError == 0 && has_error != 0) {
            ShowingError = 1;
            // Original shows an error dialog here.
        } else if (ShowingError != 0) {
            ShowingError = 0;
        }

        return nuapi.frametime;
    }
    void NuFrameSetMinDelay(void) {
    }

    // ---------------------------------------------------------------------------
    // iOS / platform
    // ---------------------------------------------------------------------------

    void NuIOSMtlInit(void) {
    }
    void NuIOS_AwardAchievement(void) {
    }
    void NuIOS_CheckCurrentFramebuffer(void) {
    }
    void NuIOS_DeallocateSystemFramebuffers(void) {
    }
    void NuIOS_DeallocateSystemRenderbuffer(void) {
    }
    void NuIOS_FreeMemoryForSuspend(void) {
    }
    f32 NuIOS_GetAspectRatio(void) {
        if (nuapi.screen_width > 0 && nuapi.screen_height > 0) {
            return (f32)nuapi.screen_width / (f32)nuapi.screen_height;
        }
        return 16.0f / 9.0f;
    }
    // Locale string filled by the platform layer (Java nativeSetLanguage on
    // device; LANG environment on host) and cached result index.
    char g_language[16] = {0}; // original bss @0x66f440
    i32 g_languageIndex = -1;  // original .data @0x616b80

    // original 0xe3640 — exact locale-matching ladder of the original.
    i32 NuIOS_GetDeviceLanguage(void) {
        if (g_languageIndex != -1) {
            return g_languageIndex;
        }
        auto matches = [&](const char *suffix, i32 n) {
            for (i32 k = 0; k < n; k++) {
                if (g_language[k] != suffix[k]) {
                    return false;
                }
            }
            return true;
        };
        if (matches("en-us", 5))
            return g_languageIndex = 0x12;
        if (matches("en-", 3))
            return g_languageIndex = 1;
        if (matches("fr-ca", 5))
            return g_languageIndex = 2;
        if (matches("fr-", 3))
            return g_languageIndex = 2;
        if (matches("it-", 3))
            return g_languageIndex = 5;
        if (matches("de-", 3))
            return g_languageIndex = 4;
        if (matches("es-mx", 5))
            return g_languageIndex = 3;
        if (matches("es-", 3))
            return g_languageIndex = 3;
        if (matches("ja-", 3))
            return g_languageIndex = 0;
        if (matches("ko-", 3))
            return g_languageIndex = 0xd;
        if (matches("nl-", 3))
            return g_languageIndex = 6;
        if (matches("pt-br", 5))
            return g_languageIndex = 0x10;
        if (matches("pt-", 3))
            return g_languageIndex = 7;
        if (strncmp(g_language, "zh-", 3) == 0)
            return g_languageIndex = 0x13;
        if (strncmp(g_language, "hu-", 3) == 0)
            return g_languageIndex = 1;
        if (strncmp(g_language, "ru-", 3) == 0)
            return g_languageIndex = 0xc;
        if (strncmp(g_language, "pl-", 3) == 0)
            return g_languageIndex = 0xb;
        if (strncmp(g_language, "cs-", 3) == 0)
            return g_languageIndex = 10;
        if (strncmp(g_language, "el-", 3) == 0)
            return g_languageIndex = 9;
        if (strncmp(g_language, "da-", 3) == 0)
            return g_languageIndex = 8;
        if (strncmp(g_language, "no-", 3) == 0)
            return g_languageIndex = 0xf;
        if (strncmp(g_language, "sv-", 3) == 0)
            return g_languageIndex = 0xe;
        if (strncmp(g_language, "fi-", 3) == 0)
            return g_languageIndex = 0x11;
        return -1;
    }
    void NuIOS_HardwareSupportsRetina(void) {
    }
    void NuIOS_IsLowestEndDevice(void) {
    }
    void NuIOS_IsMidRangeDevice(void) {
    }
    void NuIOS_IsSmallScreen(void) {
    }
    void NuIOS_IsWidescreen(void) {
    }
    void NuIOS_RecordFlurryEvent(void) {
    }
    void NuIOS_ShowAchievements(void) {
    }

    // ---------------------------------------------------------------------------
    // Animation / curves / data
    // ---------------------------------------------------------------------------

    void NuAnimBuffAccumulate_3(void) {
    }
    void NuAnimBuffCreate(void) {
    }
    void NuAnimBuffCreateScratch(void) {
    }
    void NuAnimBuffDestroyScratch(void) {
    }
    void NuAnimBuffEvaluateCallback(void) {
    }
    void NuAnimBuffEvaluate_3(void) {
    }
    void NuAnimBuffProceduralAnimation(void) {
    }
    void NuAnimCurve2CalcValEx(void) {
    }
    void NuAnimCurve2SetApplyToJoint(void) {
    }
    void NuAnimCurve2SetApplyToJointTransLoc(void) {
    }
    void NuAnimCurve2SetApplyToMatrix_3(void) {
    }
    void NuAnimCurveCalcVal2(void) {
    }
    void NuAnimCurveCreate(void) {
    }
    void NuAnimCurveDestroy(void) {
    }
    void NuAnimCurveExtractAllNodeCurves_3(void) {
    }
    void NuAnimCurveSetApplyBlendToJoint2(void) {
    }
    void NuAnimCurveSetApplyToMatrix(void) {
    }
    void NuAnimCurveSetCreate(void) {
    }
    void NuAnimCurveSetDestroy(void) {
    }
    void NuAnimData2CalcMatrix(void) {
    }
    void NuAnimData2CalcTime(void) {
    }
    void NuAnimData2FixPtrs(void) {
    }
    void NuAnimData2Fixup(void) {
    }
    void NuAnimData2LoadBuff(void) {
    }
    void NuAnimData2LoadBuffEx(void) {
    }
    void NuAnimData2LoadBuffFromPAK(void) {
    }
    void NuAnimData2Relocate(void) {
    }
    void NuAnimDataCalcTime(void) {
    }
    void NuAnimDataCreate(void) {
    }
    void NuAnimDataDestroy(void) {
    }
    void NuAnimDataFindVersion(void) {
    }
    void NuAnimDataFixPtrs(void) {
    }
    void NuAnimDataLoadBuff(void) {
    }
    void NuAnimDataRead(void) {
    }
    void NuAnimEndFrame(void) {
    }
    void NuAnimEndFrameOld(void) {
    }
    void NuAnimGetAnimLOD(void) {
    }
    void NuAnimGetUseQuatsFlag(void) {
    }
    void NuAnimInit(void) {
    }
    void NuAnimNumNodes(void) {
    }
    void NuAnimPopUseQuatsFlag(void) {
    }
    void NuAnimPushSetUseQuatsFlag(void) {
    }
    void NuAnimSetUseQuatsFlag(void) {
    }

    // ---------------------------------------------------------------------------
    // Bridge / file / heap / memory
    // ---------------------------------------------------------------------------

    void NuBridgeCreate(void) {
    }
    void NuBridgeDraw(void) {
    }
    void NuBridgeInit(void) {
    }
    void NuBridgeOn(void) {
    }
    void NuBridgeRemove(void) {
    }
    void NuBridgeUpdate(void) {
    }
    void NuDatClose(void) {
    }
    void NuDatGet(void) {
    }
    void NuDatGetFileInfo(void) {
    }
    void NuHeapAlloc(void) {
    }
    void NuHeapAllocAligned(void) {
    }
    void NuHeapAllocAlignedNamed(void) {
    }
    void NuHeapAllocNamed(void) {
    }
    void NuHeapCreate(void) {
    }
    void NuHeapDefragAllocation(void) {
    }
    void NuHeapDestroy(void) {
    }
    void NuHeapFree(void) {
    }
    void NuHeapGetAllocatedBlockCount(void) {
    }
    void NuHeapGetFreeBlockCount(void) {
    }
    void NuHeapGetTotalAllocated(void) {
    }
    void NuHeapGetTotalFree(void) {
    }
    void NuAllocHigh(void) {
    }
    void NuAllocHighInit(void) {
    }
    void NuFreeHigh(void) {
    }
    void NuMemAllocFn(void) {
    }
    void NuMemBlkAlloc(void) {
    }
    void NuMemBlkCreate(void) {
    }
    void NuMemBlkCreateEx(void) {
    }
    void NuMemBlkCreateVari(void) {
    }
    void NuMemBlkDestroy(void) {
    }
    void NuMemBlkFree(void) {
    }
    void NuMemBlkSize(void) {
    }
    void NuMemCopy128(void) {
    }
    void NuMemCreateDiscardable(void) {
    }
    void NuMemDestroyDiscardable(void) {
    }
    void NuMemFlushDiscardable(void) {
    }
    void NuMemFreeFn(void) {
    }
    void NuMemGetPeakAllocAddr(void) {
    }
    void NuMemReAllocFn(void) {
    }
    void NuMemSetDiscardable(void) {
    }
    void NuMemSetExternal(void) {
    }
    void NuMemSetHeap(void) {
    }
    void NuScratchAlloc128(void) {
    }
    void NuScratchAlloc32(void) {
    }
    void NuScratchAlloc64(void) {
    }
    void NuScratchRelease(void) {
    }
    void NuScratchReset(void) {
    }
    void NuPtrBlockRead(void) {
    }
    void NuPPGetSize(void) {
    }
    void NuPPUnpack(void) {
    }
    void NuSysDirClose(void) {
    }
    void NuSysDirOpen(void) {
    }
    void NuSysDirRead(void) {
    }

    // ---------------------------------------------------------------------------
    // Math / geometry
    // ---------------------------------------------------------------------------

    void NuBez3Subdiv(void) {
    }
    void NuBez3ToCubic(void) {
    }
    void NuBezierCubicPatchEvaluate(void) {
    }
    void NuBezierCubicPatchEvaluatePartials(void) {
    }
    void NuBezierCubicPatchPartialsU(void) {
    }
    void NuBezierCubicPatchPartialsUU(void) {
    }
    void NuBezierCubicPatchPartialsUUU(void) {
    }
    void NuBezierCubicPatchPartialsUUV(void) {
    }
    void NuBezierCubicPatchPartialsUUVV(void) {
    }
    void NuBezierCubicPatchPartialsUUVVV(void) {
    }
    void NuBezierCubicPatchPartialsUVV(void) {
    }
    void NuBezierCubicPatchPartialsV(void) {
    }
    void NuBezierCubicPatchPartialsVV(void) {
    }
    void NuBezierCubicPatchPartialsVVV(void) {
    }
    void NuBezierQuadraticTriangleEvaluateBarycentric(void) {
    }
    void NuBezierQuadraticTriangleEvaluateParametric(void) {
    }
    void NuBezierQuadraticTrianglePartialsUU(void) {
    }
    void NuBezierQuadraticTrianglePartialsUeV(void) {
    }
    void NuBezierQuadraticTrianglePartialsVV(void) {
    }
    void NuCeil(void) {
    }
    void NuCubicToBez3(void) {
    }
    void NuEquiv(void) {
    }
    void NuEquivTollerance(void) {
    }
    void NuFrsqrt(void) {
    }
    void NuHalfToFloat(void) {
    }
    void NuInfiniteLineToPointDistSqr(void) {
    }
    void NuInfiniteLineToPointDistSqrEx(void) {
    }
    void NuLineToLineDist(void) {
    }
    void NuLineToPointDistSqr(void) {
    }
    void NuLog2(void) {
    }
    void NuMiscNextPow2(void) {
    }
    void NuMiscNextPow2Exp(void) {
    }
    void NuMiscPow2Exp(void) {
    }
    void NuMtxInvVU0(void) {
    }
    void NuMtxMulArrayVU0(void) {
    }
    void NuMtxMulRVU0(void) {
    }
    void NuMtxMulVU0(void) {
    }
    void NuMtxMulnVU0(void) {
    }
    void NuMtxPreScaleUVU0(void) {
    }
    void NuMtxPreScaleVU0(void) {
    }
    void NuMtxScaleVU0(void) {
    }
    void NuMtxSetRotateXYZVU0(void) {
    }
    void NuMtxSetRotationXYVU0(void) {
    }
    void NuPlnDist(void) {
    }
    void NuPlnDist2(void) {
    }
    void NuPlnEqnPn(void) {
    }
    void NuPlnLine2(void) {
    }
    void NuPlnLineVU0(void) {
    }
    void NuPlnPlnIntersect(void) {
    }
    void NuPointRelToBoundingBox(void) {
    }
    void NuPow(void) {
    }
    void NuPower2(void) {
    }
    void NuEulerXYZFromQuat(void) {
    }

    // ---------------------------------------------------------------------------
    // Fonts / text
    // ---------------------------------------------------------------------------

    void NuFntClose(void) {
    }
    void NuFntCreate(void) {
    }
    void NuFntDestroy(void) {
    }
    void NuFntGetScreenHeight(void) {
    }
    void NuFntInit(void) {
    }
    void NuFntLoadPtr(void) {
    }
    void NuFntMoveAbs(void) {
    }
    void NuFntMoveRel(void) {
    }
    void NuFntPointSize(void) {
    }
    void NuFntPos(void) {
    }
    void NuFntPrint(void) {
    }
    void NuFntPrintEx(void) {
    }
    void NuFntPrintLen(void) {
    }
    void NuFntPrintLenV(void) {
    }
    void NuFntPrintV(void) {
    }
    void NuFntScale(void) {
    }
    void NuFntSet(void) {
    }
    void NuFntSetFixedWidthNumerals(void) {
    }
    void NuFntSetPen(void) {
    }
    void NuFntToLower(void) {
    }
    void NuFntToUpper(void) {
    }
    void NuFntWrite(void) {
    }
    void NuQFntCreate(void) {
    }
    void NuQFntDestroy(void) {
    }
    void NuQFntEncodeUnicodeString(void) {
    }
    void NuQFntGetCoordinateSystem(void) {
    }
    void NuQFntGetPrintMode(void) {
    }
    void NuQFntHeightScale(void) {
    }
    void NuQFntLenScale(void) {
    }
    void NuQFntMove2d(void) {
    }
    void NuQFntPopCoordinateSystem(void) {
    }
    void NuQFntPrint2dU(void) {
    }
    void NuQFntPrint2dW(void) {
    }
    void NuQFntPrint3DU(void) {
    }
    void NuQFntPrint3DW(void) {
    }
    void NuQFntPrintEx(void) {
    }
    void NuQFntPrintLenU(void) {
    }
    void NuQFntPrintLenV(void) {
    }
    void NuQFntPrintU(void) {
    }
    void NuQFntPrintV(void) {
    }
    void NuQFntPushCoordinateSystem(void) {
    }
    void NuQFntSet2d(void) {
    }
    void NuQFntSetColour2d(void) {
    }
    void NuQFntSetPointSize(void) {
    }
    void NuQFntSetPrintMode(void) {
    }
    void NuQFntSetScale2d(void) {
    }
    void NuQFntUTF8toQCode(void) {
    }
    void NuQFntWrite(void) {
    }
    void NuQFntWriteUniversalFont(void) {
    }
    void NuStringFilterLoad(char *path, VARIPTR *buf, VARIPTR *buf_end) {
        (void)path;
        (void)buf;
        (void)buf_end;
    }
    void NuStringFilterBadWords(void) {
    }
    void NuStringFilterBadWordsW(void) {
    }
    void NuStringTableGetById(void) {
    }
    void NuStringTableGetFormat(void) {
    }
    void NuStringTableGetIdByName(void) {
    }
    void NuStringTableLoad(void) {
    }
    void NuStringTableLoadTXT(void) {
    }
    void NuStringTableSaveCharacterList(void) {
    }
    void NuStringTableUnload(void) {
    }

    // ---------------------------------------------------------------------------
    // Rendering / materials / effects (host has GL paths elsewhere)
    // ---------------------------------------------------------------------------

    void NuAccumulationMotionBlurEffect(void) {
    }
    void NuAccumulationMotionBlurParams(void) {
    }
    void NuBackbufferCopy(void) {
    }
    void NuDeferredShadingRender(void) {
    }
    void NuDeferredShadingSetParameterf(void) {
    }
    void NuDepthOfFieldEffect(void) {
    }
    void NuDepthOfFieldEffect1(void) {
    }
    void NuDepthOfFieldEffect2(void) {
    }
    void NuDepthOfFieldEffectEx(void) {
    }
    void NuEffectTexCreate1D(void) {
    }
    void NuEffectTex360Create2D_aliased(void) {
    }
    void NuEffectTex360Create3D_aliased(void) {
    }
    void NuEffectTexCreate2D(void) {
    }
    void NuEffectTexCreateCube(void) {
    }
    void NuEffectTexCreateCube_aliased(void) {
    }
    void NuEffectTexCreateFromNativeTex(void) {
    }
    void NuEffectTexGetDimension(void) {
    }
    void NuEffectTexGetEffectFromNative(void) {
    }
    void NuEffectTexGetLockedVP(void) {
    }
    void NuEffectTexLockVP(void) {
    }
    void NuEffectTexMapNative(void) {
    }
    void NuEffectTexUnlockVP(void) {
    }
    void NuEffectTexUnmapNative(void) {
    }
    void NuFadeObjCreateMtx(void) {
    }
    void NuFadeObjDraw(void) {
    }
    void NuFadeObjInit(void) {
    }
    void NuFadeObjSet_SetLightsFn(void) {
    }
    void NuFadeObjSetup(void) {
    }
    void NuFadeObjUpdate(void) {
    }
    void NuFadeObjUpdateArray(void) {
    }
    void NuFramebufferAttachTex2D(void) {
    }
    void NuFramebufferBind(void) {
    }
    static void NuFramebufferClear(void) {
    }
    void NuFramebufferCopyTex2D(void) {
    }
    void NuFramebufferCreate(void) {
    }
    void NuFramebufferDestroy(void) {
    }
    void NuFramebufferDrawBuffers(void) {
    }
    void NuFramebufferEnableGuards(void) {
    }
    void NuFramebufferGetAttachedTex(void) {
    }
    void NuFramebufferGetBackBuffer(void) {
    }
    void NuFramebufferGetBound(void) {
    }
    void NuFramebufferGetDefault(void) {
    }
    void NuFramebufferGetFrontBuffer(void) {
    }
    void NuFramebufferGetHeight(void) {
    }
    void NuFramebufferGetObject(void) {
    }
    void NuFramebufferGetSamples(void) {
    }
    void NuFramebufferGetWidth(void) {
    }
    void NuFramebufferInitEx(void) {
    }
    void NuFramebufferResolve(void) {
    }
    void NuFramebufferResolveAll(void) {
    }
    void NuFramebufferResolveMultisample(void) {
    }
    void NuFramebufferSetClearColor(void) {
    }
    static void NuFramebufferSwapBuffers(void) {
    }
    void NuLightAddSpot(void) {
    }
    void NuLightFogG(void) {
    }
    void NuLightFogPal(void) {
    }
    void NuLightFogX(void) {
    }
    void NuLightInit(void) {
    }
    void NuLightMatInit(void) {
    }
    void NuLightSpeedBlur(void) {
    }
    void NuLightSpeedBlurOldCameraPos(void) {
    }
    void NuLightSpeedBlurScale(void) {
    }
    void NuLightSpotFadeSet(void) {
    }
    void NuLgtArcLaser(void) {
    }
    void NuLgtLaser(void) {
    }
    void NuLgtLaserDraw(void) {
    }
    void NuLgtSetArcMat(void) {
    }
    void NuPostBloom(void) {
    }
    void NuPostEffectAccumulationMotionBlur(void) {
    }
    void NuPostEffectBloom(void) {
    }
    void NuPostEffectDeferredShading(void) {
    }
    void NuPostEffectDepthOfField(void) {
    }
    void NuPostEffectDestroy(void) {
    }
    void NuPostEffectDisable(void) {
    }
    void NuPostEffectEnable(void) {
    }
    void NuPostEffectGetActiveDynamicLightCount(void) {
    }
    void NuPostEffectGetBackBuffer(void) {
    }
    void NuPostEffectGetDepthBuffer(void) {
    }
    void NuPostEffectInit(void) {
    }
    void NuPostEffectIsEnabled(void) {
    }
    void NuPostEffectMotionBlur(void) {
    }
    void NuPostEffectRender(void) {
    }
    void NuPostEffectSpeedBlur(void) {
    }
    void NuPostEffectTiming(void) {
    }
    void NuRainDraw(void) {
    }
    void NuRainProcess(void) {
    }
    void NuRainSetFall(void) {
    }
    void NuRenderContextInit(void) {
        extern f32 g_renderContext_viewProj[16];
        extern f32 g_renderContext_view[16];
        extern f32 g_renderContext_projection[16];
        extern f32 g_renderContext_world[16];
        memcpy(g_renderContext_viewProj, &numtx_identity, sizeof(numtx_identity));
        memcpy(g_renderContext_view, &numtx_identity, sizeof(numtx_identity));
        memcpy(g_renderContext_projection, &numtx_identity, sizeof(numtx_identity));
        memcpy(g_renderContext_world, &numtx_identity, sizeof(numtx_identity));
    }
    void NuRenderContext360BeginGameTime(void) {
    }
    void NuRenderContext360EndGameTime(void) {
    }
    void NuRenderContextSetAlphaBlend(void) {
    }
    __attribute__((weak)) void NuRenderContextSetViewProj(NUMTX *view, NUMTX *projection) {
        extern f32 g_renderContext_viewProj[16];
        extern f32 g_renderContext_viewProjInverse[16];
        extern f32 g_renderContext_view[16];
        extern f32 g_renderContext_projection[16];
        extern f32 g_renderContext_position[4];

        NUVEC scale = {
            g_NuVpRegion.projection_x_scale,
            g_NuVpRegion.projection_y_scale,
            1.0f,
        };
        NUVEC translation = {
            g_NuVpRegion.projection_x_offset,
            g_NuVpRegion.projection_y_offset,
            0.0f,
        };
        NUMTX scale_mtx;
        NUMTX translation_mtx;
        NUMTX adjusted_projection;
        NuMtxSetScale(&scale_mtx, &scale);
        NuMtxSetTranslation(&translation_mtx, &translation);
        NuMtxMulH(&adjusted_projection, projection, &scale_mtx);
        NuMtxMulH(&adjusted_projection, &adjusted_projection, &translation_mtx);

        memcpy(g_renderContext_view, view, sizeof(NUMTX));
        memcpy(g_renderContext_projection, &adjusted_projection, sizeof(NUMTX));

        NUMTX inverse_view;
        NuMtxInv(&inverse_view, view);
        g_renderContext_position[0] = inverse_view.m30 / inverse_view.m33;
        g_renderContext_position[1] = inverse_view.m31 / inverse_view.m33;
        g_renderContext_position[2] = inverse_view.m32 / inverse_view.m33;
        g_renderContext_position[3] = 1.0f;

        NuMtxMulH(reinterpret_cast<NUMTX *>(g_renderContext_viewProj), view, &adjusted_projection);
        NuMtxInvH(reinterpret_cast<NUMTX *>(g_renderContext_viewProjInverse),
                  reinterpret_cast<NUMTX *>(g_renderContext_viewProj));

        // OpenGL's clip-space depth is [-w,+w], while the engine camera
        // packet contains the original D3D-style [0,+w] projection.
        NUMTX depth_remap = numtx_identity;
        depth_remap.m22 = 2.0f;
        depth_remap.m32 = -1.0f;
        NuMtxMulH(reinterpret_cast<NUMTX *>(g_renderContext_viewProj),
                  reinterpret_cast<NUMTX *>(g_renderContext_viewProj), &depth_remap);

        NuShaderManagerSetfv(0x3d, g_renderContext_view);
        NuShaderManagerSetfv(0x3e, g_renderContext_viewProj);
        NuShaderManagerSetfv(0x56, g_renderContext_position);

        f32 fov;
        f32 aspect;
        f32 near_clip;
        f32 far_clip;
        f32 perspective[4];
        NuMtxGetPerspectiveD3D(projection, &fov, &aspect, &near_clip, &far_clip);
        perspective[0] = near_clip;
        perspective[1] = far_clip;
        perspective[2] = far_clip - near_clip;
        perspective[3] = perspective[2] / far_clip;
        NuShaderManagerSetfv(0x49, perspective);

        f32 frustum[4];
        NuMtxGetFrustumD3D(projection, &frustum[0], &frustum[1], &frustum[2], &frustum[3], &near_clip, &far_clip);
        frustum[1] -= frustum[0];
        frustum[3] -= frustum[2];
        NuShaderManagerSetfv(0x4a, frustum);
    }
    void NuRenderContextSetViewport(void) {
    }
    void NuRenderDeviceIsContextValid(void) {
    }
    void NuSpecialAddShadowLight(void) {
    }
    void NuSpecialBurstDrawAt(void) {
    }
    void NuSpecialClear(void) {
    }
    void NuSpecialClearShadowClipTestResults(void) {
    }
    void NuSpecialClearShadowLights(void) {
    }
    void NuSpecialClipTestExtents(void) {
    }
    void NuSpecialClipTestShadowLights(void) {
    }
    void NuSpecialCompare(void) {
    }
    void NuSpecialConstAlpha(i32 enabled, f32 alpha) {
        nuspecial_const_alpha_enabled = enabled;
        nuspecial_const_alpha = alpha;
    }
    void NuSpecialConstTint(void) {
    }
    void NuSpecialDrawAt(void) {
    }
    i32 NuSpecialDrawAtAlpha(void *special, NUMTX *mtx, f32 alpha) {
        NuSpecialHandleLayout *handle = reinterpret_cast<NuSpecialHandleLayout *>(special);
        if (handle->scene == NULL || alpha <= 0.0f) {
            return 0;
        }
        if (alpha < 1.0f) {
            NuSpecialConstAlpha(1, alpha);
            i32 result = NuDisplayListRndrSpecial(reinterpret_cast<nuhspecial_s *>(special), mtx, 0, NULL, NULL);
            NuSpecialConstAlpha(0, 0.0f);
            return result;
        }
        return NuDisplayListRndrSpecial(reinterpret_cast<nuhspecial_s *>(special), mtx, 0, NULL, NULL);
    }
    void NuSpecialDrawSmoothSkin(void) {
    }
    void NuSpecialDrawSmoothSkinDwa(void) {
    }
    void NuSpecialDrawWith(void) {
    }
    i32 NuSpecialExistsFn(void *special) {
        if (special == NULL) {
            return 0;
        }
        NuSpecialHandleLayout *handle = reinterpret_cast<NuSpecialHandleLayout *>(special);
        return handle->special != NULL || handle->display_special != NULL;
    }
    void NuSpecialFindMulti(void) {
    }
    void NuSpecialFindMultiWC(void) {
    }
    void NuSpecialForceMtl(void) {
    }
    void NuSpecialForceToAlpha(void) {
    }
    void NuSpecialGetActiveShadowLights(void) {
    }
    void NuSpecialGetAnimEndFrame(void) {
    }
    void NuSpecialGetBounds(void) {
    }
    void NuSpecialGetCollision(void) {
    }
    NUMTX *NuSpecialGetDrawMtx(void *special) {
        NuSpecialHandleLayout *handle = reinterpret_cast<NuSpecialHandleLayout *>(special);
        NuLegacySpecialLayout *legacy = static_cast<NuLegacySpecialLayout *>(handle->special);
        if (legacy != NULL) {
            NUMTX *instance = reinterpret_cast<NUMTX *>(legacy->instance);
            NUMTX *draw_mtx = *reinterpret_cast<NUMTX **>(legacy->instance + 0x48);
            return draw_mtx != NULL ? draw_mtx : instance;
        }
        NuDisplaySpecialLayout *display = static_cast<NuDisplaySpecialLayout *>(handle->display_special);
        if (display != NULL) {
            usize draw_mtx = reinterpret_cast<usize>(display->draw_mtx_ptr);
            if (draw_mtx != 0 && draw_mtx != static_cast<usize>(-1)) {
                return display->draw_mtx_ptr;
            }
            return &display->draw_mtx;
        }
        return NULL;
    }
    void NuSpecialGetDrawPos(void) {
    }
    void NuSpecialGetFirst(void) {
    }
    void NuSpecialGetInstAnim(void) {
    }
    void NuSpecialGetInstanceMtx(void) {
    }
    void NuSpecialGetInstanceix(void) {
    }
    void NuSpecialGetMtl(void) {
    }
    void NuSpecialGetMtx(void) {
    }
    void NuSpecialGetName(void) {
    }
    void NuSpecialGetNext(void) {
    }
    void NuSpecialGetNoVisiTestFn(void) {
    }
    void NuSpecialGetNumSpecials(void) {
    }
    void NuSpecialGetOnScreenFn(void) {
    }
    void NuSpecialGetOriginRadius(void) {
    }
    void NuSpecialGetPos(void) {
    }
    void NuSpecialGetRadius(void) {
    }
    void NuSpecialGetShadowClipTestResult(void) {
    }
    void NuSpecialGetShadowLight(void) {
    }
    void NuSpecialGetVisibilityFn(void) {
    }
    void NuSpecialHasActiveShadowLights(void) {
    }
    void NuSpecialHaveShadowClipTestResults(void) {
    }
    void NuSpecialList(void) {
    }
    void NuSpecialMtl(void) {
    }
    void NuSpecialMtlMap(void) {
    }
    void NuSpecialNumMtls(void) {
    }
    void NuSpecialSetAlphaTest(void) {
    }
    void NuSpecialSetBounds(void) {
    }
    i32 NuSpecialSetClipping(i32 enabled, i32 state) {
        i32 previous = nuspecial_clip_state;
        nuspecial_clip_state = enabled != 0 ? state : -1;
        return previous;
    }
    void NuSpecialSetCollision(void) {
    }
    void NuSpecialSetDrawMtx(void) {
    }
    void NuSpecialSetDrawPos(void) {
    }
    void NuSpecialSetInstAnimTime(void) {
    }
    void NuSpecialSetInstanceMtx(void) {
    }
    void NuSpecialSetMtx(void) {
    }
    void NuSpecialSetNoVisiTest(void) {
    }
    void NuSpecialSetOnScreen(void) {
    }
    void NuSpecialSetRenderPlane(void) {
    }
    void NuSpecialSetVisibility(void *special, i32 visible) {
        if (special == NULL) {
            return;
        }
        NuSpecialHandleLayout *handle = reinterpret_cast<NuSpecialHandleLayout *>(special);
        if (handle->scene == NULL) {
            return;
        }

        NuLegacySpecialLayout *legacy = static_cast<NuLegacySpecialLayout *>(handle->special);
        if (legacy != NULL) {
            if (legacy->instance != NULL) {
                legacy->instance[0x44] = static_cast<u8>((legacy->instance[0x44] & ~1U) | (visible & 1));
            }
            if (visible != 0) {
                legacy->flags |= 0x200;
            } else {
                legacy->flags &= ~0x200U;
            }
            return;
        }

        NuDisplaySpecialLayout *display = static_cast<NuDisplaySpecialLayout *>(handle->display_special);
        if (display == NULL) {
            return;
        }
        if (visible != 0) {
            display->flags |= 0x202;
        } else {
            display->flags &= ~0x202U;
        }

        NUDLDLISTSCENE *scene = reinterpret_cast<NUDLDLISTSCENE *>(handle->scene->display_list);
        if ((reinterpret_cast<u8 *>(scene)[0x76] & 1) != 0) {
            if (visible != 0) {
                scene->visibility_flags[display->instance_ix] |= 1;
            } else {
                scene->visibility_flags[display->instance_ix] &= ~1;
            }
        }
    }
    void NuSpecialTestAnim(void) {
    }
    void NuSpecialUpdate(void) {
    }
    void NuSpecialVertexOffsets(void) {
    }
    void NuSpecialVertexStates(void) {
    }
    void NuSpeedBlurSetMotionFactors(void) {
    }

    void NuTimeBarSlotLastValue(void) {
    }
    void NuTimeBarSlotLastValueMicroseconds(void) {
    }
    void NuTimeBarSlotSetEx(void) {
    }
    void NuWaterInit(void) {
    }
    void NuWaterOverride(void) {
    }
    void NuWaterRender(void) {
    }
    void NuWaterSpeed(void) {
    }

    // ---------------------------------------------------------------------------
    // Light / wind / particles / debris
    // ---------------------------------------------------------------------------

    void NuDynamicLightAddRenderScene(void) {
    }
    void NuDynamicLightAddShadowCasterScene(void) {
    }
    void NuDynamicLightBeginCapture(void) {
    }
    void NuDynamicLightClone(void) {
    }
    void NuDynamicLightCreate(void) {
    }
    void NuDynamicLightDestroy(void) {
    }
    void NuDynamicLightEndCapture(void) {
    }
    void NuDynamicLightGetActiveRenderSetCount(void) {
    }
    void NuDynamicLightGetDList(void) {
    }
    void NuDynamicLightGetParameterf(void) {
    }
    void NuDynamicLightGetParameteri(void) {
    }
    void NuDynamicLightGetProjection(void) {
    }
    void NuDynamicLightGetView(void) {
    }
    void NuDynamicLightIsUsedOnSpecials(void) {
    }
    void NuDynamicLightLookAt(void) {
    }
    void NuDynamicLightResetGeometry(void) {
    }
    void NuDynamicLightSetDirectional(void) {
    }
    void NuDynamicLightSetEnabled(void) {
    }
    void NuDynamicLightSetParameterf(void) {
    }
    void NuDynamicLightSetParameteri(void) {
    }
    void NuDynamicLightSetUsedOnSpecials(void) {
    }
    void NuDynamicLightSetupCustomCameraFrustum(void) {
    }
    void NuDynamicLightTestShadowExtrusionExtent(void) {
    }
    void NuDynamicLightTestShadowExtrusions(void) {
    }
    void NuDynamicLightTestShadowExtrusionsExtent(void) {
    }
    void NuDynamicLightTestShadowExtrusionsSpecial(void) {
    }
    void NuWindAnimate(void) {
    }
    void NuWindCreateMtx(void) {
    }
    void NuWindCurrent(void) {
    }
    void NuWindDraw(void) {
    }
    void NuWindInit(void) {
    }
    void NuWindLoad(void) {
    }
    void NuWindRand(void) {
    }
    void NuWindSetCurrent(void) {
    }
    void NuWindSetSpeed(void) {
    }
    void NuWindSetWorldSize(void) {
    }
    void NuWindSetup(void) {
    }
    void NuWindUnload(void) {
    }
    void NuWindUpdate(void) {
    }
    void NuWindUpdateArray(void) {
    }
    void NuPartEnableRayCasts(void) {
    }
    void NuPartGetSeed(void) {
    }
    void NuPartResetGlobalTime(void) {
    }
    void NuPartSetSeed(void) {
    }
    void NuInitDebrisRenderer(void) {
    }
    void NuPolyShadowInit(void) {
    }

    // ---------------------------------------------------------------------------
    // Gobj / hierarchy / scene graph
    // ---------------------------------------------------------------------------

    void NuHGobjDestroy(void) {
    }
    void NuHGobjEval(void) {
    }
    void NuHGobjEvalAnim(void) {
    }
    void NuHGobjEvalAnim2(void) {
    }
    void NuHGobjEvalAnim2Root(void) {
    }
    void NuHGobjEvalAnim2Root_3(void) {
    }
    void NuHGobjEvalAnimBlend(void) {
    }
    void NuHGobjEvalAnimBlend2(void) {
    }
    void NuHGobjEvalAnimBlend2Root(void) {
    }
    void NuHGobjEvalDwa(void) {
    }
    void NuHGobjEvalDwa2(void) {
    }
    void NuHGobjEvalDwaBlend(void) {
    }
    void NuHGobjEvalDwaBlend2(void) {
    }
    void NuHGobjForceShadowsOnCharacters(i32 v) {
        (void)v;
    }
    void NuHGobjFromVideoMem(void) {
    }
    void NuHGobjGetLayerIndex(void) {
    }
    void NuHGobjGetPOI(void) {
    }
    void NuHGobjJointMtx(void) {
    }
    void NuHGobjPOILocalMtxFromIX(void) {
    }
    void NuHGobjPOIMtx(void) {
    }
    void NuHGobjPOIMtxFromIX(void) {
    }
    void NuHGobjRestoreEvaluation(void) {
    }
    void NuHGobjRestrictEvaluation(void) {
    }
    void NuHGobjReversibleCharacters(i32 v) {
        (void)v;
    }
    void NuHGobjRndr(void) {
    }
    void NuHGobjRndrMtxDwa(void) {
    }
    void NuHGobjRndrRandShadowSurfacePoints(void) {
    }
    void NuHGobjSetClippingRootTrackerOverride(void) {
    }
    void NuHGobjToVideoMem(void) {
    }
    void NuGCutCharAnimProcess(void) {
    }
    void NuGCutLocatorCalcMtx(void) {
    }
    void NuGCutLocatorIsVisble(void) {
    }
    void NuGCutSceneDestroy(void) {
    }
    void NuGCutSceneIsBackgroundLoading(void) {
    }
    void NuGCutSceneLoadAddr(void) {
    }
    void NuGCutSceneSysBackgroundFlush(void) {
    }
    void NuGCutSceneSysInit(void) {
    }
    void NuGCutSceneSysPostBackgroundLoad(void) {
    }
    void NuGCutSceneSysRender(void) {
    }
    void NuGCutSceneSysUpdate(void) {
    }
    void NuGCutSetCutAudioStream(void) {
    }
    void NuGSceneProcessCrossFade(void) {
    }
    void NuGSceneSetCrossFade(void) {
    }
    void NuGSceneSetCrossFadeAlpha(void) {
    }
    void NuGHGPostRelocateFixupPS(void) {
    }
    void NuGHGPreRelocateFixupPS(void) {
    }
    void NuGHGRead(void) {
    }
    void NuGHGRelocate(void) {
    }

    // ---------------------------------------------------------------------------
    // Input / pad / key / mouse
    // ---------------------------------------------------------------------------

    void NuKeyFlush(void) {
    }
    void NuKeyGet(void) {
    }
    void NuKeyToAscii(void) {
    }
    void NuKey_current(void) {
    }
    void NuKey_last(void) {
    }
    void NuKey_simple(void) {
    }
    void NuKeyboard(void) {
    }
    void NuKeyboard_db(void) {
    }
    void NuMouseButton(void) {
    }
    void NuMouseButton_db(void) {
    }
    void NuMouseReadButtons(void) {
    }
    void NuMouseReadButtons_db(void) {
    }
    void NuMouseReadX(void) {
    }
    void NuMouseReadXRel(void) {
    }
    void NuMouseReadXVel(void) {
    }
    void NuMouseReadY(void) {
    }
    void NuMouseReadYRel(void) {
    }
    void NuMouseReadYVel(void) {
    }
    void NuMouseReadZ(void) {
    }
    void NuMouseReadZRel(void) {
    }
    void NuMouseReadZVel(void) {
    }
    void NuPadGetNumberOfPortsPS(void) {
    }
    void NuPadMapPlayerToPort(void) {
    }
    void NuPadRecordInit(void) {
    }
    void NuPadResetState(void) {
    }
    void NuPadSetDirectMappingState(void) {
    }
    void NuPadSetMaxGamePads(void) {
    }
    void NuPadSetMotors(void) {
    }
    void NuPadSetMotorsPS(void) {
    }
    void NuPadSetStatus(void) {
    }
    void NuPadSetValid(void) {
    }
    void NuPadUseCorrectDeadZoning(void) {
    }
    void NuPad_Interface_Render(void) {
    }
    void NuPad_Interface_ResetAllTouches(void) {
    }
    void NuPad_Interface_TouchScreenInput(void) {
    }
    void NuPs2ApplyDeadZone(void) {
    }
    void NuPs2VideoScreenDump(void) {
    }
    void NuPs2VideoSetPos(void) {
    }

    // ---------------------------------------------------------------------------
    // Culling / visibility / portals / occlusion
    // ---------------------------------------------------------------------------

    void NuPortalClipTest(void) {
    }
    void NuPortalClipTestBox(void) {
    }
    void NuPortalEnabled(void) {
    }
    void NuPortalNumRooms(void) {
    }
    void NuPortalResetActive(void) {
    }
    void NuPortalRoomClipTest(void) {
    }
    void NuPortalRoomClipTestAll(void) {
    }
    void NuPortalSetActive(void) {
    }
    void NuPortalWhichRoom(void) {
    }
    void NuVisiBoxTree(void) {
    }
    void NuVisiEvaluate(void) {
    }
    void NuVisiInstTree(void) {
    }
    void NuVisiOcclusion(void) {
    }
    void NuVisiOctree(void) {
    }
    void NuOcclusionManagerAddOccluderOBB(void) {
    }
    void NuOcclusionManagerAddOccluderQuad(void) {
    }
    void NuOcclusionManagerAddOccluderSphere(void) {
    }
    void NuOcclusionManagerEndFrame(void) {
    }
    void NuOcclusionManagerInit(void) {
    }
    void NuOcclusionManagerIsEnabled(void) {
    }
    void NuOcclusionManagerIsInitialised(void) {
    }
    void NuOcclusionManagerIsOccludedOBB(void) {
    }
    void NuOcclusionManagerIsOccludedSphere(void) {
    }
    void NuOcclusionManagerOnCameraSet(void) {
    }
    void NuOcclusionManagerRenderStats(void) {
    }
    void NuOcclusionManagerRenderZPass(void) {
    }
    void NuOcclusionManagerSetEnabled(void) {
    }
    void NuOcclusionManagerSetOccluderDotProductThreshold(void) {
    }
    void NuOcclusionManagerSetOccluderScreenSpaceThreshold(void) {
    }
    void NuInvalidateClipRanges(void) {
    }
    void NuClipXPlane(void) {
    }
    void NuClipYPlane(void) {
    }
    void NuClipZPlane(void) {
    }

    // ---------------------------------------------------------------------------
    // Viewport
    // ---------------------------------------------------------------------------

    void NuViewPortSet(void) {
    }
    void NuVpGetClippingMtx(void) {
    }
    void NuVpGetCurrent(void) {
    }
    void NuVpGetCurrent2(void) {
    }
    void NuVpGetRegions(void) {
    }
    void NuVpPixelHeight(void) {
    }
    void NuVpPixelWidth(void) {
    }
    void NuVpSetCentre(void) {
    }
    void NuVpSetClipping(void) {
    }
    void NuVpSetCurrent(void) {
    }
    void NuVpSetCurrent2(void) {
    }
    void NuVpSetPosition(void) {
    }
    void NuVpSetPosition2(void) {
    }
    void NuVpSetSize(void) {
    }
    void NuVpSetSize2(void) {
    }
    void NuVpSetZRange(void) {
    }
    void NuVpVirtualHeight(void) {
    }
    void NuVpVirtualWidth(void) {
    }

    // ---------------------------------------------------------------------------
    // Strings / conversion / Unicode
    // ---------------------------------------------------------------------------

    void NuAToFW(void) {
    }
    void NuAToIW(void) {
    }
    void NuIToA(void) {
    }
    void NuIToAW(void) {
    }
    void NuIsAl(void) {
    }
    void NuIsAlW(void) {
    }
    void NuSPrintfW(void) {
    }
    void NuStrCatW(void) {
    }
    void NuStrChrW(void) {
    }
    void NuStrCmpW(void) {
    }
    void NuStrCpyWC(void) {
    }
    void NuStrFindPosU(void) {
    }
    void NuStrFixExt(void) {
    }
    void NuStrGetExt(void) {
    }
    void NuStrGetFilenameNoExt(void) {
    }
    void NuStrGetPath(void) {
    }
    void NuStrICmpWC(void) {
    }
    void NuStrIStrW(void) {
    }
    void NuStrLenU(void) {
    }
    void NuStrLwr(void) {
    }
    void NuStrLwrW(void) {
    }
    void NuStrNCat(void) {
    }
    void NuStrNCatW(void) {
    }
    void NuStrNCmpW(void) {
    }
    void NuStrNCpyW(void) {
    }
    void NuStrNICmpW(void) {
    }
    void NuStrRChrW(void) {
    }
    void NuStrStrW(void) {
    }
    void NuStrSubstituteString(void) {
    }
    void NuStrToL(void) {
    }
    void NuStrToLW(void) {
    }
    void NuStrToLower(void) {
    }
    void NuStrTrap(void) {
    }
    void NuStrUprW(void) {
    }
    void NuStringTok(void) {
    }
    void NuUTF8ToUnicode(void) {
    }
    void NuFParCreateGivenFH(void) {
    }
    void NuFParGetOptionalFloat(void) {
    }
    void NuFParGetOptionalInt(void) {
    }
    void NuFParGetPos(void) {
    }
    void NuFParPushComCTX2(void) {
    }
    void NuFParSetPos(void) {
    }
    void NuLanguageGet(void) {
    }
    void NuLanguageSet(i32 language) {
        Text_Language = language;
    }
    void NuQTAddElement(void) {
    }
    void NuQTCreate(void) {
    }
    void NuQTRead(void) {
    }
    void NuQTWrite(void) {
    }

    // ---------------------------------------------------------------------------
    // Containers / lists / params
    // ---------------------------------------------------------------------------

    void NuLinkedListCheck(void) {
    }
    void NuLinkedListInsertAfter(void) {
    }
    void NuLinkedListInsertBefore(void) {
    }
    void NuLstAllocAfter(void) {
    }
    void NuLstAllocBefore(void) {
    }
    void NuLstAllocFree(void) {
    }
    void NuLstAtachHead(void) {
    }
    void NuLstAttachTail(void) {
    }
    void NuLstCreateBuff(void) {
    }
    void NuLstGetByIdx(void) {
    }
    void NuLstGetFree(void) {
    }
    void NuLstGetPrev(void) {
    }
    void NuLstMoveNext(void) {
    }
    void NuLstMovePrev(void) {
    }

    // ---------------------------------------------------------------------------
    // Debug / error / html / profiling
    // ---------------------------------------------------------------------------

    void NuClearError(void) {
    }
    void NuErrorCheck(void) {
    }
    void NuErrorProlog(void) {
    }
    void NuErrorSetFilter(void) {
    }
    void NuErrorSleep(void) {
    }
    void NuHasError(void) {
    }
    void NuDebugMsgProlog(void) {
    }
    void NuDebugMsgPrologTTY(void) {
    }
    void NuGetErrN(void) {
    }
    void NuGetError(void) {
    }
    void NuSevereWarning(void) {
    }
    void NuWarningProlog(void) {
    }
    void NuHtmlBanner(void) {
    }
    void NuHtmlBitmap(void) {
    }
    void NuHtmlEnd(void) {
    }
    void NuHtmlHBarGraph(void) {
    }
    void NuHtmlHLineGraph(void) {
    }
    void NuHtmlHeading2(void) {
    }
    void NuHtmlHeading3(void) {
    }
    void NuHtmlVBarGraph(void) {
    }
    // Profiling timebar sets are a deferred subsystem (the real one is
    // NuTimeBarCreateSet @0x2d7450 -> CreateSetEx @0x2d73f0 -> CreateTimeBar
    // @0x2a9860). Consumers only ever hand the returned handle to the
    // NuTimeBarSlot* stubs, so NULL behaves like profiling disabled.
    void *NuTimeBarCreateSet(i32) {
        return NULL;
    }
    void NuTimeBarCreateSetEx(void) {
    }
    void NuTimeBarCreateSetEx2(void) {
    }
    void NuTimeBarDestroySet(void) {
    }
    void NuTimeBarEnable(void) {
    }
    void NuTimeBarIndicateGpuFrameOut(void) {
    }
    void NuTimeBarInit(void) {
    }
    void NuTimeBarInitEx(void) {
    }
    void NuTimeBarResetPeaks(void) {
    }
    void NuTimeBarSetRender(void) {
    }
    void NuTimeBarSetRenderHorizontal(void) {
    }
    void NuTimeBarSetScaleY(void) {
    }
    void NuSetDebugMsgHandler(void) {
    }
    void NuSetErrorMsgHandler(void) {
    }
    void NuSetWarningMsgHandler(void) {
    }

    // ---------------------------------------------------------------------------
    // Time
    // ---------------------------------------------------------------------------

    void NuTimeForceFrameTime(void) {
    }
    void NuTimeGetSinceStartFrame(void) {
    }
    void NuTimeGetStartFrame(void) {
    }
    void NuTimeGetTime(void) {
    }
    void NuTimeScanlines(void) {
    }
    void NuTimeStartFrame(void) {
    }
    void NuTimeWait(void) {
    }

    // ---------------------------------------------------------------------------
    // Thread / misc OS
    // ---------------------------------------------------------------------------

    void NuDisableVBlankE(void) {
    }
    void NuEnableVBlankE(void) {
    }
    void NuGetCurrentThreadId(void) {
    }
    void NuThreadCreate(void) {
    }
    void NuThreadSignalRecieve(void) {
    }
    void NuThreadSignalSend(void) {
    }
    void NuPause(void) {
    }
    void NuPhoneOSMessagePost(void) {
    }
    void NuPhoneOSMessagePump(void) {
    }
    void NuPhysicalProtect(void) {
    }
    void NuSetGetHGObjFromIndxFn(void) {
    }
    void NuSetPadDemoEndButtons(void) {
    }
    void NuSetupVideoModeParams(void) {
    }
    void NuSetCutSceneCharacterCreateDataFn(void) {
    }
    void NuSetCutSceneCharacterDestroyDataFn(void) {
    }
    void NuSetCutSceneCharacterEvalFn(void) {
    }
    void NuSetCutSceneCharacterProcessFn(void) {
    }
    void NuSetCutSceneCharacterReleaseFn(void) {
    }
    void NuSetCutSceneCharacterRenderFn(void) {
    }
    void NuSetCutSceneDestroyCharactersFn(void) {
    }
    void NuSetCutSceneFindCharactersFn(void) {
    }
    void NuSetCutSceneRequestSFXFn(void) {
    }
    void NuSetCutSceneResetCharactersFn(void) {
    }
    void NuSetCutSceneRigidCollisionCheckFn(void) {
    }
    void NuSetCutSceneRigidPostRenderFn(void) {
    }
    void NuSetCutSceneSFXFixUpFn(void) {
    }
    void NuSetCutSceneSFXUpdateFn(void) {
    }

    // ---------------------------------------------------------------------------
    // Spline / online / net / other gameplay support
    // ---------------------------------------------------------------------------

    void NuSplineFindAllBeg(void) {
    }
    void NuSplineFindAllSub(void) {
    }
    void NuSplineFindNextBeg(void) {
    }
    void NuSplineGetRandomPoint(void) {
    }
    void NuSplineList(void) {
    }
    void NuOnlineAchievementAchieved(void) {
    }
    void NuOnlineAchievementAchievedEx(void) {
    }
    void NuOnlineAchievementAchievedExPS(void) {
    }
    void NuOnlineAchievementAchievedPS(void) {
    }
    void NuOnlineHasPlayerDownloaded(void) {
    }
    void NuOnlineHasPlayerDownloadedPS(void) {
    }
    void NuOnlineHasPlayerSignedIn(void) {
    }
    void NuOnlineHasPlayerSignedInEx(void) {
    }
    void NuOnlineHasPlayerSignedInExPS(void) {
    }
    void NuOnlineHasPlayerSignedInPS(void) {
    }
    void NuOnlineInit(void) {
    }
    void NuOnlineInitPS(void) {
    }
    void NuOnlineSetContext(void) {
    }
    void NuOnlineSetContextEx(void) {
    }
    void NuOnlineSetContextExPS(void) {
    }
    void NuOnlineSetContextPS(void) {
    }
    void NuOnlineSetDefaultContext(void) {
    }
    void NuOnlineSetDefaultContextEx(void) {
    }
    void NuOnlineSetDefaultContextExPS(void) {
    }
    void NuOnlineSetDefaultContextPS(void) {
    }
    void NuOnlineSetDefaultPresenceMode(void) {
    }
    void NuOnlineSetDefaultPresenceModeEx(void) {
    }
    void NuOnlineSetDefaultPresenceModeExPS(void) {
    }
    void NuOnlineSetDefaultPresenceModePS(void) {
    }
    void NuOnlineSetPresenceMode(void) {
    }
    void NuOnlineSetPresenceModeEx(void) {
    }
    void NuOnlineSetPresenceModeExPS(void) {
    }
    void NuOnlineSetPresenceModePS(void) {
    }
    void NuOnlineSetProfilePlayer(void) {
    }
    void NuOnlineSetProperty(void) {
    }
    void NuOnlineSetPropertyEx(void) {
    }
    void NuOnlineSetPropertyExPS(void) {
    }
    void NuOnlineSetPropertyPS(void) {
    }
    void NuOnlineSignInPlayer(void) {
    }
    void NuOnlineSignInPlayerPS(void) {
    }
    void NuMcCheckCardFormatted(void) {
    }
    void NuMcCheckCardFreeSpace(void) {
    }
    void NuMcCheckCardPresent(void) {
    }
    void NuMcCloseDir(void) {
    }
    void NuMcCreateDir(void) {
    }
    void NuMcFormat(void) {
    }
    void NuMcGetSlotMax(void) {
    }
    void NuMcOpenDir(void) {
    }
    void NuMcReadDir(void) {
    }
    void NuFmvInit(void) {
    }
    void NuFmvPlay(void) {
    }
    void NuFmvPlayV(void) {
    }
    void NuRegisterEndFrameCallBackFn(void) {
    }
    void NuRenderThreadDestroy(void) {
    }
    void NuRenderThreadIsCurrentThread(void) {
    }
    void NuRndIsReflectionGobj(void) {
    }
    void NuRndIsShadowReceiveRenderGobj(void) {
    }

    // ---------------------------------------------------------------------------
    // Rendering extras
    // ---------------------------------------------------------------------------

    void NuStreamInit(void) {
    }
    void NuFpException(void) {
    }
    void NuFpExceptionMask(void) {
    }
    void NuVSPrintf(void) {
    }
    void Nu360GetCommandLine(void) {
    }

    // ---------------------------------------------------------------------------
    // PS2 / Xbox legacy shims
    // ---------------------------------------------------------------------------

    struct nupad_s;
    struct nuframebuffer_s;
    struct nushaderobject_s;
    union variptr_u;

    void NuXboxLiveInit() __asm__("_Z14NuXboxLiveInitv");
    void NuXboxLiveInit() {
    }
    void NuPs2PadDemoEnd() __asm__("_Z15NuPs2PadDemoEndv");
    void NuPs2PadDemoEnd() {
    }
    i32 NuPs2GetLanguage() __asm__("_Z16NuPs2GetLanguagev");
    i32 NuPs2GetLanguage() {
        return 0;
    }
    void NuPs2PadSetMotors(nupad_s *, i32, i32) __asm__("_Z17NuPs2PadSetMotorsP7nupad_sii");
    void NuPs2PadSetMotors(nupad_s *, i32, i32) {
    }
    void Nu360ConfigureSMBSharing(char **) __asm__("_Z24Nu360ConfigureSMBSharingPPc");
    void Nu360ConfigureSMBSharing(char **) {
    }
    void NuFramebuffer360EndZPass() __asm__("_Z24NuFramebuffer360EndZPassv");
    void NuFramebuffer360EndZPass() {
    }
    bool NuFramebuffer360HasZPass() __asm__("_Z24NuFramebuffer360HasZPassv");
    bool NuFramebuffer360HasZPass() {
        return false;
    }
    void NuFramebuffer360BeginZPass(i32) __asm__("_Z26NuFramebuffer360BeginZPassi");
    void NuFramebuffer360BeginZPass(i32) {
    }
    i32 NuFramebuffer360GetTileCount(nuframebuffer_s *) __asm__("_Z28NuFramebuffer360GetTileCountP15nuframebuffer_s");
    i32 NuFramebuffer360GetTileCount(nuframebuffer_s *) {
        return 0;
    }
    void NuShaderObject360LoadShader(nushaderobject_s *) __asm__("_Z27NuShaderObject360LoadShaderP16nushaderobject_s");
    void NuShaderObject360LoadShader(nushaderobject_s *) {
    }
    void NuShaderObject360LoadPackFile(char *, variptr_u *,
                                       char *) __asm__("_Z29NuShaderObject360LoadPackFilePcP9variptr_uS0_");
    void NuShaderObject360LoadPackFile(char *, variptr_u *, char *) {
    }
    void
    NuShaderObject360UnloadShader(nushaderobject_s *) __asm__("_Z29NuShaderObject360UnloadShaderP16nushaderobject_s");
    void NuShaderObject360UnloadShader(nushaderobject_s *) {
    }

} // extern "C"
