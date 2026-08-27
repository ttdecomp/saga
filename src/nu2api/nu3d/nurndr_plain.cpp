// nurndr_plain.cpp — Host "plain" renderer backend.
//
// This TU is the host replacement for the PS2 rendering TU (nurndr).
// It owns three things the rest of the engine expects to exist:
//
//   1. Scene lifecycle  — NuRndrBeginScene / NuRndrClear / NuRndrEndScene
//      builds the current `nudisplayscene_s` (0x218 bytes in the original BSS)
//      and queues it into a 16-slot ring consumed by the render thread.
//
//   2. Immediate-mode 2D — NuPrim2DBegin / NuPrim2DAddXYZ / NuPrim2DEnd.
//      Vertices are streamed directly into the display-list vertex buffer
//      (`display_list_buffer`).  Type 4 (quad) is expanded to two triangles
//      (6 vertices) word-wise so both full-float and half-float UV layouts
//      work without a branch.
//
//   3. Frame present   — NuRndrSwapScreen / NuRndrSwapScreenEx.  Flushes
//      debris, swaps the display-list and stream buffers, kicks the render
//      thread, then paces the game thread until the application status
//      leaves the "running" state (or the host render fence completes).
//
// All other entry points from the original TU are retained as link stubs
// until their subsystems are transcribed.  Their signatures are not yet
// recovered, so they are left as `void(void)`.

#include <string.h>

#include "decomp.h"
#include "nu2api/nucore/nucore.hpp"
#include "globals.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nuandroid/ios_graphics.h"

// ---------------------------------------------------------------------------
// Scene state — mirrors original BSS layout
// ---------------------------------------------------------------------------

static constexpr i32 kSceneRingCapacity = 16;

extern "C" {
    // Scene currently being built.  Size is 0x218 bytes; original lives in BSS
    // and is referenced as a plain object by all render/present code.
    struct nudisplayscene_s currentScene = {0};

    // Deferred ring: NuRndrEndScene copies the completed scene here; the render
    // thread drains it.  Stride is 0x218, 16 slots (0x2180 bytes total) in the
    // original.
    i32 sceneParametersCount = 0;
    struct nudisplayscene_s sceneParameters[kSceneRingCapacity] = {0};

    // Shared renderer state block (original BSS @0x119b900, 0x1b0 bytes).
    u8 render_state[0x1b0] = {0};
} // extern "C"

// Swap/present pacing flags (original BSS).
i32 g_isBlockedInSwapScreen = 0;
i32 rndr_blend_shape_deformer_wt_cnt = 0;
i32 rndr_blend_shape_deformer_wt_ptrs_cnt = 0;

// ---------------------------------------------------------------------------
// Immediate-mode 2D stream state
// ---------------------------------------------------------------------------

// Globals shared with nuprim.cpp (see nuprim.h for the canonical declarations).
i32 g_NuPrim_VertexCount;

// File-local bookkeeping for the in-flight prim.  These mirror the original
// TU statics at 0x99b60c (vertex-count pointer) and 0x628cc0 (prim type).
static u16 *s_pendingVertexCount = nullptr;
static u16 s_activePrimType = 0;

// Default 2D render-state constant passed to RndrStateSetConstAlphaTint.
// The original passes a pointer into .rodata @0x57bd40; content is all-zero.
static const u32 kDefault2DState[4] = {0};

// Display-list cursor for the 2D stream.  Defined in nudlist.cpp.
extern VARIPTR *display_list_buffer;

// Per-TU copy of NuDisplayListAddItem.  The original nurndr TU carries its
// own copy at 0x29cc6c alongside the nudlist one; behaviour is identical.
static nudisplaylistitem_s *AddDisplayListItem(nudisplaylist_s *list, u8 type, void *next) {
    nudisplaylistitem_s *item = list->items;
    item->type = type;
    item->id = 3; // CALL
    item->next = next;
    list->items = (nudisplaylistitem_s *)((u8 *)list->items + 0x10);
    return (nudisplaylistitem_s *)((u8 *)list->items - 0x10);
}

