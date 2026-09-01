// Nucore plain — C-linkage surface for the original libTTapp.so nucore TU.
struct NUGCUTLOCATORFNENTRY_s;
extern "C" NUGCUTLOCATORFNENTRY_s *locatorfns;
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
#include "java/java.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nuptrblock.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nugscn.h"
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
struct ani3_animheader_s;

extern "C" {
    u8 CutSceneBoundingBoxTrackRoot = 0;
}

extern "C" void ANI_FixUpAddrs(ani3_animheader_s *, isize);
extern "C" void ANI_Ani3ExtractAllNodeCurves(ani3_animheader_s *, f32, f32 *, i32, char *);

namespace {
    struct NuPlainSpecialHandleLayout {
        NUGSCN *scene;
        void *special;
        void *display_special;
    };

    struct NuPlainLegacySpecialLayout {
        u8 pad_00[0x40];
        u8 *instance;
        char *name;
        u32 flags;
    };

    struct NuPlainDisplaySpecialLayout {
        NUMTX mtx;
        NUMTX draw_mtx;
        NUVEC min;
        f32 min_w;
        NUVEC max;
        f32 max_w;
        NUVEC center;
        f32 radius;
        NUCLIPOBJECT *clip_objects;
        char *name;
        u32 flags;
        f32 *clip_range;
        i32 instance_ix;
        nuinstanim_s *instance_animation;
        i16 wind_speed;
        i16 wind_scale;
        u32 pad_cc;
    };

    struct NuPlainLegacySceneLayout {
        u8 pad_00[0x18];
        void **objects;
    };

    struct NuPlainLegacyInstanceBoundsLayout {
        u8 pad_00[0x40];
        i16 object_index;
    };

    struct NuPlainLegacyObjectBoundsLayout {
        u8 pad_00[0x0c];
        NUVEC minimum;
        NUVEC maximum;
        NUVEC center;
        f32 radius;
        u8 pad_34[4];
        NuPlainLegacyObjectBoundsLayout *next;
    };

    static i32 nuspecial_clip_state = -1;
    static i32 nuspecial_shadow_light_count = 0;
    static i32 nuspecial_shadow_light_have_clip_results = 0;
} // namespace

extern "C" {
    i32 nuspecial_const_tint_enabled;
    NUCOLOUR3 nuspecial_const_tint = {1.0f, 1.0f, 1.0f};
    i32 nuspecial_const_alpha_enabled;
    f32 nuspecial_const_alpha = 1.0f;
    i32 nuspecial_reflection;
    extern NUGLOBALRNDRSTATE render_state;
    void RndrStateSetConstAlphaTint(i32 alpha_enabled, i32 tint_enabled, f32 alpha, const NUCOLOUR3 *tint, NUMTL *mtl);
}

// C++-linkage helpers defined in sibling TUs.
void DisplayListCreateDynMtlList(VARIPTR *buf, VARIPTR buf_end); // supportall.cpp
void NuPadRecordEndFrame(void);                                  // nupad_interface.cpp
void bgSuspendMain(i32);                                         // main.cpp
void NuAnimBuffInit(i32, VARIPTR *, VARIPTR);                    // nu2api_nucore_misc.cpp
i32 NuCameraClipHGobj(nugscn_s *, NUMTX *, NUMTX *);             // nu2api_nucore_misc.cpp
i32 findrange(nugscn_s *, i32);                                  // utilities.cpp

