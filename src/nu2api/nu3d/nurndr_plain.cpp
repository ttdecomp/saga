
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

#ifdef HOST_BUILD
#endif

// Current render scene being built (0x218 bytes), in bss like the original
// remaining.c binding. Referenced as a plain object by all render/present code.
extern "C" struct nudisplayscene_s currentScene = {0};

// Deferred render-scene ring: completed scenes are copied here by NuRndrEndScene
// and consumed by the render thread. Scene stride is 0x218; 16 ring slots in the
// original (0x2180 bytes total).
extern "C" i32 sceneParametersCount = 0;
extern "C" struct nudisplayscene_s sceneParameters[16] = {0};

// Immediate-mode 2D primitive stream state (globals shared with nuprim.cpp).
// g_NuPrim_StreamBufferPtr and the overbrightening/half-UV flags live in
// nuprim.cpp; see nuprim.h.
i32 g_NuPrim_VertexCount;

// File-static primitive bookkeeping (original: u16* static @0x99b60c and
// current-prim-type u16 @0x628cc0 of this TU).
static u16 *g_NuPrim_VertexCountPtr;
static u16 g_NuPrim_CurrentPrimType;

// Default 2D render-state constant passed to RndrStateSetConstAlphaTint by
// NuPrim2DBegin (original passes a pointer into .rodata @0x57bd40).
static const u32 nudrndr_2d_default_state[4] = {0};