// ---------------------------------------------------------------------------
// Prim vertex layout
// ---------------------------------------------------------------------------

// Immediate-mode vertex: {x,y,z, colour, u,v}.  Full UVs are f32[2],
// half UVs are f16[2] packed at the same offsets.  Stride is always 0x18.
// Copied word-wise during quad expansion so both layouts are handled
// without branching.
struct PrimVertexRaw {
    f32 x, y, z;
    u32 color;
    u32 uv[2];
};
static_assert(sizeof(PrimVertexRaw) == 0x18, "PrimVertex stride must be 0x18");

// Header emitted at the start of each prim stream chunk.  The vertex count
// lives at +0xa and is patched by NuPrim2DEnd.
struct PrimStreamHeader {
    u32 prim_type;
    u32 pad0;
    u16 pad1;
    u16 vertex_count; // patched on End
    u32 pad2;
};
static_assert(sizeof(PrimStreamHeader) == 0x10, "PrimStreamHeader must be 0x10");

// ---------------------------------------------------------------------------
// Forward declarations for C-visible helpers
// ---------------------------------------------------------------------------

extern "C" {
    void NuVpGetPosition2(i32 *, i32 *);
    void NuVpGetSize2(i32 *, i32 *);
    i32 NuDisplayListAddRenderScene(void);
    i32 NuDynamicLightIsEnabled(i32);
    void NuDynamicLightAddRenderScene(i32, i32, i32);
    void RndrStateSetConstAlphaTint(i32, i32, const void *, i32, i32);
    void DisplayListUpdateRenderState(void *list, void *state);
    void NuDisplayListLinkMtl(nudisplaylist_s *list, NUMTL *mtl);
    void NuDisplayListLinkItems(nudisplaylist_s *list, i32 count);
    nudisplaylist_s *NuDisplayListGet2dList(void);
}

void NuDebrisRendererFlushBuffers(void);

extern "C" {
    void NuRndrSwapStreamBuffers(void);
    void NuRenderThreadPrepareRender(void);
    void NuRenderThreadStartRender(void);
    void NuShaderManagerBindShader(i32 shader);
    void NuDisplayListCheckBuffer(void);
    void NuDisplayListResetBuffer(void);
    void NuRenderThreadLock(void);
    void NuRenderThreadUnlock(void);
}

// ---------------------------------------------------------------------------
// Immediate-mode 2D API
// ---------------------------------------------------------------------------

extern "C" void NuPrim2DAddXYZ(float x, float y, float z) {
    PrimVertexRaw *vtx = (PrimVertexRaw *)((*g_NuPrim_StreamBufferPtr)->addr);
    vtx->x = NuPrim_XBias + NuPrim_XScale * x;
    vtx->y = NuPrim_YBias + NuPrim_YScale * y;
    vtx->z = z;
    (*g_NuPrim_StreamBufferPtr)->addr += sizeof(PrimVertexRaw);
    g_NuPrim_VertexCount++;

    // Quad expansion (prim type 4): every pair of AddXYZ calls becomes a
    // 6-vertex quad (two triangles).  The expansion is done word-wise so
    // both full-float and half-float UV encodings are preserved without
    // needing to know which is active.  See original 0x29d235..0x29d395.
    if (s_activePrimType == 4 && (g_NuPrim_VertexCount & 1) == 0) {
        u32 *words = (u32 *)(usize)((*g_NuPrim_StreamBufferPtr)->addr - 0x30);
        (*g_NuPrim_StreamBufferPtr)->addr += 0x60;
        g_NuPrim_VertexCount += 4;

        memcpy(&words[12], &words[6], 0x18);
        memcpy(&words[18], &words[12], 0x18);
        words[24] = words[0];
        words[25] = words[7];
        words[26] = words[8];
        words[27] = words[9];
        words[28] = words[4];
        words[29] = words[11];
        words[6] = words[12];
        words[7] = words[1];
        words[8] = words[2];
        words[9] = words[3];
        words[10] = words[16];
        words[11] = words[5];
        memcpy(&words[30], &words[0], 0x18);
    }
}