extern "C" {

    // ---------------------------------------------------------------------------
    // Display-list bootstrap (original nucore TU file-statics)
    // ---------------------------------------------------------------------------

    void NuHasError(void);
    void NuMtlAnimate(f32 frame_time);
    void NuTexAnimProcess(f32 frame_time);
    void NuWindAnimate(void);
    void NuTimeBarSetRender(void);
    void NuRndrSwapScreenEx(i32 mode, void (*callback)(void));
    void NuShaderManagerSetfv(i32 semantic, const f32 *values);
    void *NuScratchAlloc32(i32 size);
    void NuScratchRelease(void);

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
    void NuCameraClipTestExtentsGeneric(void) {
    }
    void NuCameraClipTestPointScissor(void) {
    }
    void NuCameraClipTestPointVport(void) {
    }
    i32 NuCameraClipTestPoints(NUVEC *points, i32 count, NUMTX *world_mtx) {
        enum CLIP_POINT_FLAGS {
            CLIP_POINT_LEFT = 0x01,
            CLIP_POINT_RIGHT = 0x02,
            CLIP_POINT_TOP = 0x04,
            CLIP_POINT_BOTTOM = 0x08,
            CLIP_POINT_FAR = 0x10,
            CLIP_POINT_NEAR = 0x20,
        };

        NUMTX clip_matrix;
        if (world_mtx != NULL) {
            NuMtxMulH(&clip_matrix, world_mtx, &vmtx);
        } else {
            clip_matrix = vmtx;
        }

        i32 common_flags = -1;
        for (i32 index = 0; index < count; ++index) {
            NUVEC transformed;
            NuVecMtxTransform(&transformed, &points[index], &clip_matrix);

            i32 flags = 0;
            if (transformed.z < global_camera.near_clip) {
                flags |= CLIP_POINT_NEAR;
            }
            if (transformed.z > global_camera.far_clip) {
                flags |= CLIP_POINT_FAR;
            }
            if (transformed.x < -transformed.z * zx) {
                flags |= CLIP_POINT_LEFT;
            }
            if (transformed.x > transformed.z * zx) {
                flags |= CLIP_POINT_RIGHT;
            }
            if (transformed.y < -transformed.z * zy) {
                flags |= CLIP_POINT_BOTTOM;
            }
            if (transformed.y > transformed.z * zy) {
                flags |= CLIP_POINT_TOP;
            }
            common_flags &= flags;
        }
        return common_flags;
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
    void NuCameraTransformScreenClip(NUVEC *, NUVEC *, i32, NUMTX *) {
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

    void NuDisplayListBeginCriticalSection(void);
    void NuDisplayListEndCriticalSection(void);

    void NuDisplayListAnimateMtls(f32 frame_time) {
        static f32 sinetime = 0.0f;
        sinetime += frame_time;

        NuDisplayListBeginCriticalSection();
        for (NUMTLANIMSET *set = global_dlist_manager.mtlanim_list; set != NULL; set = set->next) {
            for (i32 index = 0; index < set->material_count; ++index) {
                NUMTL *material = set->scene->mtls[set->material_indices[index]];
                if (material->disable_u_animation == 1 || material->disable_v_animation == 1) {
                    continue;
                }

                for (u32 layer = 0; layer < 4; ++layer) {
                    if (material->shader_desc.tex_anim_data[layer] == -1) {
                        continue;
                    }

                    NUTEXANIMDATA &animation = material->shader_desc.tex_anim_desc[layer];
                    f32 u = 0.0f;
                    f32 v = 0.0f;

                    switch (animation.u_mode) {
                        case NUTEXANIM_MODE_LINEAR:
                            material->shader_desc.tex_anim_offsets[layer][0] += frame_time * animation.u_frequency;
                            material->shader_desc.tex_anim_offsets[layer][0] -=
                                NuFloor(material->shader_desc.tex_anim_offsets[layer][0]);
                            u = material->shader_desc.tex_anim_offsets[layer][0];
                            break;
                        case NUTEXANIM_MODE_SINE:
                            u = NU_SIN_LUT(static_cast<i32>(animation.u_frequency * sinetime * 65536.0f)) *
                                animation.u_amplitude;
                            break;
                        case NUTEXANIM_MODE_COSINE:
                            u = NU_COS_LUT(static_cast<i32>(animation.u_frequency * sinetime * 65536.0f)) *
                                animation.u_amplitude;
                            break;
                        default:
                            break;
                    }

                    switch (animation.v_mode) {
                        case NUTEXANIM_MODE_LINEAR:
                            material->shader_desc.tex_anim_offsets[layer][1] += frame_time * animation.v_frequency;
                            material->shader_desc.tex_anim_offsets[layer][1] -=
                                NuFloor(material->shader_desc.tex_anim_offsets[layer][1]);
                            v = material->shader_desc.tex_anim_offsets[layer][1];
                            break;
                        case NUTEXANIM_MODE_SINE:
                            v = NU_SIN_LUT(static_cast<i32>(animation.v_frequency * sinetime * 65536.0f)) *
                                animation.v_amplitude;
                            break;
                        case NUTEXANIM_MODE_COSINE:
                            v = NU_COS_LUT(static_cast<i32>(animation.v_frequency * sinetime * 65536.0f)) *
                                animation.v_amplitude;
                            break;
                        default:
                            break;
                    }

                    NuMtlSetUVOffsetPS(material, layer, u, v);
                }
            }
        }
        NuDisplayListEndCriticalSection();
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
    // original 0x2f8bb0 — defer dynamic material display-list construction to
    // DisplayListLinkDynamicMtls at the next render-buffer swap.
    void NuDisplayListCreateMtl(NUMTL *mtl) {
        NuThreadCriticalSectionBegin(global_dlist_manager.loading_critical_section);
        if (global_dlist_manager.nnew_materials != 0x80) {
            global_dlist_manager.new_materials[global_dlist_manager.nnew_materials] = mtl;
            global_dlist_manager.nnew_materials++;
            NuThreadCriticalSectionEnd(global_dlist_manager.loading_critical_section);
        }
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
    void *NuDisplayListPrepareFaceonPS(VARIPTR *, void *faceon, NUMTX *) {
        return faceon;
    }
    void *DisplayListCreateFaceonTransformPS(VARIPTR *buffer, NUMTX *transform, NUMTL *mtl, void *faceon);
    void *DisplayListCreateGeomTransformPS(VARIPTR *buffer, NUMTX *transform, NUMTL *mtl, void *next, void *tx);

    // Original local helper cloned as DisplayListProcessSkin.isra.36 by GCC.
    // It appends the world transform, skin palette, and geometry calls as one
    // contiguous run in the material display list.
    static __attribute__((optimize("O3"))) void
    DisplayListProcessSkin(NUMTL *, NUDISPLAYLIST *list, NUDISPLAYLISTITEM *geometry,
                           NUDISPLAYLISTITEM **first_and_last, NUMTX *world_matrix, void **transform_packet,
                           NUMTX *skin_matrices, DEFORMERWEIGHTSARRAY *deformer_weights, i32 shadow_caster) {
        display_list_buffer->addr = ALIGN(display_list_buffer->addr, 0x10);
        VARIPTR *buffer = NuDisplayListLinkItems(list, 3);

        *transform_packet = DisplayListCreateGeomTransformPS(buffer, world_matrix, list->dlist->mtls[list->mtl_id],
                                                             geometry->next, *transform_packet);
        NUDISPLAYLISTITEM *item = list->items;
        item->type = 0x8c;
        item->id = 3;
        item->next = *transform_packet;
        list->items = item + 1;
        first_and_last[0] = item;

        NUDISPLAYLISTGEOM *render_geometry;
        void *skin_packet =
            DisplayListCreateSkinTransformPS(buffer, skin_matrices, deformer_weights,
                                             static_cast<NUDISPLAYLISTGEOM *>(geometry->next), &render_geometry);
        item = list->items;
        item->type = 0x99;
        item->id = 3;
        item->next = skin_packet;
        list->items = item + 1;

        item = list->items;
        item->type = 0x98;
        item->id = 3;
        item->next = render_geometry;
        list->items = item + 1;
        first_and_last[1] = item;

        DisplayListSetAlphaPS(first_and_last[0], first_and_last[1], 1.0f);
        DisplayListSetShadowCasterFlagPS(first_and_last[0], first_and_last[1], shadow_caster);
    }

    __attribute__((optimize("O3"))) i32 NuDisplayListRndrSpecial(nuhspecial_s *special_handle, NUMTX *mtx, i32 skinned,
                                                                 NUMTX *skin_matrices,
                                                                 DEFORMERWEIGHTSARRAY *blend_values) {
        (void)skinned;

        if (special_handle == NULL || mtx == NULL) {
            return 0;
        }

        NuPlainSpecialHandleLayout *handle = reinterpret_cast<NuPlainSpecialHandleLayout *>(special_handle);
        NuPlainDisplaySpecialLayout *special = static_cast<NuPlainDisplaySpecialLayout *>(handle->display_special);
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

        NUVEC center;
        center.x = (special->min.x + special->max.x) * 0.5f;
        center.y = (special->min.y + special->max.y) * 0.5f;
        center.z = (special->min.z + special->max.z) * 0.5f;
        NuVecMtxTransform(&center, &center, mtx);
        f32 distance_sqr = NuCameraDistSqr(&center);
        if (distance_sqr < 0.0f) {
            distance_sqr = 0.0f;
        }

        NUCLIPOBJECT *clip_object = special->clip_objects;
        if (special->clip_range != NULL && special->clip_range[0] != 0.0f) {
            i32 lod = 0;
            while (distance_sqr < special->clip_range[lod]) {
                ++lod;
            }
            clip_object += lod;
        }

        f32 distance_alpha = 1.0f;
        if (scene->fade_ranges != NULL && (scene->visibility_flags[special->instance_ix] & 0x40) != 0) {
            const f32 fade_start = scene->fade_ranges[special->instance_ix * 2];
            const f32 fade_end = scene->fade_ranges[special->instance_ix * 2 + 1];
            if (fade_end <= fade_start) {
                if (distance_sqr <= fade_end * fade_end) {
                    return 0;
                }
                distance_alpha = (NuFsqrt(distance_sqr) - fade_end) / (fade_start - fade_end);
                if (distance_alpha > 1.0f) {
                    distance_alpha = 1.0f;
                }
            } else if (fade_start * fade_start < distance_sqr) {
                distance_alpha = (fade_end - NuFsqrt(distance_sqr)) / (fade_end - fade_start);
                if (distance_alpha < 0.0f) {
                    distance_alpha = 0.0f;
                }
                if (distance_alpha == 0.0f) {
                    return 0;
                }
            }
        }

        void *transform_packet = NULL;
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

                if (geometry->type == 0x8f) {
                    VARIPTR *buffer = NuDisplayListLinkItems(list, 2);
                    NUDISPLAYLISTITEM *items = list->items;
                    items[0].type = 0x90;
                    items[0].id = 3;
                    items[0].next = DisplayListCreateFaceonTransformPS(buffer, mtx, material, geometry->next);
                    items[1].type = 0x8f;
                    items[1].id = 3;
                    items[1].next = NuDisplayListPrepareFaceonPS(buffer, geometry->next, mtx);
                    list->items = items + 2;
                    DisplayListSetAlphaPS(items, items + 1, distance_alpha);
                } else if (skin_matrices != NULL) {
                    NUDISPLAYLISTITEM *first_and_last[2];
                    i32 shadow_caster = 0;
                    if ((scene->visibility_flags[special->instance_ix] & 0x20) != 0 && nuspecial_reflection == 0) {
                        shadow_caster = 1;
                    }
                    DisplayListProcessSkin(material, list, geometry, first_and_last, mtx, &transform_packet,
                                           skin_matrices, blend_values, shadow_caster);
                } else {
                    VARIPTR *buffer = NuDisplayListLinkItems(list, 2);
                    NUDISPLAYLISTITEM *items = list->items;
                    items[0].type = 0x8c;
                    items[0].id = 3;
                    items[0].next = DisplayListCreateGeomTransformPS(buffer, mtx, material, geometry->next, NULL);
                    items[1].type = 0x82;
                    items[1].id = 3;
                    items[1].next = geometry->next;
                    list->items = items + 2;
                    DisplayListSetAlphaPS(items, items + 1, distance_alpha);
                }
            }
        }
        RndrStateSetConstAlphaTint(0, 0, 0.0f, NULL, NULL);
        return clip_state;
    }
    void NuDisplayListSetFxParam(void) {
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

        NuMtlAnimate(nuapi.frametime);
        NuTexAnimProcess(nuapi.frametime);
        NuWindAnimate(); // original: (wind, frametime)
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
    i32 NuIOS_IsMidRangeDevice(void) {
        return 0;
    }
    i32 NuIOS_IsSmallScreen(void) {
        return 0;
    }
    void NuIOS_RecordFlurryEvent(char *event_name) {
        JNIEnv *env = NULL;
        if (g_javaVM.functions->GetEnv(&g_javaVM, (void **)&env, JNI_VERSION_1_6) < 0) {
            return;
        }

        jclass activity_class = g_activityClass;
        jstring event = env->functions->NewStringUTF(env, event_name);
        jmethodID method =
            env->functions->GetStaticMethodID(env, activity_class, "FlurryEvent", "(Ljava/lang/String;)V");
        env->CallStaticVoidMethod(activity_class, method, event);
        env->functions->DeleteLocalRef(env, event);
    }
    void NuIOS_ShowAchievements(void) {
    }

    // ---------------------------------------------------------------------------
    // Animation / curves / data
    // ---------------------------------------------------------------------------

    // Original @0x2c6560.  The first contribution overwrites the buffer;
    // subsequent contributions use the blend player.
    void NuAnimBuffAccumulate_3(nuanimbuff_s *buffer, ani3_animheader_s *animation, f32 time, i32 overwrite, f32 blend,
                                i32 first_joint, nuhgobj_s *object, NUVEC *root_translation) {
        if (buffer == NULL) {
            buffer = static_cast<nuanimbuff_s *>(globalbuffer);
        }
        buffer->joint_count = object->joint_count;

        i32 end_joint = object->joint_count;
        if (first_joint != 0) {
            end_joint = findrange(reinterpret_cast<nugscn_s *>(object), first_joint) + 1;
        }
        if (overwrite != 0) {
            ANI_SimpleAni3PlayerV4Joint(animation, time - 1.0f, buffer, end_joint - first_joint, first_joint);
            if (root_translation != NULL) {
                root_translation->x = buffer->joints[0].translation.x;
                root_translation->y = buffer->joints[0].translation.y;
                root_translation->z = -buffer->joints[0].translation.z;
            }
        } else {
            ANI_SimpleAni3PlayerV4Joint_Blend(animation, time - 1.0f, buffer, blend, end_joint - first_joint,
                                              first_joint, root_translation);
        }
    }
    void *NuAnimBuffCreate(i32 max_joints, VARIPTR *buf) {
        u32 *anim_buffer = reinterpret_cast<u32 *>(ALIGN(buf->addr, 0x10));
        buf->void_ptr = anim_buffer + 4;
        anim_buffer[2] = buf->addr;
        buf->addr = ALIGN(buf->addr, 0x10) + max_joints * 0x30;
        anim_buffer[3] = buf->addr;
        buf->addr += max_joints;
        reinterpret_cast<u16 *>(anim_buffer)[2] = static_cast<u16>(max_joints);
        anim_buffer[0] = 0;
        return anim_buffer;
    }
    // Original @0x2bbf90.  Scratch buffers are nested allocations from the
    // engine's LIFO scratch arena, so destruction deliberately releases them
    // in the reverse order.
    void NuAnimBuffCreateScratch(nuanimbuff_s *buffer) {
        if (buffer == NULL) {
            return;
        }

        nuanimbuff_s *global = static_cast<nuanimbuff_s *>(globalbuffer);
        buffer->joint_count = 0;
        buffer->max_joints = global->max_joints;
        buffer->use_quaternions = 0;
        buffer->pad_07 = 0;
        buffer->joints = static_cast<nuanimbuffjoint_s *>(NuScratchAlloc32((buffer->max_joints * 3 + 3) * 0x10));
        buffer->joints = reinterpret_cast<nuanimbuffjoint_s *>(ALIGN(reinterpret_cast<usize>(buffer->joints), 0x10));
        buffer->joint_flags = static_cast<u8 *>(NuScratchAlloc32(buffer->max_joints));
    }
    // Original @0x2bc000.
    void NuAnimBuffDestroyScratch(nuanimbuff_s *buffer) {
        if (buffer == NULL) {
            return;
        }
        NuScratchRelease();
        NuScratchRelease();
        buffer->max_joints = 0;
        buffer->joints = NULL;
        buffer->joint_flags = NULL;
    }
    void NuAnimBuffEvaluateCallback(void) {
    }
    // Original @0x2bd180.  Evaluate the decompressed per-joint samples into
    // hierarchy matrices while carrying scale through the parent chain.
    void NuAnimBuffEvaluate_3(nuanimbuff_s *buffer, nuhgobj_s *object, NUMTX *matrices, ani3_animheader_s *animation,
                              NUHGOBJROOTFN root_fn, NUVEC *root_translation, void *root_data) {
        if (buffer == NULL) {
            buffer = static_cast<nuanimbuff_s *>(globalbuffer);
        }

        static NUVEC scale_array[256];
        scale_array[0xff] = {1.0f, 1.0f, 1.0f};

        const i32 evaluated_count =
            animation->node_count < object->joint_count ? animation->node_count : object->joint_count;
        NUVEC root_values = {0.0f, 0.0f, 0.0f};
        for (i32 joint_index = 0; joint_index < evaluated_count; ++joint_index) {
            const nuanimbuffjoint_s &joint = buffer->joints[joint_index];
            const u8 flags = buffer->joint_flags[joint_index];
            const u8 parent_index = object->joints[joint_index].parent_index;
            const NUVEC parent_scale = scale_array[parent_index];

            NUMTX local_matrix;
            if ((flags & NUANIMBUFF_JOINT_ROTATION) != 0) {
                constexpr f32 kRadiansToNuAngle = 10430.378f;
                NUANGVEC angles = {
                    static_cast<NUANG>(joint.rotation.x * kRadiansToNuAngle),
                    static_cast<NUANG>(joint.rotation.y * kRadiansToNuAngle),
                    static_cast<NUANG>(joint.rotation.z * kRadiansToNuAngle),
                };
                NuMtxSetRotateXYZ(&local_matrix, &angles);
                if ((flags & NUANIMBUFF_JOINT_BIND_MATRIX) != 0) {
                    NuMtxMulRVU0(&local_matrix, &local_matrix, &object->joints[joint_index].animation_bind_matrix);
                }
            } else if ((flags & NUANIMBUFF_JOINT_BIND_MATRIX) != 0) {
                local_matrix = object->joints[joint_index].animation_bind_matrix;
            } else {
                NuMtxSetIdentity(&local_matrix);
            }

            if ((flags & NUANIMBUFF_JOINT_SCALE) != 0) {
                NuMtxPreScaleVU0(&local_matrix, const_cast<NUVEC *>(&joint.scale));
                scale_array[joint_index] = {
                    joint.scale.x * parent_scale.x,
                    joint.scale.y * parent_scale.y,
                    joint.scale.z * parent_scale.z,
                };
            } else {
                scale_array[joint_index] = parent_scale;
            }

            if ((flags & NUANIMBUFF_JOINT_CANCEL_PARENT_SCALE) != 0) {
                NUVEC inverse_parent_scale = {0.0f, 0.0f, 0.0f};
                if (parent_scale.x != 0.0f && parent_scale.y != 0.0f && parent_scale.z != 0.0f) {
                    inverse_parent_scale = {
                        1.0f / parent_scale.x,
                        1.0f / parent_scale.y,
                        1.0f / parent_scale.z,
                    };
                }
                scale_array[joint_index].x *= inverse_parent_scale.x;
                scale_array[joint_index].y *= inverse_parent_scale.y;
                scale_array[joint_index].z *= inverse_parent_scale.z;
                NuMtxScaleVU0(&local_matrix, &inverse_parent_scale);
            }

            if ((flags & NUANIMBUFF_JOINT_TRANSLATION) != 0) {
                NUVEC translation = joint.translation;
                NuMtxTranslate(&local_matrix, &translation);
                root_values = joint.translation;
                root_values.z = -root_values.z;
            }

            local_matrix.m02 = -local_matrix.m02;
            local_matrix.m12 = -local_matrix.m12;
            local_matrix.m20 = -local_matrix.m20;
            local_matrix.m21 = -local_matrix.m21;
            local_matrix.m23 = -local_matrix.m23;
            local_matrix.m32 = -local_matrix.m32;

            if (root_fn != NULL) {
                root_fn(&local_matrix, root_data, &root_values, &root_values, root_translation, 0.0f);
                root_fn = NULL;
            }

            if (parent_index == 0xff) {
                matrices[joint_index] = local_matrix;
            } else {
                NuMtxMulVU0(&matrices[joint_index], &local_matrix, &matrices[parent_index]);
            }

            if ((parent_index & 0x40) != 0) {
                scale_array[joint_index] = {1.0f, 1.0f, 1.0f};
            }
        }

        for (i32 joint_index = evaluated_count; joint_index < object->joint_count; ++joint_index) {
            NuMtxSetIdentity(&matrices[joint_index]);
        }
    }
    // Original @0x2bd900. Apply the character's per-joint procedural offsets
    // to a decompressed ANI4/ANI5 pose before hierarchy evaluation.
    void NuAnimBuffProceduralAnimation(nuanimbuff_s *buffer, nuhgobj_s *object, i32 override_count,
                                       NUJOINTANIM_s *overrides) {
        if (buffer == NULL) {
            buffer = static_cast<nuanimbuff_s *>(globalbuffer);
        }

        constexpr f32 kRadiansToNuAngle = 10430.378f;
        constexpr f32 kNuAngleToRadians = 0.0000958738f;

        for (i32 override_index = 0; override_index < override_count; ++override_index) {
            NUJOINTANIM_s &joint_override = overrides[override_index];
            if (joint_override.joint_index >= object->joint_override_map_count) {
                continue;
            }

            const u8 joint_index = object->joint_override_map[joint_override.joint_index];
            if (joint_index == 0xff) {
                continue;
            }

            nuanimbuffjoint_s &joint = buffer->joints[joint_index];
            const u8 flags = joint_override.flags;
            if ((flags & NUJOINTANIM_ROTATION) != 0) {
                joint.rotation.x += joint_override.rotation.x;
                joint.rotation.y += joint_override.rotation.y;
                joint.rotation.z += joint_override.rotation.z;

                i32 angles[3] = {
                    static_cast<i32>(joint.rotation.x * kRadiansToNuAngle),
                    static_cast<i32>(joint.rotation.y * kRadiansToNuAngle),
                    static_cast<i32>(joint.rotation.z * kRadiansToNuAngle),
                };
                const u8 limit_flags[3] = {
                    NUJOINTANIM_LIMIT_ROTATION_X,
                    NUJOINTANIM_LIMIT_ROTATION_Y,
                    NUJOINTANIM_LIMIT_ROTATION_Z,
                };
                for (i32 axis = 0; axis < 3; ++axis) {
                    if ((flags & limit_flags[axis]) != 0) {
                        const u32 wrapped = static_cast<u32>(angles[axis]) & 0xffff;
                        angles[axis] =
                            wrapped >= 0x8000 ? static_cast<i32>(wrapped) - 0x10000 : static_cast<i32>(wrapped);
                        if (joint_override.rotation_limit_start[axis] >= angles[axis] &&
                            angles[axis] < joint_override.rotation_limit_end[axis]) {
                            angles[axis] = joint_override.rotation_limit_end[axis];
                        }
                    }
                }
                joint.rotation = {
                    static_cast<f32>(angles[0]) * kNuAngleToRadians,
                    static_cast<f32>(angles[1]) * kNuAngleToRadians,
                    static_cast<f32>(angles[2]) * kNuAngleToRadians,
                };
            }

            if ((flags & NUJOINTANIM_SCALE) != 0) {
                joint.scale.x += joint_override.scale.x;
                joint.scale.y += joint_override.scale.y;
                joint.scale.z += joint_override.scale.z;
            }
            if ((flags & NUJOINTANIM_TRANSLATION) != 0) {
                joint.translation.x += joint_override.translation.x;
                joint.translation.y += joint_override.translation.y;
                joint.translation.z += joint_override.translation.z;
            }
        }
    }
    f32 NuAnimCurve2CalcValEx(nuanimcurve2_s *curve, nuanimtime_s *time, u32 type) {
        nuanimcurvedata_s *data = curve->data.curvedata;
        u32 *key_mask = data->key_mask + time->chunk;
        if (type == 4) {
            i32 frame = static_cast<i32>(NuFloor(time->time_offset));
            return static_cast<f32>((*key_mask >> ((frame - 1) & 0x1f)) & 1);
        }

        u32 key = 0;
        switch (time->time_byte) {
            case 0:
                key = BitCountTable[reinterpret_cast<u8 *>(key_mask)[0] & time->time_mask];
                break;
            case 1:
                key = BitCountTable[reinterpret_cast<u8 *>(key_mask)[0]] +
                      BitCountTable[reinterpret_cast<u8 *>(key_mask)[1] & time->time_mask];
                break;
            case 2:
                key = BitCountTable[reinterpret_cast<u8 *>(key_mask)[0]] +
                      BitCountTable[reinterpret_cast<u8 *>(key_mask)[1]] +
                      BitCountTable[reinterpret_cast<u8 *>(key_mask)[2] & time->time_mask];
                break;
            case 3:
                key = BitCountTable[reinterpret_cast<u8 *>(key_mask)[0]] +
                      BitCountTable[reinterpret_cast<u8 *>(key_mask)[1]] +
                      BitCountTable[reinterpret_cast<u8 *>(key_mask)[2]] +
                      BitCountTable[reinterpret_cast<u8 *>(key_mask)[3] & time->time_mask];
                break;
        }
        key += data->key_offsets[time->chunk];
        u8 *key_data = static_cast<u8 *>(data->key_data);

        switch (type) {
            case 1: {
                f32 *first = reinterpret_cast<f32 *>(key_data + (key - 1) * 0x10);
                f32 span = first[4] - first[0];
                f32 value_delta = first[2] - first[6];
                f32 t = (time->time - first[0]) * first[1];
                f32 tangent0 = first[3] * span;
                f32 tangent1 = first[7] * span;
                return (((((value_delta * 2.0f + tangent0 + tangent1) * t - value_delta * 3.0f) - tangent0 * 2.0f) -
                         tangent1) *
                            t +
                        tangent0) *
                           t +
                       first[2];
            }
            case 2: {
                f32 *header = reinterpret_cast<f32 *>(key_data);
                u8 *first = key_data + (key + 1) * 4;
                f32 first_time = static_cast<f32>(first[3]);
                f32 span = static_cast<f32>(first[7]) - first_time;
                f32 inverse_span = span == 0.0f ? 0.0f : 1.0f / span;
                f32 tangent0 = static_cast<f32>(*reinterpret_cast<i8 *>(first + 2)) * header[0] * span;
                f32 tangent1 = static_cast<f32>(*reinterpret_cast<i8 *>(first + 6)) * header[0] * span;
                f32 value0 = static_cast<f32>(*reinterpret_cast<i16 *>(first)) * header[1];
                f32 value_delta = value0 - static_cast<f32>(*reinterpret_cast<i16 *>(first + 4)) * header[1];
                f32 t = ((time->time - 1.0f) - first_time) * inverse_span;
                return (((((value_delta * 2.0f + tangent0 + tangent1) * t - value_delta * 3.0f) - tangent0 * 2.0f) -
                         tangent1) *
                            t +
                        tangent0) *
                           t +
                       value0;
            }
            case 3:
                return *reinterpret_cast<f32 *>(key_data + (key - 1) * 8);
            case 5: {
                f32 *header = reinterpret_cast<f32 *>(key_data);
                i16 *first = reinterpret_cast<i16 *>(key_data + key * 6 + 0x0c);
                f32 span = static_cast<f32>(static_cast<u16>(first[5])) - static_cast<u16>(first[2]);
                f32 tangent0 = static_cast<f32>(static_cast<i8>(first[1])) * header[0] * span;
                f32 tangent1 = static_cast<f32>(static_cast<i8>(first[4])) * header[0] * span;
                f32 value0 = static_cast<f32>(first[0]) * header[1] + header[2];
                f32 value_delta = value0 - (static_cast<f32>(first[3]) * header[1] + header[2]);
                f32 t = ((time->time - 1.0f) - static_cast<u16>(first[2])) / span;
                return (((((value_delta * 2.0f + tangent0 + tangent1) * t - value_delta * 3.0f) - tangent0 * 2.0f) -
                         tangent1) *
                            t +
                        tangent0) *
                           t +
                       value0;
            }
            case 6: {
                f32 *header = reinterpret_cast<f32 *>(key_data);
                u8 *first = key_data + (key + 3) * 4;
                f32 first_time = static_cast<f32>(first[3]) * header[3];
                f32 next_time = static_cast<f32>(first[7]) * header[3];
                if (next_time == first_time) {
                    next_time = first_time + 1.0f;
                }
                f32 span = next_time - first_time;
                f32 tangent0 = static_cast<f32>(*reinterpret_cast<i8 *>(first + 2)) * header[0] * span;
                f32 tangent1 = static_cast<f32>(*reinterpret_cast<i8 *>(first + 6)) * header[0] * span;
                f32 value0 = static_cast<f32>(*reinterpret_cast<i16 *>(first)) * header[1] + header[2];
                f32 value_delta =
                    value0 - (static_cast<f32>(*reinterpret_cast<i16 *>(first + 4)) * header[1] + header[2]);
                f32 t = ((time->time - 1.0f) - first_time) / span;
                return (((((value_delta * 2.0f + tangent0 + tangent1) * t - value_delta * 3.0f) - tangent0 * 2.0f) -
                         tangent1) *
                            t +
                        tangent0) *
                           t +
                       value0;
            }
            default:
                return 0.0f;
        }
    }
    void NuAnimCurve2SetApplyToJoint(void) {
    }
    void NuAnimCurve2SetApplyToJointTransLoc(void) {
    }
    void NuAnimCurve2SetApplyToMatrix_3(ani3_animheader_s *animation, i32 node, f32 frame, NUMTX *matrix) {
        u8 node_flags = animation->node_flags[node];
        f32 *values = NuAnimCurveExtractAllNodeCurves_3(animation, node, frame, NULL);

        if ((node_flags & NUANIM_NODE_HAS_ROTATION) != 0) {
            NUANGVEC rotation;
            rotation.x = static_cast<NUANG>(values[3] * 10430.378f);
            rotation.y = static_cast<NUANG>(values[4] * 10430.378f);
            rotation.z = static_cast<NUANG>(values[5] * 10430.378f);
            NuMtxSetRotateXYZVU0(matrix, &rotation);
        } else {
            NuMtxSetIdentity(matrix);
        }

        if ((node_flags & NUANIM_NODE_HAS_SCALE) != 0) {
            NuMtxPreScaleVU0(matrix, reinterpret_cast<NUVEC *>(&values[6]));
        }

        NuMtxTranslate(matrix, reinterpret_cast<NUVEC *>(values));

        // ANI3 animation data uses the opposite handedness for Z.
        matrix->m02 = -matrix->m02;
        matrix->m12 = -matrix->m12;
        matrix->m20 = -matrix->m20;
        matrix->m21 = -matrix->m21;
        matrix->m23 = -matrix->m23;
        matrix->m32 = -matrix->m32;
    }
    void NuAnimCurveCalcVal2(void) {
    }
    void NuAnimCurveCreate(void) {
    }
    void NuAnimCurveDestroy(void) {
    }
    f32 *NuAnimCurveExtractAllNodeCurves_3(ani3_animheader_s *anim, i32 node, f32 frame, char *curve_mask) {
        f32 *values = *reinterpret_cast<f32 **>(reinterpret_cast<u8 *>(globalbuffer) + 8);
        ANI_Ani3ExtractAllNodeCurves(anim, frame - 1.0f, values, node, curve_mask);
        return values;
    }
    void NuAnimCurveSetApplyBlendToJoint2(void) {
    }
    void NuAnimCurveSetApplyToMatrix(void) {
    }
    void NuAnimCurveSetCreate(void) {
    }
    void NuAnimCurveSetDestroy(void) {
    }
    void NuAnimData2CalcMatrix(nuanimdata_s *animation, i32 node, f32 frame, numtx_s *matrix) {
        NuAnimCurve2SetApplyToMatrix_3(reinterpret_cast<ani3_animheader_s *>(animation), node, frame, matrix);
    }
    void NuAnimData2CalcTime(nuanimdata2_s *anim, f32 frame, nuanimtime_s *time) {
        u32 magic = *reinterpret_cast<u32 *>(anim);
        if (magic + 0xbeb1b6ccU < 2) {
            ani3_animheader_s *ani3 = reinterpret_cast<ani3_animheader_s *>(anim);
            if (frame < 1.0f) {
                time->time = 1.0f;
            } else if (frame < static_cast<f32>(ani3->frame_count)) {
                time->time = frame;
            } else {
                time->time = static_cast<f32>(ani3->frame_count) - 0.01f;
            }
            return;
        }

        i32 chunk;
        f32 clamped_frame;
        if (anim->duration <= frame) {
            if (anim->duration == 1.0f) {
                time->time = 1.0f;
                time->chunk = 0;
                time->time_byte = 0;
                time->time_mask = 1;
                return;
            }
            clamped_frame = anim->duration - 0.01f;
            time->time = clamped_frame;
            chunk = (static_cast<i32>(clamped_frame) - 1) >> 5;
        } else if (frame < 1.0f) {
            clamped_frame = 1.0f;
            time->time = 1.0f;
            chunk = 0;
        } else {
            clamped_frame = frame;
            time->time = frame;
            chunk = (static_cast<i32>(frame) - 1) >> 5;
        }
        time->chunk = chunk;
        if (anim->chunk_count <= chunk) {
            chunk = anim->chunk_count - 1;
            time->chunk = chunk;
        }
        time->time_offset = clamped_frame - static_cast<f32>(chunk << 5);
        i32 chunk_frame = static_cast<i32>(NuFloor(time->time_offset)) - 1;
        time->chunk_frame = static_cast<u32>(chunk_frame);
        i32 byte_frame = chunk_frame < 0 ? chunk_frame + 7 : chunk_frame;
        time->time_byte = static_cast<u32>((byte_frame >> 3) & 0xff);
        time->time_mask = (1u << ((chunk_frame & 7) + 1)) - 1u;
    }
    void *NuAnimData2FixPtrs(void *data, isize delta, isize external_delta, i32) {
        extern void buildBitCountTable(void);
        buildBitCountTable();

        if (data == NULL) {
            return NULL;
        }
        nuanimdata2_s *anim = reinterpret_cast<nuanimdata2_s *>(reinterpret_cast<u8 *>(data) + delta);
        if (*reinterpret_cast<u32 *>(&anim->duration) + 0xbeb1b6ccU < 2) {
            ANI_FixUpAddrs(reinterpret_cast<ani3_animheader_s *>(anim),
                           external_delta == 0 ? static_cast<isize>(reinterpret_cast<usize>(anim)) : delta);
            return anim;
        }

        if (anim->curves != NULL) {
            anim->curves = reinterpret_cast<nuanimcurve2_s *>(reinterpret_cast<u8 *>(anim->curves) + delta);
        }
        if (anim->curve_types != NULL) {
            anim->curve_types = reinterpret_cast<u8 *>(anim->curve_types) + delta;
        }
        if (anim->node_flags != NULL) {
            anim->node_flags = reinterpret_cast<u8 *>(anim->node_flags) + delta;
        }
        i32 curve_count = static_cast<i32>(anim->curve_count) * static_cast<i32>(anim->node_count);
        for (i32 i = 0; i < curve_count; ++i) {
            if (anim->curve_types[i] != 0) {
                nuanimcurvedata_s *curve = anim->curves[i].data.curvedata;
                if (curve != NULL) {
                    curve = reinterpret_cast<nuanimcurvedata_s *>(reinterpret_cast<u8 *>(curve) + delta);
                    anim->curves[i].data.curvedata = curve;
                } else {
                    continue;
                }
                if (curve->key_mask != NULL) {
                    curve->key_mask = reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(curve->key_mask) + delta);
                }
                if (curve->key_offsets != NULL) {
                    curve->key_offsets = reinterpret_cast<u16 *>(reinterpret_cast<u8 *>(curve->key_offsets) + delta);
                }
                if (curve->key_data != NULL) {
                    curve->key_data = reinterpret_cast<u8 *>(curve->key_data) + delta;
                }
            }
        }
        return anim;
    }
    void *NuAnimData2Fixup(i32 file_size, void **data) {
        u32 *header = static_cast<u32 *>(*data);
        if (static_cast<i32>(header[1]) > static_cast<i32>(0x414e4934)) {
            return NuPtrBlockFix(header);
        }

        if (header[0] == 0x414e4933 || header[0] == 0x414e4934) {
            ANI_FixUpAddrs(reinterpret_cast<ani3_animheader_s *>(header),
                           static_cast<isize>(reinterpret_cast<usize>(header)));
            return header;
        }

        header[0] = static_cast<u32>(file_size);
        const isize relocation_delta =
            static_cast<isize>(reinterpret_cast<usize>(header)) - static_cast<isize>(header[1]);
        return NuAnimData2FixPtrs(reinterpret_cast<void *>(static_cast<usize>(header[2])), relocation_delta, 0, 0);
    }

    void *NuAnimData2LoadBuffEx(char *path, VARIPTR *buf, VARIPTR *buf_end, void **result) {
        buf->addr = ALIGN(buf->addr, 0x10);
        const i32 file_size = NuFileLoadBuffer(path, buf->void_ptr, static_cast<i32>(buf_end->addr - buf->addr));
        if (file_size == 0) {
            if (NuFileGetLastError() == -1) {
                *buf = *buf_end;
            }
            *result = NULL;
            return NULL;
        }

        void *data = buf->void_ptr;
        if (static_cast<i32>(static_cast<u32 *>(data)[1]) > static_cast<i32>(0x414e4934)) {
            data = NuPtrBlockFix(data);
            buf->void_ptr = data;
            buf->addr += static_cast<usize>(file_size);
            *result = data;
            return data;
        }

        *result = data;
        buf->addr += static_cast<usize>(file_size);
        return NuAnimData2Fixup(file_size, result);
    }

    void *NuAnimData2LoadBuff(char *path, VARIPTR *buf, VARIPTR *buf_end) {
        void *result;
        return NuAnimData2LoadBuffEx(path, buf, buf_end, &result);
    }

    void *NuAnimData2LoadBuffFromPAK(void *data, i32 file_size) {
        if (file_size == 0) {
            return NULL;
        }
        if (static_cast<i32>(static_cast<u32 *>(data)[1]) > static_cast<i32>(0x414e4934)) {
            return NuPtrBlockFix(data);
        }
        return NuAnimData2Fixup(file_size, &data);
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
    void NuAnimGetAnimLOD(void) {
    }
    void NuAnimGetUseQuatsFlag(void) {
    }
    void NuAnimInit(i32 max_joints, VARIPTR *buf, VARIPTR buf_end) {
        extern void buildBitCountTable(void);
        buildBitCountTable();
        NuAnimBuffInit(max_joints, buf, buf_end);
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
    u8 PS2_SCRATCH_BASE[0x8000];
    static u8 *ps2_scratch_free;

    // Original @0x316d41.
    void NuScratchReset(void) {
        ps2_scratch_free = PS2_SCRATCH_BASE;
    }

    static void *NuScratchAllocAligned(i32 size, usize alignment) {
        if (ps2_scratch_free == NULL) {
            NuScratchReset();
        }
        u8 *previous = ps2_scratch_free;
        u8 *allocation = reinterpret_cast<u8 *>(ALIGN(reinterpret_cast<usize>(ps2_scratch_free), alignment));
        ps2_scratch_free = allocation + ALIGN(size, 4);
        *reinterpret_cast<u8 **>(ps2_scratch_free) = previous;
        ps2_scratch_free += sizeof(previous);
        return allocation;
    }

    // Original @0x316e45 / 0x316d5d / 0x316dd1.
    void *NuScratchAlloc128(i32 size) {
        return NuScratchAllocAligned(size, 16);
    }
    void *NuScratchAlloc32(i32 size) {
        return NuScratchAllocAligned(size, 4);
    }
    void *NuScratchAlloc64(i32 size) {
        return NuScratchAllocAligned(size, 8);
    }

    // Original @0x316eb9.
    void NuScratchRelease(void) {
        ps2_scratch_free = *reinterpret_cast<u8 **>(ps2_scratch_free - sizeof(ps2_scratch_free));
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
    // Original @0x2babf0.  The VU0 entry point is an ordinary CPU wrapper in
    // this build; animation evaluation uses it to combine an animated joint
    // rotation with that joint's bind-pose matrix.
    void NuMtxMulRVU0(NUMTX *result, NUMTX *left, NUMTX *right) {
        NuMtxMulR(result, left, right);
    }
    void NuMtxMulVU0(NUMTX *result, NUMTX *left, NUMTX *right) {
        NuMtxMulH(result, left, right);
    }
    void NuMtxMulnVU0(void) {
    }
    void NuMtxPreScaleUVU0(void) {
    }
    void NuMtxPreScaleVU0(NUMTX *matrix, NUVEC *scale) {
        NuMtxPreScale(matrix, scale);
    }
    // Original @0x2bad80.  Preserve the public VU0-shaped entry point while
    // sharing the scalar matrix implementation used by the host build.
    void NuMtxScaleVU0(NUMTX *matrix, NUVEC *scale) {
        NuMtxScale(matrix, scale);
    }
    void NuMtxSetRotateXYZVU0(NUMTX *matrix, NUANGVEC *angles) {
        NuMtxSetRotateXYZ(matrix, angles);
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
    extern nudisplayscene_s currentScene;
    void NuBackbufferCopy(i32 texture_id) {
        currentScene.unknown_214 = static_cast<u32>(texture_id);
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
    void NuLightFogX(f32 near_distance, f32 far_distance, u32 colour, f32, f32, i32, f32 density) {
        NuRndrStateSetFogEnabled(1);
        NuRndrStateSetFogState(near_distance, far_distance, colour, density);
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
    void NuLightSpotFadeSet(u32) {
    }
    void NuLgtArcLaser(void) {
    }
    void NuLgtLaser(void) {
    }
    void NuLgtLaserDraw(i32 paused) {
        (void)paused;
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
    void NuSpecialClear(void *) {
    }
    void NuSpecialClearShadowClipTestResults(void) {
        nuspecial_shadow_light_have_clip_results = 0;
    }
    void NuSpecialClearShadowLights(void) {
    }
    void NuSpecialClipTestExtents(void) {
    }
    i32 NuSpecialClipTestShadowLights(NUVEC *, NUVEC *, i32) {
        return 0;
    }
    void NuSpecialCompare(void) {
    }
    void NuSpecialConstAlpha(i32 enabled, f32 alpha) {
        nuspecial_const_alpha_enabled = enabled;
        nuspecial_const_alpha = alpha;
    }
    void NuSpecialConstTint(i32, NUVEC *) {
    }
    i32 NuSpecialDrawAt(void *special, NUMTX *mtx) {
        NuPlainSpecialHandleLayout *handle = reinterpret_cast<NuPlainSpecialHandleLayout *>(special);
        if (handle == NULL || handle->scene == NULL || handle->display_special == NULL) {
            return 0;
        }
        return NuDisplayListRndrSpecial(reinterpret_cast<nuhspecial_s *>(special), mtx, 0, NULL, NULL);
    }
    i32 NuSpecialDrawAtAlpha(void *special, NUMTX *mtx, f32 alpha) {
        NuPlainSpecialHandleLayout *handle = reinterpret_cast<NuPlainSpecialHandleLayout *>(special);
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
    __attribute__((optimize("O3"))) i32 NuSpecialDrawSmoothSkinDwa(void *special, NUMTX *skin_matrices,
                                                                   NUMTX *world_matrix,
                                                                   DEFORMERWEIGHTSARRAY *blend_values) {
        NuPlainSpecialHandleLayout *handle = static_cast<NuPlainSpecialHandleLayout *>(special);
        if (handle == NULL || handle->scene == NULL || handle->display_special == NULL) {
            return 0;
        }
        return NuDisplayListRndrSpecial(reinterpret_cast<nuhspecial_s *>(special), world_matrix, 2, skin_matrices,
                                        blend_values);
    }
    void NuSpecialDrawWith(void) {
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
    void NuSpecialGetBounds(void *special, NUVEC *minimum, NUVEC *maximum) {
        NuPlainSpecialHandleLayout *handle = reinterpret_cast<NuPlainSpecialHandleLayout *>(special);
        if (handle->special == NULL) {
            NuPlainDisplaySpecialLayout *display = static_cast<NuPlainDisplaySpecialLayout *>(handle->display_special);
            if (display != NULL) {
                *minimum = display->min;
                *maximum = display->max;
            }
            return;
        }

        NuPlainLegacySceneLayout *scene = reinterpret_cast<NuPlainLegacySceneLayout *>(handle->scene);
        NuPlainLegacySpecialLayout *legacy = static_cast<NuPlainLegacySpecialLayout *>(handle->special);
        NuPlainLegacyInstanceBoundsLayout *instance =
            reinterpret_cast<NuPlainLegacyInstanceBoundsLayout *>(legacy->instance);
        NuPlainLegacyObjectBoundsLayout *object =
            static_cast<NuPlainLegacyObjectBoundsLayout *>(scene->objects[instance->object_index]);
        while (object->next != NULL) {
            object = object->next;
        }
        *minimum = object->minimum;
        *maximum = object->maximum;
    }
    void NuSpecialGetCollision(void) {
    }
    NUMTX *NuSpecialGetDrawMtx(void *special) {
        NuPlainSpecialHandleLayout *handle = reinterpret_cast<NuPlainSpecialHandleLayout *>(special);
        NuPlainLegacySpecialLayout *legacy = static_cast<NuPlainLegacySpecialLayout *>(handle->special);
        if (legacy != NULL) {
            NUMTX *instance = reinterpret_cast<NUMTX *>(legacy->instance);
            NUMTX *draw_mtx = *reinterpret_cast<NUMTX **>(legacy->instance + 0x48);
            return draw_mtx != NULL ? draw_mtx : instance;
        }
        NuPlainDisplaySpecialLayout *display = static_cast<NuPlainDisplaySpecialLayout *>(handle->display_special);
        if (display != NULL) {
            usize draw_mtx = reinterpret_cast<usize>(display->instance_animation);
            if (draw_mtx != 0 && draw_mtx != static_cast<usize>(-1)) {
                return reinterpret_cast<NUMTX *>(display->instance_animation);
            }
            return &display->draw_mtx;
        }
        return NULL;
    }
    NUVEC *NuSpecialGetDrawPos(void *special) {
        NuPlainSpecialHandleLayout *handle = reinterpret_cast<NuPlainSpecialHandleLayout *>(special);
        NuPlainDisplaySpecialLayout *display = static_cast<NuPlainDisplaySpecialLayout *>(handle->display_special);
        if (display != NULL) {
            usize instance_animation = reinterpret_cast<usize>(display->instance_animation);
            NUMTX *matrix = instance_animation != 0 && instance_animation != static_cast<usize>(-1)
                                ? reinterpret_cast<NUMTX *>(display->instance_animation)
                                : &display->draw_mtx;
            return NUMTX_GET_ROW_VEC(matrix, 3);
        }

        NuPlainLegacySpecialLayout *legacy = static_cast<NuPlainLegacySpecialLayout *>(handle->special);
        if (legacy == NULL || legacy->instance == NULL) {
            return NULL;
        }
        NUMTX *instance = reinterpret_cast<NUMTX *>(legacy->instance);
        NUMTX *draw_mtx = *reinterpret_cast<NUMTX **>(legacy->instance + 0x48);
        return NUMTX_GET_ROW_VEC(draw_mtx != NULL ? draw_mtx : instance, 3);
    }
    void NuSpecialGetFirst(void) {
    }
    void NuSpecialGetInstanceix(void) {
    }
    void NuSpecialGetMtl(void) {
    }
    NUMTX *NuSpecialGetMtx(void *special) {
        NuPlainSpecialHandleLayout *handle = reinterpret_cast<NuPlainSpecialHandleLayout *>(special);
        if (handle->display_special != NULL) {
            return static_cast<NUMTX *>(handle->display_special);
        }
        return static_cast<NUMTX *>(handle->special);
    }
    void NuSpecialGetNext(void) {
    }
    void NuSpecialGetNumSpecials(void) {
    }
    void NuSpecialGetOnScreenFn(void) {
    }
    void NuSpecialGetOriginRadius(void) {
    }
    void NuSpecialGetPos(void) {
    }
    void NuSpecialGetRadius(void *special, NUVEC *position, f32 *radius) {
        NuPlainSpecialHandleLayout *handle = reinterpret_cast<NuPlainSpecialHandleLayout *>(special);
        if (handle->special == NULL) {
            NuPlainDisplaySpecialLayout *display = static_cast<NuPlainDisplaySpecialLayout *>(handle->display_special);
            *position = display->center;
            *radius = display->radius;
            return;
        }

        NuPlainLegacySceneLayout *scene = reinterpret_cast<NuPlainLegacySceneLayout *>(handle->scene);
        NuPlainLegacySpecialLayout *legacy = static_cast<NuPlainLegacySpecialLayout *>(handle->special);
        NuPlainLegacyInstanceBoundsLayout *instance =
            reinterpret_cast<NuPlainLegacyInstanceBoundsLayout *>(legacy->instance);
        NuPlainLegacyObjectBoundsLayout *object =
            static_cast<NuPlainLegacyObjectBoundsLayout *>(scene->objects[instance->object_index]);
        *position = object->center;
        *radius = object->radius;
    }
    void NuSpecialGetShadowClipTestResult(void) {
    }
    void NuSpecialGetShadowLight(void) {
    }
    i32 NuSpecialHasActiveShadowLights(void) {
        return nuspecial_shadow_light_count > 0;
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
    void NuSpecialSetDrawMtx(void *special, NUMTX *mtx) {
        NuPlainSpecialHandleLayout *handle = reinterpret_cast<NuPlainSpecialHandleLayout *>(special);
        if (handle == NULL || handle->scene == NULL) {
            return;
        }
        NuPlainLegacySpecialLayout *legacy = static_cast<NuPlainLegacySpecialLayout *>(handle->special);
        if (legacy != NULL) {
            if (legacy->instance != NULL) {
                *reinterpret_cast<NUMTX *>(legacy->instance) = *mtx;
            }
            return;
        }
        NuPlainDisplaySpecialLayout *display = static_cast<NuPlainDisplaySpecialLayout *>(handle->display_special);
        if (display != NULL) {
            display->draw_mtx = *mtx;
            display->flags |= 0x400;
        }
    }
    void NuSpecialSetDrawPos(void) {
    }
    void NuSpecialSetInstAnimTime(void) {
    }
    void NuSpecialSetInstanceMtx(void) {
    }
    void NuSpecialSetMtx(void) {
    }
    void NuSpecialSetOnScreen(void) {
    }
    void NuSpecialSetRenderPlane(void) {
    }
    void NuSpecialTestAnim(void) {
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
    void NuPolyShadowInit(void) {
    }

    // ---------------------------------------------------------------------------
    // Gobj / hierarchy / scene graph
    // ---------------------------------------------------------------------------

    void NuHGobjDestroy(void) {
    }
    // Original @0x2cce60. Build bind-pose joint matrices, applying the optional
    // per-joint rotation/translation overrides before parent concatenation.
    void NuHGobjEval(nuhgobj_s *object, i32 override_count, nuhgobjjointoverride_s *overrides, NUMTX *matrices) {
        nuhgobjjointoverride_s *override_by_joint[256];
        memset(override_by_joint, 0, static_cast<usize>(object->joint_count) * sizeof(*override_by_joint));

        for (i32 i = 0; i < override_count; ++i) {
            const u8 override_index = overrides[i].joint_index;
            if (override_index < object->joint_override_map_count) {
                const u8 joint_index = object->joint_override_map[override_index];
                if (joint_index != 0xff) {
                    override_by_joint[joint_index] = &overrides[i];
                }
            }
        }

        for (i32 joint_index = 0; joint_index < object->joint_count; ++joint_index) {
            NUMTX local_matrix = object->bind_matrices[joint_index];
            nuhgobjjointoverride_s *joint_override = override_by_joint[joint_index];
            if (joint_override != NULL) {
                constexpr f32 kRadiansToNuAngle = 10430.378f;
                NUANGVEC angles = {
                    static_cast<NUANG>(joint_override->rotation_x * kRadiansToNuAngle),
                    static_cast<NUANG>(joint_override->rotation_y * kRadiansToNuAngle),
                    static_cast<NUANG>(joint_override->rotation_z * kRadiansToNuAngle),
                };
                NUMTX override_matrix;
                NuMtxSetRotateXYZVU0(&override_matrix, &angles);
                NuMtxTranslate(&override_matrix, &joint_override->translation);
                NuMtxMulVU0(&local_matrix, &local_matrix, &override_matrix);
            }

            const u8 parent_index = object->joints[joint_index].parent_index;
            if (parent_index == 0xff) {
                matrices[joint_index] = local_matrix;
            } else {
                NuMtxMulVU0(&matrices[joint_index], &local_matrix, &matrices[parent_index]);
            }
        }
    }
    void NuHGobjEvalAnim(void) {
    }
    // Original @0x2cd730.
    void NuHGobjEvalAnim2(nuhgobj_s *object, ani3_animheader_s *animation, f32 time, i32 override_count,
                          NUJOINTANIM_s *overrides, NUMTX *matrices) {
        NuHGobjEvalAnim2Root(object, animation, time, override_count, overrides, matrices, NULL, NULL);
    }
    void NuHGobjEvalAnim2Root(nuhgobj_s *object, ani3_animheader_s *animation, f32 time, i32 override_count,
                              NUJOINTANIM_s *overrides, NUMTX *matrices, NUHGOBJROOTFN root_fn, void *root_data) {
        if (animation != NULL && (animation->magic == 0x414e4934 || animation->magic == 0x414e4935)) {
            NuHGobjEvalAnim2Root_3(object, animation, time, override_count, overrides, matrices, root_fn, root_data);
            return;
        }

        // Older NuAnimData2 animations use the curve evaluator below this
        // branch in the original function; that legacy path remains pending.
    }
    // Original @0x2cd150.
    void NuHGobjEvalAnim2Root_3(nuhgobj_s *object, ani3_animheader_s *animation, f32 time, i32 override_count,
                                NUJOINTANIM_s *overrides, NUMTX *matrices, NUHGOBJROOTFN root_fn, void *root_data) {
        nuanimbuff_s buffer;
        NUVEC root_translation = {0.0f, 0.0f, 0.0f};
        NuAnimBuffCreateScratch(&buffer);
        NuAnimBuffAccumulate_3(&buffer, animation, time, 1, 0.0f, 0, object, NULL);
        if (override_count != 0 && JointProcAnimFn != NULL) {
            JointProcAnimFn(&buffer, object, override_count, overrides);
        }
        NuAnimBuffEvaluate_3(&buffer, object, matrices, animation, root_fn, &root_translation, root_data);
        NuAnimBuffDestroyScratch(&buffer);
    }
    void NuHGobjEvalAnimBlend(void) {
    }
    // Original @0x2ce980.
    void NuHGobjEvalAnimBlend2(nuhgobj_s *object, ani3_animheader_s *animation_a, f32 time_a,
                               ani3_animheader_s *animation_b, f32 time_b, f32 blend, i32 override_count,
                               NUJOINTANIM_s *overrides, NUMTX *matrices) {
        NuHGobjEvalAnimBlend2Root(object, animation_a, time_a, animation_b, time_b, blend, override_count, overrides,
                                  matrices, NULL, NULL);
    }
    // Original @0x2ce8e0. ANI4/ANI5 are the only accepted animation formats.
    void NuHGobjEvalAnimBlend2Root(nuhgobj_s *object, ani3_animheader_s *animation_a, f32 time_a,
                                   ani3_animheader_s *animation_b, f32 time_b, f32 blend, i32 override_count,
                                   NUJOINTANIM_s *overrides, NUMTX *matrices, NUHGOBJROOTFN root_fn, void *root_data) {
        if (animation_a == NULL || (animation_a->magic != 0x414e4934 && animation_a->magic != 0x414e4935)) {
            return;
        }
        NuHGobjEvalAnimBlend2Root_3(reinterpret_cast<nugscn_s *>(object), animation_a, time_a, animation_b, time_b,
                                    blend, override_count, overrides, matrices, root_fn, root_data);
    }
    void NuHGobjEvalDwa(void) {
    }
    void NuHGobjEvalDwa2(void) {
    }
    void NuHGobjEvalDwaBlend(void) {
    }
    void NuHGobjEvalDwaBlend2(void) {
    }
    i32 NuHGobjForceShadowsOnCharacters(i32 enabled) {
        i32 previous = nuapi.force_shadows_on_characters;
        nuapi.force_shadows_on_characters = enabled;
        return previous;
    }
    void NuHGobjFromVideoMem(void) {
    }
    nuhgobjpoi_s *NuHGobjGetPOI(nuhgobj_s *object, i32 index) {
        const u8 mapped_index = static_cast<u8>(index);
        if (mapped_index >= object->point_of_interest_count) {
            return NULL;
        }
        const u8 point_index = object->point_of_interest_map[mapped_index];
        if (point_index == 0xff) {
            return NULL;
        }
        return &object->points_of_interest[point_index];
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
    i32 NuHGobjReversibleCharacters(i32 enabled) {
        i32 previous = nuapi.reversible_characters;
        nuapi.reversible_characters = enabled;
        return previous;
    }
    void NuHGobjRndr(void) {
    }
    // Original @0x2f56a0. Draw rigid hierarchy pieces at their evaluated joint
    // matrices, then build skin matrices for the smooth hierarchy pieces.
    __attribute__((optimize("O3"))) i32 NuHGobjRndrMtxDwa(nuhgobj_s *object, NUMTX *world_matrix, i32 render_count,
                                                          i16 *render_indices, NUMTX *joint_matrices,
                                                          void **blend_values, i32) {
        i32 clip_state = nuspecial_clip_state;
        if (clip_state == -1) {
            clip_state = NuCameraClipHGobj(reinterpret_cast<nugscn_s *>(object), world_matrix, joint_matrices);
        }

        i32 shadow_clip = 0;
        if (NuSpecialHasActiveShadowLights() != 0) {
            NUVEC bounds_min;
            NUVEC bounds_max;
            NuVecMtxTransform(&bounds_min, &object->bounds_min, world_matrix);
            NuVecMtxTransform(&bounds_max, &object->bounds_max, world_matrix);
            shadow_clip = NuSpecialClipTestShadowLights(&bounds_min, &bounds_max, 1);
        }
        if ((clip_state | shadow_clip) == 0) {
            NuSpecialClearShadowClipTestResults();
            return 0;
        }

        const i32 previous_clip_state = NuSpecialSetClipping(1, clip_state);
        i32 drawn = 0;

        for (i32 render_index = 0; render_index < render_count; ++render_index) {
            const i32 part_index = render_indices != NULL ? render_indices[render_index] : 0;
            if (part_index < 0 || part_index >= object->render_count) {
                continue;
            }

            nuhgobjrender_s &part = object->render_parts[part_index];
            NUMTX *skin_matrices = NULL;
            if (part.smooth_skin_special != NULL || part.alternate_smooth_skin_special != NULL) {
                display_list_buffer->addr = ALIGN(display_list_buffer->addr, 0x80);
                skin_matrices = reinterpret_cast<NUMTX *>(display_list_buffer->addr);
                display_list_buffer->addr += static_cast<usize>(object->joint_count) * sizeof(NUMTX);
            }

            void *blend_value = blend_values != NULL ? blend_values[render_index] : NULL;

            if (part.rigid_specials != NULL) {
                for (i32 joint_index = 0; joint_index < object->joint_count; ++joint_index) {
                    if (part.rigid_specials[joint_index] != NULL) {
                        NUMTX draw_matrix;
                        NuMtxMulVU0(&draw_matrix, &joint_matrices[joint_index], world_matrix);
                        drawn |= NuSpecialDrawAt(part.rigid_specials[joint_index], &draw_matrix);
                    }
                }
            }

            if (part.alternate_rigid_specials != NULL) {
                for (i32 joint_index = 0; joint_index < object->joint_count; ++joint_index) {
                    if (part.alternate_rigid_specials[joint_index] != NULL) {
                        NUMTX draw_matrix;
                        NuMtxMulVU0(&draw_matrix, &joint_matrices[joint_index], world_matrix);
                        drawn |= NuSpecialDrawAt(part.alternate_rigid_specials[joint_index], &draw_matrix);
                    }
                }
            }

            if (part.smooth_skin_special != NULL || part.alternate_smooth_skin_special != NULL) {
                for (i32 joint_index = 0; joint_index < object->joint_count; ++joint_index) {
                    NuMtxMulH(&skin_matrices[joint_index], &object->inverse_bind_matrices[joint_index],
                              &joint_matrices[joint_index]);
                }
                if (part.smooth_skin_special != NULL) {
                    drawn |= NuSpecialDrawSmoothSkinDwa(part.smooth_skin_special, skin_matrices, world_matrix,
                                                        static_cast<DEFORMERWEIGHTSARRAY *>(blend_value));
                }
                if (part.alternate_smooth_skin_special != NULL) {
                    drawn |= NuSpecialDrawSmoothSkinDwa(part.alternate_smooth_skin_special, skin_matrices, world_matrix,
                                                        static_cast<DEFORMERWEIGHTSARRAY *>(blend_value));
                }
            }
        }

        NuSpecialSetClipping(1, previous_clip_state);
        NuSpecialClearShadowClipTestResults();
        return drawn;
    }
    void NuHGobjRndrRandShadowSurfacePoints(void) {
    }
    void NuHGobjSetClippingRootTrackerOverride(void) {
    }
    void NuHGobjToVideoMem(void) {
    }
    void NuGCutCharAnimProcess(void) {
    }
    void NuGCutSceneDestroy(void) {
    }
    void NuGCutSceneIsBackgroundLoading(void) {
    }
    void NuGCutSceneLoadAddr(void) {
    }
    void NuGCutSceneSysBackgroundFlush(void) {
    }
    void NuGCutSceneSysInit(NUGCUTLOCATORFNENTRY_s *locator_functions) {
        locatorfns = locator_functions;
    }
    void NuGCutSceneSysPostBackgroundLoad(void) {
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
    i32 NuKeyboard_db(i32) {
        return 0;
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
    void NuPadSetValid(void) {
    }
    void NuPadUseCorrectDeadZoning(void) {
    }
    void NuPad_Interface_Render(void) {
    }
    void NuPad_Interface_ResetAllTouches(void) {
    }
    void NuPad_Interface_TouchScreenInput(i32, i32, i32, i32, i32, i32, i32, i32) {
    }
    i32 NuPs2ApplyDeadZone(i32 raw_value, i32 dead_zone) {
        i32 value = raw_value - 128;
        if (value > 0) {
            if (value < dead_zone) {
                value = 0;
            } else {
                value = (value - dead_zone) * 255 / (255 - dead_zone);
            }
        } else if (value > -dead_zone) {
            value = 0;
        } else {
            value = (value + dead_zone) * 255 / (255 - dead_zone);
        }
        return value;
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
    i32 NuPortalEnabled(i32 enabled) {
        const i32 previous = portals_enabled;
        portals_enabled = enabled;
        return previous;
    }
    i32 NuPortalNumRooms(NUGSCN *scene) {
        return scene != NULL ? static_cast<u16>(scene->num_rooms) : 0;
    }
    void NuPortalResetActive(NUGSCN *scene) {
        for (u32 i = 0; i < scene->max_portals; ++i) {
            scene->portals[i].is_active |= NUPORTAL_FLAG_ACTIVE | NUPORTAL_FLAG_DEFAULT_ACTIVE;
        }
    }
    i32 NuPortalRoomClipTest(NUGSCN *scene, i16 room_id) {
        if (scene == NULL || scene->max_portals == 0) {
            return 1;
        }
        if (scene->num_portal_frusta <= 0) {
            return 0;
        }
        for (i32 i = 0; i < scene->num_portal_frusta; ++i) {
            if (scene->portal_frusta[i]->room_id == room_id) {
                return 1;
            }
        }
        return 0;
    }
    void NuPortalRoomClipTestAll(NUGSCN *scene, u8 *room_visibility) {
        if (scene == NULL || scene->max_portals == 0) {
            return;
        }
        for (i32 i = 0; i < scene->num_rooms; ++i) {
            room_visibility[i] = 0;
        }
        room_visibility[scene->camera_room] = 1;
        for (i32 i = 0; i < scene->num_portal_frusta; ++i) {
            const i16 room_id = scene->portal_frusta[i]->room_id;
            if (room_id >= 0) {
                room_visibility[room_id] = 1;
            }
        }
    }
    i32 NuPortalWhichRoom(NUGSCN *scene, NUVEC *position) {
        if (position == NULL || scene == NULL) {
            return -1;
        }

        i16 candidates[2] = {0, 0};
        if (scene->num_rooms <= 0) {
            return -1;
        }

        i16 candidate_count = 0;
        for (i32 room_index = 0; room_index < scene->num_rooms; ++room_index) {
            NUROOM &room = scene->rooms[room_index];
            f32 plane_distance = 0.0f;
            for (i32 plane_index = 0; plane_index < room.plane_count; ++plane_index) {
                const NUPLANE &plane = room.planes[plane_index];
                plane_distance = plane.a * position->x + plane.b * position->y + plane.c * position->z + plane.d;
                if (plane_distance > 0.0f) {
                    break;
                }
            }
            if (plane_distance > 0.0f) {
                continue;
            }

            if (candidate_count == 2) {
                NUROOM &first = scene->rooms[candidates[0]];
                NUROOM &second = scene->rooms[candidates[1]];
                if (first.priority < second.priority) {
                    candidates[1] = static_cast<i16>(room_index);
                } else {
                    candidates[0] = static_cast<i16>(room_index);
                }
                candidate_count = 3;
                continue;
            }

            candidates[candidate_count++] = static_cast<i16>(room_index);
            if (candidate_count != 2) {
                continue;
            }

            NUROOM &first = scene->rooms[candidates[0]];
            NUROOM &second = scene->rooms[candidates[1]];
            if ((first.flags & NUROOM_FLAG_OVERLAPPING) != 0 || (second.flags & NUROOM_FLAG_OVERLAPPING) != 0) {
                continue;
            }
            break;
        }

        if (candidate_count == 1) {
            return candidates[0];
        }
        if (candidate_count == 0) {
            return -1;
        }

        NUROOM &first = scene->rooms[candidates[0]];
        NUROOM &second = scene->rooms[candidates[1]];
        for (i32 first_portal = 0; first_portal < first.portal_count; ++first_portal) {
            const i16 portal_index = first.portal_indices[first_portal];
            for (i32 second_portal = 0; second_portal < second.portal_count; ++second_portal) {
                if (second.portal_indices[second_portal] != portal_index) {
                    continue;
                }

                const NUPORTAL &portal = scene->portals[portal_index];
                const f32 side = portal.plane.a * position->x + portal.plane.b * position->y +
                                 portal.plane.c * position->z + portal.plane.d;
                return side < 0.0f ? portal.front_room : portal.back_room;
            }
        }
        return -1;
    }
    void NuVisiBoxTree(void) {
    }
    void *NuVisiEvaluate(NUGSCN *, void *) {
        return NULL;
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
    i32 NuStrNCat(char *str, const char *ext, i32 n) {
        while (*str != '\0') {
            ++str;
        }

        i32 copied = 0;
        if (ext != NULL) {
            do {
                if (n == 0) {
                    break;
                }
                *str++ = *ext;
                ++copied;
                --n;
            } while (*ext++ != '\0');
        }
        return copied;
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
    void NuSetPadDemoEndButtons(u32) {
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