// Per-TU static copy of NuDisplayListAddItem (the original nurndr TU carries
// its own copy at 0x29cc6c alongside the nudlist one).
static nudisplaylistitem_s *nurndr_NuDisplayListAddItem(nudisplaylist_s *list, u8 type, void *next) {
    nudisplaylistitem_s *item = list->items;

    item->type = type;
    item->id = 3; // CALL
    item->next = next;

    list->items = (nudisplaylistitem_s *)((u8 *)list->items + 0x10);

    return (nudisplaylistitem_s *)((u8 *)list->items - 0x10);
}

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
    // Originals 0x2ccb10/... — texture-animation pump; empty until the texanim
    // subsystem is transcribed (no texture animations run during boot).
    // Shared renderer state block

    // defined below, inside this extern "C" block.
    extern u8 render_state[0x1b0];
    extern VARIPTR *display_list_buffer;

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
    void NuMtlDestroy(NUMTL *mtl) {
        (void)mtl;
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
    void NuPrim2DAddXYZ(float x, float y, float z) {
        u8 *vtx = (u8 *)((*g_NuPrim_StreamBufferPtr)->addr);

        *(f32 *)(vtx + 0) = NuPrim_XBias + NuPrim_XScale * x;
        *(f32 *)(vtx + 4) = NuPrim_YBias + NuPrim_YScale * y;
        *(f32 *)(vtx + 8) = z;
        (*g_NuPrim_StreamBufferPtr)->addr += 0x18; // {x,y,z,colour,u,v} stride
        g_NuPrim_VertexCount++;

        if (g_NuPrim_CurrentPrimType == 4 && (g_NuPrim_VertexCount & 1) == 0) {
            // Pair-per-quad mode: expand the four vertices ending at the
            // cursor into a six-vertex quad (two triangles). S = block start.
            u32 *s = (u32 *)(usize)((*g_NuPrim_StreamBufferPtr)->addr - 0x30);
            (*g_NuPrim_StreamBufferPtr)->addr += 0x60;
            g_NuPrim_VertexCount += 4;

            // Exact transcription of the original expansion
            // (NuPrim2DAddXYZ @ 0x29d235..0x29d395). One AddXYZ pair becomes a
            // full quad as triangles (V0,V1',V2)+(V3,V4,V5):
            //   vert2(s[12..17]) := vert1 (s[6..11])
            memcpy(&s[12], &s[6], 0x18);
            //   vert3(s[18..23]) := vert2 (now a copy of vert1)
            memcpy(&s[18], &s[12], 0x18);
            //   vert4 = {V0.x, V1.y, V1.z, V1.c, V0.u, V1.v} (bottom-left)
            s[24] = s[0];
            s[25] = s[7];
            s[26] = s[8];
            s[27] = s[9];
            s[28] = s[4];
            s[29] = s[11];
            //   vert1 = {V2.x, V0.y, V0.z, V0.c, V2.u, V0.v} (top-right;
            //   0x29d301..0x29d368 overwrites vert1 AFTER vert4 was built)
            s[6] = s[12];
            s[7] = s[1];
            s[8] = s[2];
            s[9] = s[3];
            s[10] = s[16];
            s[11] = s[5];
            //   vert5(s[30..35]) := vert0
            memcpy(&s[30], &s[0], 0x18);
        }
    }
    void NuPrim2DBegin(u32 prim_type, u32 vtx_fmt, NUMTL *mtl) {
        if (mtl == NULL) {
            mtl = numtl_defaultmtl2d;
        }

        g_NuPrim_NeedsOverbrightening = mtl->tex_id != 0;
        g_NuPrim_NeedsHalfUVs = mtl->shader_desc.vtx_desc.has_half_uvs;

        VARIPTR *buf = NuDisplayListGetBuffer();

        nudisplaylist_s *list;
#ifdef HOST_BUILD
        // HOST: force 2D through the shared dlist_2d path so the faithful
        // display-list execute (DrawRenderScene) carries the legal quad.
        // The original shader-variant path (mtl->display_list) would require
        // the sort-pri machinery (nused_sort_pris) which the host-boot 2D
        // path never populates, causing AddRenderScene to return -1 and the
        // legal to never reach the render thread. This is PS-equivalent.
        list = NuDisplayListGet2dList();
        NuDisplayListLinkMtl(list, mtl);
#else
        if (mtl->display_list != NULL) {
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
#endif

        RndrStateSetConstAlphaTint(0, 0, nudrndr_2d_default_state, 0, 0);
        DisplayListUpdateRenderState(list, render_state);
        NuDisplayListLinkItems(list, 1);

        // The static slot holds the address of the active cursor variable
        // (original: g_NuPrim_StreamBufferPtr = &display_list_buffer).
        g_NuPrim_StreamBufferPtr = &display_list_buffer;

        u8 *cursor = (u8 *)display_list_buffer->addr;

        *(u32 *)(cursor + 0) = prim_type;
        display_list_buffer->addr += 0x10; // stream item header

        g_NuPrim_VertexCountPtr = (u16 *)(cursor + 0xa);
        g_NuPrim_CurrentPrimType = (u16)prim_type;
        g_NuPrim_VertexCount = 0;

        nurndr_NuDisplayListAddItem(list, 0x93, cursor);
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
        // The 16-slot sceneParameters ring is drained by the render thread's
        // processRenderScenes; clamp it here exactly like the original's modular
        // index so a stalled consumer cannot corrupt the array.
        if (sceneParametersCount >= 16) {
            sceneParametersCount = 0;
        }
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
    extern i32 g_isBlockedInSwapScreen;
    extern i32 rndr_blend_shape_deformer_wt_cnt;
    extern i32 rndr_blend_shape_deformer_wt_ptrs_cnt;
    void NuDebrisRendererFlushBuffers(void);
    void NuRndrSwapStreamBuffers(void);
    void NuRenderThreadPrepareRender(void);
    void NuRenderThreadStartRender(void);
    void NuShaderManagerBindShader(i32 shader);

    extern "C" void NuDisplayListCheckBuffer(void);
    extern "C" void NuDisplayListResetBuffer(void);
    extern "C" void NuRenderThreadLock(void);
    extern "C" void NuRenderThreadUnlock(void);
    // original 0x2967db
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

        for (;;) {
            NuApplicationState *state = NuCore::GetApplicationState();
            if (state->GetStatus().status != 1) {
                break;
            }
            g_isBlockedInSwapScreen = 1;
            NuThreadSleep(1);
        }
        g_isBlockedInSwapScreen = 0;

#ifdef HOST_BUILD
        // HOST-ONLY: on Android the spin above is released by the activity
        // lifecycle (nativeSetSurface/nativeOnPause flip NUAPPLICATIONSTATUS).
        // The host window backend has no activity lifecycle, so wait for the
        // render thread through the engine's own frame-pacing primitive
        // (original 0xe34b0) to keep the game thread from lapping the safe scene
        // buffer.
        NuIOS_WaitForRenderThreadCompletion();
#endif
        return 1;
    }

    // original 0x296888
    extern "C" void NuRndrSwapScreenEx(i32 mode, void (*callback)(void)) {
        (void)mode;
        if (callback != NULL) {
            callback();
        }
        NuRndrSwapScreen();
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
    void NuShaderProgramCreateIOS(void) {
    }
    void NuShaderUniformGetByString(void) {
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

// Originals 0x2ccb10/... — texture-animation pump; empty until the texanim
// subsystem is transcribed (no texture animations run during boot).
extern "C" void NuTexAnimProcess(void) {
}
extern "C" void NuTexAnimProcessEx(void) {
}
extern "C" void NuTexAnimProcessList(void) {
}

// Shared renderer state block (original bss @0x119b900, 0x1b0 bytes)
// with the swap-screen spin flag and blend-shape counters from original bss.
i32 g_isBlockedInSwapScreen = 0;
i32 rndr_blend_shape_deformer_wt_cnt = 0;
i32 rndr_blend_shape_deformer_wt_ptrs_cnt = 0;
u8 render_state[0x1b0] = {0};