extern "C" void NuPrim2DBegin(u32 prim_type, u32 /*vtx_fmt*/, NUMTL *mtl) {
    if (mtl == nullptr) {
        mtl = numtl_defaultmtl2d;
    }

    g_NuPrim_NeedsOverbrightening = mtl->tex_id != 0;
    g_NuPrim_NeedsHalfUVs = mtl->shader_desc.vtx_desc.has_half_uvs;

    VARIPTR *buf = NuDisplayListGetBuffer();

    nudisplaylist_s *list;
    if (mtl->display_list != nullptr) {
        list = mtl->display_list;
        u8 *base = *(u8 **)list;
        base[0x74] |= 2;
        i32 bit = (base[0x75] >> 7) & 1;
        u8 *slot = *(u8 **)((u8 *)base + 8 + (0x14 + bit) * 4);
        i32 cnt = ((i32 *)list)[1];
        i32 byte_idx = cnt >= 0 ? (cnt + 7) >> 3 : (~cnt + 9) >> 3;
        u8 mask = (u8)(1 << (((i32 *)list)[1] & 7));
        slot[(usize)byte_idx] |= mask;
    } else {
        list = NuDisplayListGet2dList();
        NuDisplayListLinkMtl(list, mtl);
    }

    RndrStateSetConstAlphaTint(0, 0, kDefault2DState, 0, 0);
    DisplayListUpdateRenderState(list, render_state);
    NuDisplayListLinkItems(list, 1);

    g_NuPrim_StreamBufferPtr = &display_list_buffer;

    auto *hdr = (PrimStreamHeader *)display_list_buffer->addr;
    hdr->prim_type = prim_type;
    hdr->vertex_count = 0;
    display_list_buffer->addr += sizeof(PrimStreamHeader);

    s_pendingVertexCount = &hdr->vertex_count;
    s_activePrimType = (u16)prim_type;
    g_NuPrim_VertexCount = 0;

    AddDisplayListItem(list, 0x93, hdr);
}

extern "C" void NuPrim2DEnd(void) {
    *s_pendingVertexCount = (u16)g_NuPrim_VertexCount;
    g_NuPrim_VertexCount = 0;
}

extern "C" void NuPrim3DEnd(void) {
    *s_pendingVertexCount = (u16)g_NuPrim_VertexCount;
    g_NuPrim_VertexCount = 0;
}

// ---------------------------------------------------------------------------
// Scene lifecycle
// ---------------------------------------------------------------------------

extern "C" i32 NuRndrBeginScene(i32 /*begin_flags*/) {
    // Clear the in-flight scene to a known baseline.  Field names below
    // still use the original unknown_* tags because the 0x218-byte layout
    // has not been fully recovered; values match the PS2 initial defaults.
    currentScene.unknown_4c = 0;
    currentScene.render_scene_id = 0xffffffff;
    currentScene.clear_flags = 0;
    currentScene.state_ptr = nullptr;
    currentScene.unknown_24 = nullptr;
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

extern "C" i32 NuRndrBeginSceneEx(i32 begin_flags) {
    return NuRndrBeginScene(begin_flags);
}

extern "C" void NuRndrClear(i32 clear_flags, i32 bg_colour, f32 alpha) {
    auto *scene = &currentScene;

    if (NuIOS_IsLowEndDevice() && g_BackgroundUsedFogColour) {
        bg_colour = g_BackgroundColour;
    }

    scene->clear_alpha = alpha;
    scene->clear_flags |= clear_flags;
    scene->bg_colour = bg_colour;
    NuVpGetPosition2(&scene->vp_x, &scene->vp_y);
    NuVpGetSize2(&scene->vp_w, &scene->vp_h);
}

extern "C" void NuRndrGradClear(i32 a, i32 b, i32 /*c*/, f32 d) {
    NuRndrClear(a, b, d);
}

extern "C" void NuRndrEndScene(void) {
    auto *scn = &currentScene;

    scn->render_scene_id = NuDisplayListAddRenderScene();

    // If a dynamic light was attached to this scene, hand it to the light
    // manager now that the render-scene id is known.
    if (scn->unknown_38 != 0 && scn->render_scene_id != -1) {
        if (NuDynamicLightIsEnabled(scn->unknown_3c)) {
            NuDynamicLightAddRenderScene(scn->unknown_3c, scn->unknown_40, scn->render_scene_id);
        }
        scn->unknown_38 = 0;
        scn->render_scene_id = -1;
    }

    // Queue into the ring and advance the write index with wrap-around.
    // The render thread drains this ring in processRenderScenes; wrapping
    // here prevents a stalled consumer from corrupting the array.
    i32 slot = sceneParametersCount;
    sceneParametersCount = slot + 1;
    memcpy(&sceneParameters[slot], scn, sizeof(nudisplayscene_s));
    if (sceneParametersCount >= kSceneRingCapacity) {
        sceneParametersCount = 0;
    }
}

extern "C" void NuRndrEndSceneEx(void) {
    NuRndrEndScene();
}

// ---------------------------------------------------------------------------
// Frame present / swap
// ---------------------------------------------------------------------------

// Original 0x2967db — swap display-list and stream buffers, kick the render
// thread, then pace the game thread until the app leaves the running state.
extern "C" i32 NuRndrSwapScreen(void) {
    NuRenderThreadLock();
    rndr_blend_shape_deformer_wt_cnt = 0x3f00;
    rndr_blend_shape_deformer_wt_ptrs_cnt = 0x800;
    NuRenderThreadPrepareRender();
    NuShaderManagerBindShader(0);
    NuDebrisRendererFlushBuffers();
    NuDisplayListSwapBuffersEndFrame();
    NuRndrSwapStreamBuffers();
    NuDisplayListSwapBuffersBeginFrame();
    NuDisplayListCheckBuffer();
    NuDisplayListResetBuffer();
    NuRenderThreadUnlock();
    NuRenderThreadStartRender();

    // Spin until the application status is no longer "running" (1).
    // On Android this is released by the activity lifecycle
    // (nativeSetSurface / nativeOnPause flip NUAPPLICATIONSTATUS).
    for (;;) {
        NuApplicationState *state = NuCore::GetApplicationState();
        if (state->GetStatus().status != 1) {
            break;
        }
        g_isBlockedInSwapScreen = 1;
        NuThreadSleep(1);
    }
    g_isBlockedInSwapScreen = 0;

    return 1;
}

// Original 0x296888
extern "C" void NuRndrSwapScreenEx(i32 /*mode*/, void (*callback)(void)) {
    if (callback != nullptr) {
        callback();
    }
    NuRndrSwapScreen();
}

// ---------------------------------------------------------------------------
// Texture-animation pump — empty until the texanim subsystem is transcribed.
// No texture animations run during boot, so these are intentional no-ops.
// Originals 0x2ccb10 / ...
// ---------------------------------------------------------------------------

extern "C" void NuTexAnimProcess(void) {
}
extern "C" void NuTexAnimProcessEx(void) {
}
extern "C" void NuTexAnimProcessList(void) {
}

// ---------------------------------------------------------------------------
// Link stubs — retained for compatibility, not yet implemented
// ---------------------------------------------------------------------------
//
// Every function below exists in the original binary.  Signatures have not
// been recovered, so they are kept as void(void) stubs.  Grouped by
// subsystem so it is obvious what is still missing.

// Scene / GScn
extern "C" void NuGScnFixupPS(void) {
}
extern "C" void NuGScnFixupTIDsPS(void) {
}
extern "C" void NuGScnFromVideoMem(void) {
}
extern "C" void NuGScnGetSpecial(void) {
}
extern "C" void NuGScnNumSpecials(void) {
}
extern "C" void NuGScnReadForMultiRender(void) {
}
extern "C" void NuGScnRemove(void) {
}
extern "C" void NuGScnRestoreTIDsPS(void) {
}
extern "C" void NuGScnRndr(void) {
}
extern "C" void NuGScnToVideoMem(void) {
}

// Material
extern "C" void NuMtlAnimate(void) {
}
extern "C" void NuMtlAnimateSetMask(void) {
}
extern "C" void NuMtlAnimateSetSpeedScale(void) {
}
extern "C" void NuMtlAnimateShaderMtlTextures(void) {
}
extern "C" void NuMtlCopy(void) {
}
static void NuMtlCreate3D(void) {
}
extern "C" void NuMtlCreateBuff(void) {
}
extern "C" void NuMtlCreateBuff3D(void) {
}
extern "C" void NuMtlCreateEx(void) {
}
extern "C" void NuMtlCreateEx3D(void) {
}
extern "C" void NuMtlDestroy(NUMTL *mtl) {
    (void)mtl;
}
extern "C" void NuMtlFindVariantMtl(void) {
}
extern "C" void NuMtlFindVariantMtlFromDesc(void) {
}
extern "C" void NuMtlInitOverride(void) {
}
extern "C" void NuMtlReadEventSetHandler(void) {
}
extern "C" void NuMtlRegisterForOverride(void) {
}
extern "C" void NuMtlSetCurrentRenderPlane(void) {
}
extern "C" void NuMtlSetRenderPlane(void) {
}
static void NuMtlSetRenderStatesPS(void) {
}
extern "C" void NuMtlSpecialSetUV(void) {
}

// Debug / visualisation geometry
extern "C" void NuRndr3dLine(void) {
}
extern "C" void NuRndrAddFootPrint(void) {
}
extern "C" void NuRndrAddShadow(void) {
}
extern "C" void NuRndrAddShadowPrims(void) {
}
extern "C" void NuRndrAnglesZX(void) {
}
extern "C" void NuRndrAxes(void) {
}
extern "C" void NuRndrAxisArrows(void) {
}
extern "C" void NuRndrAxisArrowsMtx(void) {
}
extern "C" void NuRndrAxisBright(void) {
}
extern "C" void NuRndrBoundingBox(void) {
}
extern "C" void NuRndrBurstObjAdd(void) {
}
extern "C" void NuRndrBurstObjAddNoClip(void) {
}
extern "C" void NuRndrBurstObjEnd(void) {
}
extern "C" void NuRndrCircle(void) {
}
extern "C" void NuRndrCreateBlendShapeDeformerWeightsArray(void) {
}
extern "C" void NuRndrDither(void) {
}
extern "C" void NuRndrEndReflectionRender(void) {
}
extern "C" void NuRndrEndShadowReceiveRender(void) {
}
extern "C" void NuRndrFootPrints(void) {
}
extern "C" void NuRndrFx(void) {
}
extern "C" void NuRndrGetCullDebug(void) {
}
extern "C" void NuRndrGlobalFrameCount(void) {
}
extern "C" void NuRndrGlobalFrameCountPause(void) {
}
extern "C" void NuRndrGradRect2di(void) {
}
extern "C" void NuRndrGradRectUV2di(void) {
}
extern "C" void NuRndrGrid(void) {
}
extern "C" void NuRndrHighResScreenGrab(void) {
}
extern "C" void NuRndrLine2d(void) {
}
extern "C" void NuRndrLine2di(void) {
}
extern "C" void NuRndrLine3d(void) {
}
extern "C" void NuRndrLine3dDbg(void) {
}
extern "C" void NuRndrLine3dDbgFlush(void) {
}
extern "C" void NuRndrLineRect2di(void) {
}
extern "C" void NuRndrLineStrip2d(void) {
}
extern "C" void NuRndrLineStrip2di(void) {
}
extern "C" void NuRndrParticleGroup(void) {
}
extern "C" void NuRndrPspDraw(void) {
}
extern "C" void NuRndrRect(void) {
}
extern "C" void NuRndrRect2d(void) {
}
extern "C" void NuRndrRect2di(void) {
}
extern "C" void NuRndrRectUV2di(void) {
}
extern "C" void NuRndrScreenGrabTileBegin(void) {
}
extern "C" void NuRndrScreenGrabTileDeInit(void) {
}
extern "C" void NuRndrScreenGrabTileEnd(void) {
}
extern "C" void NuRndrScreenGrabTileInit(void) {
}
extern "C" void NuRndrSetAmbientLightPS(void) {
}
extern "C" void NuRndrSetAmbientLightSpecular(void) {
}
extern "C" void NuRndrSetBlendData(void) {
}
extern "C" void NuRndrSetCullDebug(void) {
}
extern "C" void NuRndrSetDebBaseRange(void) {
}
extern "C" void NuRndrSetDebBox(void) {
}
extern "C" void NuRndrSetDirectionalLightsPS(void) {
}
extern "C" void NuRndrSetFxMtx(void) {
}
extern "C" void NuRndrSetGlobalMinMipLevel(void) {
}
extern "C" void NuRndrSetGlobalMipMapBias(void) {
}
extern "C" void NuRndrSetParticleRotation(void) {
}
extern "C" void NuRndrSetSpecularLightPS(void) {
}
extern "C" void NuRndrSetWind(void) {
}
extern "C" void NuRndrShadPolys(void) {
}
extern "C" void NuRndrShadowDirCol(void) {
}
extern "C" void NuRndrShadowInit(void) {
}
extern "C" void NuRndrShadowOnOff(void) {
}
extern "C" void NuRndrSolidTri(void) {
}
extern "C" void NuRndrSphere(void) {
}
extern "C" void NuRndrSphereEx(void) {
}
extern "C" void NuRndrSphereMtx(void) {
}
extern "C" void NuRndrSphereTRS(void) {
}
extern "C" void NuRndrStartReflectionRender(void) {
}
extern "C" void NuRndrStartShadowReceiveRender(void) {
}
extern "C" void NuRndrStateGetFogEnabled(void) {
}
extern "C" void NuRndrStateInit(void) {
}
extern "C" void NuRndrStateSetFogEnabled(void) {
}
extern "C" void NuRndrStateSetFogState(void) {
}
extern "C" void NuRndrStateSetSpecularLight(void) {
}
extern "C" void NuRndrStateSetSpecularLightEx(void) {
}
extern "C" void NuRndrStateUpdateCameraState(void) {
}
extern "C" void NuRndrStrip3d(void) {
}
extern "C" void NuRndrTrailEx(void) {
}
extern "C" void NuRndrTri3dClip(void) {
}
extern "C" void NuRndrTriStrip2di(void) {
}
extern "C" void NuRndrTriStrip3dClip(void) {
}
extern "C" void NuRndrWasDrawnUnreflectedGobj(void) {
}
extern "C" void NuRndrWireTri(void) {
}

// Shader / texture / vertex state
extern "C" void NuShaderGetDirtyMask(void) {
}
extern "C" void NuShaderProgramCreateIOS(void) {
}
extern "C" void NuShaderUniformGetByString(void) {
}
extern "C" void NuTexCleartid(void) {
}
extern "C" void NuTexCreateEx(void) {
}
extern "C" void NuTexCreateFramebufferCopy(void) {
}
extern "C" void NuTexDestroy(void) {
}
static void NuTexGenTexture(void) {
}
extern "C" void NuTexReserveNative(void) {
}
extern "C" void NuTexResolveReference(void) {
}
extern "C" void NuTextureBlendEffect(void) {
}
extern "C" void NuVertexStatesCreate(void) {
}
extern "C" void NuVertexStatesSetGroupState(void) {
}
