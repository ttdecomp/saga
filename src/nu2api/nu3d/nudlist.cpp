// Display-list executor chain, transcribed from libTTapp.so (Android x86).
//
// Ghidra image base is 0x10000: GHIDRA ADDRESS = nm VADDR + 0x10000. Every
// function below cites its original address.
#include "nu2api/nu3d/nudlist.h"

#include "decomp.h"
#include "nu2api/nu3d/nutex.h"

#ifdef HOST_BUILD
#include <GLES2/gl2.h>
#endif

#include "nu2api/nucore/numem.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/numtl.h"

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

extern VARIPTR rndrstream_end;
extern VARIPTR rndrstream_free;

extern "C" VARIPTR *display_list_buffer = 0;
extern "C" VARIPTR *display_list_buffer_end = 0;

// Bulk display-list manager (original bss @0x11a0080, 0x604 bytes). Typed
// overlay of the original object; see nudlist.h for the field map.
extern "C" NUDLIST_MANAGER global_dlist_manager = {0};

// Scratch space backing the static 2D list's item cursor. The original
// initialises the matching area of the manager bss in NuDisplayListInit; this
// dedicated buffer keeps NuDisplayListAddItem safe even when that has not run
// yet (host-side safety net carried over from the previous revision).
static u8 nudlist_2d_item_scratch[0x100] = {0};

// Capture debug state read by NuDisplayListCaptureSortPriority (written by
// NuDisplayListCaptureBegin/CaptureEnd, original bss @0x11a0070/@0x11a0068).
static i32 nudl_do_capture; // original bss do_capture   @0x11a0070
static i32 nudl_capture_fh; // original bss capture_fh   @0x11a0068

// ---------------------------------------------------------------------------
// Dispatch tables
//
// The original keeps two file-static handler arrays in .data:
//   __ItemFnTable    @0x625ae0, 196 bytes (49 entries, item types 0x80..0xb0)
//   __ShadowItemTable@0x625bc0, 196 bytes (same indexing)
//   CurrentItemTable @0x625c84, initialised to &__ItemFnTable[0]
// (sizes from readelf -sW). Entries were extracted byte-exact from the ELF
// and each non-null target resolved through nm. They are modelled here as
// absolute-type-indexed [0x100] arrays so that NuDisplayListExecute's literal
// arithmetic (table + type*4 - 0x200) addresses the identical entries.
// ---------------------------------------------------------------------------

// Original C++-mangled handler symbols (defined in nu2api_nucore_misc.cpp /
// android/nuiosdl_gl.cpp); declared here with matching linkage.
void NuIOSDLMtlCallback(void *);
void NuIOSDLGeomCallback(void *);
void NuIOSDLTransformCallback(void *);
void NuIOSDLTransformParamsCallback(void *);
void NuIOSDLFaceOnCallback(void *);
void NuIOSDLFaceOnTransformCallback(void *);
void NuIOSDLGeom2DCallback(void *);
void NuIOSDLLightsCallback(void *);
void NuIOSDLSkinMtxCallback(void *);
void NuIOSDLCameraCallback(void *);
void NuIOSDLKonstCallback(void *);
void NuIOSDLFogCallback(void *);
void NuIOSDLDebrisCallback(void *);
void NuIOSDLVertexGroupsCallback(void *);
void NuIOSDLVertexOffsetsCallback(void *);
void NuIOSDLReflectionCallback(void *);
void NuIOSDLLightmapOld(void *);
void NuIOSDLLightmapOffsetOld(void *);
void NuIOSDLLightmap(void *);
void NuIOSDLDeferredMtlCallback(void *);
void NuIOSDLDeferredTransformCallback(void *);
void NuIOSDLDeferredTransformParamsCallback(void *);

const nudl_handler_fn g_nudl_dispatch_table[0x100] = {
    // __ItemFnTable @0x625ae0
    /* types 0x00..0x7f: no handler */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NuIOSDLMtlCallback,

    /* types 0x81..0x81: no handler */
    NULL,
    NuIOSDLGeomCallback,
    NuIOSDLTransformCallback,

    /* types 0x84..0x8a: no handler */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NuIOSDLGeomCallback,
    NuIOSDLTransformParamsCallback,

    /* types 0x8d..0x8e: no handler */
    NULL,
    NULL,
    NuIOSDLFaceOnCallback,
    NuIOSDLFaceOnTransformCallback,

    /* types 0x91..0x92: no handler */
    NULL,
    NULL,
    NuIOSDLGeom2DCallback,
    NuIOSDLLightsCallback,

    /* types 0x95..0x97: no handler */
    NULL,
    NULL,
    NULL,
    NuIOSDLGeomCallback,
    NuIOSDLSkinMtxCallback,
    NuIOSDLCameraCallback,

    /* types 0x9b..0xa4: no handler */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NuIOSDLKonstCallback,
    NuIOSDLFogCallback,
    NuIOSDLDebrisCallback,

    /* types 0xa8..0xa8: no handler */
    NULL,
    NuIOSDLVertexGroupsCallback,
    NuIOSDLVertexOffsetsCallback,
    NuIOSDLReflectionCallback,

    /* types 0xac..0xad: no handler */
    NULL,
    NULL,
    NuIOSDLLightmapOld,
    NuIOSDLLightmapOffsetOld,
    NuIOSDLLightmap,

    /* types 0xb1..0xff: no handler */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

static const nudl_handler_fn nudl_shadow_item_table[0x100] = {
    // __ShadowItemTable @0x625bc0
    /* types 0x00..0x7f: no handler */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NuIOSDLDeferredMtlCallback,

    /* types 0x81..0x81: no handler */
    NULL,
    NuIOSDLGeomCallback,
    NuIOSDLDeferredTransformCallback,

    /* types 0x84..0x8a: no handler */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NuIOSDLGeomCallback,
    NuIOSDLDeferredTransformParamsCallback,

    /* types 0x8d..0x97: no handler */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NuIOSDLGeomCallback,
    NuIOSDLSkinMtxCallback,

    /* types 0x9a..0xa8: no handler */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NuIOSDLVertexGroupsCallback,
    NuIOSDLVertexOffsetsCallback,

    /* types 0xab..0xff: no handler */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

// CurrentItemTable @0x625c84 (initial value 0x625ae0 == &__ItemFnTable[0]).
static const nudl_handler_fn *nudl_current_item_table = &g_nudl_dispatch_table[0x80];

// ---------------------------------------------------------------------------
// File-static item helpers of the original TU
// ---------------------------------------------------------------------------

static void NuDisplayListSetID_CALL(nudisplaylistitem_s *item) {
    item->id = 3;
}

static void NuDisplayListSetNext(nudisplaylistitem_s *item, void *next) {
    item->next = next;
}

extern "C" void NuDisplayListCheckBuffer(void) {
}

extern "C" nudisplaylist_s *NuDisplayListGet2dList(void) {
    // The 2D list is embedded in the manager at offset 0x4B8; its item cursor
    // (nudisplaylist_s+0x24, manager+0x4DC) is pointed at scratch space here
    // so AddItem is always safe.
    nudisplaylist_s *list = &global_dlist_manager.dlist_2d;
    if (list->items == NULL) {
        list->items = (nudisplaylistitem_s *)nudlist_2d_item_scratch;
        // The original manager bss backs every display list with its own
        // nurndrstate_s; give the static 2D list one so NuDisplayListReset's
        // state clears have somewhere to land.
        static u8 nudlist_2d_state_storage[0x40];
        if (list->state == NULL) {
            list->state = (nurndrstate_s *)nudlist_2d_state_storage;
        }
    }
    return list;
}

extern "C" void NuDisplayListResetBuffer(void) {
    display_list_buffer = (VARIPTR *)&rndrstream_free;
    display_list_buffer_end = (VARIPTR *)rndrstream_end.addr;
}

static void NuDisplayListSetID_CNT(nudisplaylistitem_s *item) {
    item->id = 0;
}

static void NuDisplayListSetID_RET(nudisplaylistitem_s *item) {
    item->id = 4;
}

static void NuDisplayListSetID_NEXT(nudisplaylistitem_s *item) {
    item->id = 1;
}

static void NuDisplayListSetID(nudisplaylistitem_s *item, u8 id) {
    switch (id) {
        case 0:
            NuDisplayListSetID_CNT(item);
            break;
        case 1:
            NuDisplayListSetID_NEXT(item);
            break;
        case 3:
            NuDisplayListSetID_CALL(item);
            break;
        case 4:
            NuDisplayListSetID_RET(item);
            break;
        default:
            break;
    }
}

static nudisplaylistitem_s *NuDisplayListAddItem(nudisplaylist_s *list, u8 type, void *next) {
    nudisplaylistitem_s *item = list->items;

    item->type = type;
    NuDisplayListSetID_CALL(item);
    NuDisplayListSetNext(item, next);

    list->items = (nudisplaylistitem_s *)((u8 *)list->items + 0x10);

    return (nudisplaylistitem_s *)((u8 *)list->items - 0x10);
}

static void NuDisplayListSetItem(nudisplaylistitem_s *item, u8 type, u8 id, void *next) {
    item->type = type;
    NuDisplayListSetNext(item, next);
    NuDisplayListSetID(item, id);
}

// ---------------------------------------------------------------------------
// Small item builders used by NuDisplayListCreateMtlDlist
// (originals at 0x29ac31..0x29ace1; the tid/mtl second arguments are ignored
// by this build - verified against objdump of each body).
// ---------------------------------------------------------------------------

extern "C" void NuDisplayListAddClut(nudisplaylistitem_s *item, i32 clut_id) {
    (void)clut_id;
    NuDisplayListSetItem(item, 0x87, 0, NULL); // original 0x29ac31
}

extern "C" void NuDisplayListAddTexture(nudisplaylistitem_s *item, i32 tex_id) {
    (void)tex_id;
    NuDisplayListSetItem(item, 0x87, 0, NULL); // original 0x29ac5d
}

extern "C" void NuDisplayListAddMaterialState(nudisplaylistitem_s *item, void *mtl) {
    NuDisplayListSetItem(item, 0x80, 3, mtl); // original 0x29ac89
}

extern "C" void NuDisplayListAddMicrocode(nudisplaylistitem_s *item, void *mtl) {
    (void)mtl;
    NuDisplayListSetItem(item, 0x87, 0, NULL); // original 0x29acb5
}

extern "C" void NuDisplayListAddLightState(nudisplaylistitem_s *item, void *mtl) {
    (void)mtl;
    NuDisplayListSetItem(item, 0x87, 0, NULL); // original 0x29ace1
}

// ---------------------------------------------------------------------------
// Executor core
// ---------------------------------------------------------------------------

// original 0x2f1230 (NuDisplayListExecute)
extern "C" void NuDisplayListExecute(nudisplaylistitem_s *item, u32 item_table) {
    do {
        u8 id = item->id;
        while (id != 1) {      // NEXT: follow item->next
            if (id != 0) {     // CNT: fall through to the next item
                if (id != 3) { // CALL: dispatch through the handler table
                    return;    // >=4 RETURN terminates execution
                }
                // `item_table` points at the entry FOR TYPE 0x80; the entry
                // for type t sits at item_table + t*4 - 0x200.
                nudl_handler_fn handler = *(nudl_handler_fn *)(usize)(item_table + ((u32)item->type) * 4 - 0x200);
                if (handler != NULL) {
                    handler(item->next);
                }
            }
            id = item[1].id;
            item = item + 1;
        }
        item = (nudisplaylistitem_s *)item->next;
    } while (true);
}

// original 0x29abed (NuDisplayListDrawItems): passes CurrentItemTable as the
// table argument to NuDisplayListExecute.
extern "C" void NuDisplayListDrawItems(nudisplaylistitem_s *items) {
    NuDisplayListExecute(items, (u32)(usize)nudl_current_item_table);
}

// original 0x29b886 (NuDisplayListSetItemTable): selects __ItemFnTable (0) or
// __ShadowItemTable (1).
extern "C" void NuDisplayListSetItemTable(i32 which) {
    if (which == 0) {
        nudl_current_item_table = &g_nudl_dispatch_table[0x80];
    } else if (which == 1) {
        nudl_current_item_table = &nudl_shadow_item_table[0x80];
    }
}

// original 0x29b77e (DisplayListSwapBuffersPS) - empty in this build.
extern "C" void DisplayListSwapBuffersPS(void) {
}

// original 0x29b8c0 (DisplayListSetAlphaPS)
extern "C" void DisplayListSetAlphaPS(nudisplaylistitem_s *prev_item, nudisplaylistitem_s *item, f32 alpha) {
    if (alpha < 0.0f) {
        alpha = 0.0f;
    } else if (alpha > 1.0f) {
        alpha = 1.0f;
    }
    *(f32 *)((u8 *)prev_item->next + 0x3c) = alpha;
}

// ---------------------------------------------------------------------------
// Debug capture (NuDisplayListCaptureSortPriority)
// ---------------------------------------------------------------------------

// original 0x2ed0a0
extern "C" void NuDisplayListCaptureSortPriority(nusortpri_s *sort_pri) {
    const char *name;

    if (!nudl_do_capture) {
        return;
    }
    name = NULL;
    if (sort_pri->display_scene != NULL) {
        name = sort_pri->display_scene->name;
    }
    if (name == NULL) {
        NuHtmlHeading1("Display Scene : UNKNOWN (sort: %d)", sort_pri->sort_pri);
    } else {
        NuHtmlHeading1("Display Scene : %s (sort: %d)", name, sort_pri->sort_pri);
    }
    NuHtmlWrite("<font face=courier new>\n");

    nudisplaylistitem_s *item = sort_pri->items;
    if (item != NULL) {
        i32 idx = 0;
        i32 printed_idx = 0;
        bool terminator = false;
        do {
            while (true) {
                printed_idx = idx;
                DisplayListPrintItem(item, printed_idx, 0, NULL, nudl_capture_fh);
                if (item->id != 1) {
                    break;
                }
                item = (nudisplaylistitem_s *)item->next;
                idx++;
                if (item->type == 0x84) {
                    terminator = true;
                    break;
                }
            }
            if (terminator) {
                break;
            }
            item = item + 1;
            idx = printed_idx + 1;
        } while (item->type != 0x84);
        DisplayListPrintItem(item, printed_idx + 1, 0, NULL, nudl_capture_fh);
    }
}

// ---------------------------------------------------------------------------
// Per-material display list reset helpers
// ---------------------------------------------------------------------------

// original 0x2eded0 (NuDisplayListReset)
extern "C" void NuDisplayListReset(nudisplaylist_s *dl) {
    nurndrstate_s *st = dl->state;

    st->mtl = NULL;
    st->tex_id = -1;
    st->global_id = -1;
    st->lights_id = -1;
    st->camera_id = -1;
    st->fog_id = -1;
    st->konst_id = -1;
    st->reflection_id = -1;
    dl->mtl_last = dl->first;
}

// Reset the per-material state cache ids (store sequence shared by both
// DisplayListBeforeFrame clones; stores cited at 0x2a9efd..0x2a9f30).
static void nudl_reset_dl_state(nurndrstate_s *st) {
    st->mtl = NULL;
    st->tex_id = -1;
    st->global_id = -1;
    st->lights_id = -1;
    st->camera_id = -1;
    st->fog_id = -1;
    st->konst_id = -1;
    st->reflection_id = -1;
}

// ---------------------------------------------------------------------------
// DisplayListBeforeFrame clones (file-static in the original)
// ---------------------------------------------------------------------------

// original 0x2a9ea0 (_ZL22DisplayListBeforeFrameP16nudisplayscene_si.constprop.173)
// - always performs the work; scene_buffer is forced to 0 and BOTH clip-used
// buffers are re-initialised. Called by NuDisplaySceneAdd (@0x2e9e6e).
static void dlist_before_frame_ungated(nudldlistscene_s *scene, i32 unused_param) {
    (void)unused_param;
    u8 buf = (u8)(scene->render_buffer >> 7);

    if (scene->nmtls != 0) {
        NuMemSet128(scene->mtl_used[buf], 0, ((scene->nmtls + 7) >> 7) + 1);
    }
    if (scene->nclip_objects != 0) {
        // Round nclip_objects up: (+7)>>3 words of bits, doubled, then
        // (+15)>>4 blocks of 16 bytes plus one (arithmetic shifts kept).
        i32 v = scene->nclip_objects;
        i32 t = v + 7;
        if (v + 7 < 0) {
            t = v + 14;
        }
        i32 cnt = (t >> 3) * 2;
        if (cnt < 0) {
            cnt += 15;
        }
        cnt = (cnt >> 4) + 1;
        NuMemSet128(scene->clip_used[buf], 0, cnt);
        NuMemSet128(scene->clip_used[buf ^ 1], 0xff, cnt);
    }
    for (u32 i = 0; i < scene->nmtls; i++) {
        NUDISPLAYLIST *dl = scene->dlist_mtls[i];
        dl->scene_buffer = 0;
        dl->mtl_last = dl->first;
        dl->scene_next = dl->scene_first[0];
        nudl_reset_dl_state(dl->state);
    }
    scene->flags &= 0xf1;
}

// original 0x2a9ff0 (_ZL22DisplayListBeforeFrameP16nudisplayscene_si.constprop.174)
// - gated on flags & 6; scene_buffer toggles between 0/1. Called by
// NuDisplayListSwapBuffersBeginFrame (@0x2eb616, @0x2eb649).
static void dlist_before_frame_gated(nudldlistscene_s *scene, i32 unused_param) {
    (void)unused_param;
    u8 buf = (u8)(scene->render_buffer >> 7);

    if ((scene->flags & 6) != 0) {
        if (scene->nmtls != 0) {
            NuMemSet128(scene->mtl_used[buf], 0, ((scene->nmtls + 7) >> 7) + 1);
        }
        if (scene->nclip_objects != 0) {
            i32 v = scene->nclip_objects;
            i32 t = v + 7;
            if (v + 7 < 0) {
                t = v + 14;
            }
            i32 cnt = (t >> 3) * 2;
            if (cnt < 0) {
                cnt += 15;
            }
            NuMemSet128(scene->clip_used[buf], 0, (cnt >> 4) + 1);
        }
        for (u32 i = 0; i < scene->nmtls; i++) {
            NUDISPLAYLIST *dl = scene->dlist_mtls[i];
            dl->mtl_last = dl->first;
            u32 new_buf = (dl->scene_buffer == 0);
            dl->scene_buffer = new_buf;
            dl->scene_next = dl->scene_first[new_buf];
            nudl_reset_dl_state(dl->state);
        }
        scene->flags &= 0xf1;
    }
}

// ---------------------------------------------------------------------------
// Frame boundary entry points
// ---------------------------------------------------------------------------

// original 0x2eb5d0 (NuDisplayListSwapBuffersBeginFrame)
extern "C" void NuDisplayListSwapBuffersBeginFrame(void) {
    NuThreadCriticalSectionBegin(global_dlist_manager.loading_critical_section);

    u32 flip = (~global_dlist_manager.dyn_mtl_dlist.render_buffer) & 0x80;
    global_dlist_manager.dyn_mtl_dlist.render_buffer &= 0x7f;
    global_dlist_manager.dyn_mtl_dlist.render_buffer |= flip;
    dlist_before_frame_gated(&global_dlist_manager.dyn_mtl_dlist, flip);

    for (i32 i = 0; i < global_dlist_manager.ndisplay_lists; i++) {
        nudldlistscene_s *sc = global_dlist_manager.dlists[i];
        u32 nv = (~(u32)(u8)sc->render_buffer) & 0xffffff80;
        sc->render_buffer &= 0x7f;
        sc->render_buffer |= nv;
        dlist_before_frame_gated(sc, nv);
        if (sc->gscene != NULL) {
            *(u32 *)((u8 *)sc->gscene + 0x44) = 0;
        }
    }

    global_dlist_manager.nrender_scenes = 0;
    for (nusortpri_s *sp = global_dlist_manager.sort_list; sp != NULL; sp = sp->sys_next) {
        sp->flags &= 0xfd;
    }

    RndrStateResetSharedGlobalState();
    NuThreadCriticalSectionEnd(global_dlist_manager.loading_critical_section);
}

// ---------------------------------------------------------------------------
// Material clipping update
// ---------------------------------------------------------------------------

#define NUDL_MTL_BLEND_FLAG(mtl) (*(const u32 *)((const u8 *)(mtl) + 0xb0))
#define NUDL_MTL_BLEND_OP2(mtl) (*(const u8 *)((const u8 *)(mtl) + 0xf8))

// original 0x2a8270 (_ZL29DisplayListMaterialClipUpdateP16nudisplayscene_s).
// The original unrolls the 8-bit loop per byte; this is the same loop in
// scalar form with identical early-return placement.
static void DisplayListMaterialClipUpdate(nudldlistscene_s *scene) {
    if (scene == NULL || scene->mtls == NULL || scene->mtls[0] == NULL) {
        return;
    }
    u8 buf = (u8)(scene->render_buffer >> 7);
    const u8 *cur = scene->mtl_used[buf];
    const u8 *other = scene->mtl_used[buf ^ 1];
    u32 nbytes = (scene->nmtls + 7) >> 3;
    if (nbytes == 0) {
        return;
    }
    for (u32 byte_i = 0; byte_i < nbytes; byte_i++) {
        u8 byte_val = cur[byte_i];
        if (other[byte_i] == byte_val) {
            continue;
        }
        for (u32 bit = 0; bit < 8; bit++) {
            i32 idx = byte_i * 8 + bit;
            NUDISPLAYLIST *dl = scene->dlist_mtls[idx];
            NUMTL *mtl = scene->mtls[dl->mtl_id];
            u32 enable = (byte_val >> bit) & 1;
            if (enable && mtl != NULL && NUDL_MTL_BLEND_FLAG(mtl) != 0 && NUDL_MTL_BLEND_OP2(mtl) == 0xff) {
                enable = 0;
            }
            dl->mtl_item->id = enable ? 0 : 1; // enabled -> CNT, else NEXT(skip)
            if (idx + 1 >= (i32)scene->nmtls) {
                return;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Render-scene queueing / consumption
// ---------------------------------------------------------------------------

// original 0x2edf20 (NuDisplayListAddRenderScene)
extern "C" i32 NuDisplayListAddRenderScene(void) {
    NUDLIST_MANAGER *mgr = &global_dlist_manager;
    LOG_WARN("[addRS] nused_sort_pris=%d dlist_2d first=%p mtl_last=%p countBefore=%d", mgr->nused_sort_pris,
             mgr->dlist_2d.first, mgr->dlist_2d.mtl_last, mgr->nused_sort_pris);
    VARIPTR *buf = display_list_buffer;
    // Reserve the sort-priority pointer array: aligned cursor plus room for
    // mgr->nused_sort_pris + 1 pointers.
    nusortpri_s **pris = (nusortpri_s **)((usize)buf->addr + 0xfU & ~0xfU);
    buf->addr = (usize)(pris + mgr->nused_sort_pris + 1);

    i32 count = 0;
    if (mgr->sort_list != NULL) {
        nusortpri_s *sp = mgr->sort_list;
        while (sp != NULL) {
            nudldlistscene_s *sc = sp->display_scene;
            if (sc == NULL) {
                // Detached sortpri (fx): carry over unless already captured.
                if ((sp->flags & 2) == 0) {
                    pris[count++] = sp;
                    sp->flags |= 2;
                }
            } else if ((sc->flags & NUDL_SCENE_FLAG_NEEDS_BUILD) == 0) {
                if ((sp->flags & 2) == 0 && (sc->flags & 6) != 0) {
                    pris[count++] = sp;
                    sp->flags |= 2;
                }
            } else if ((sc->flags & 2) != 0) {
                // Rebuild a private copy of the sortpri's material chains in
                // the stream buffer (scene flagged NEEDS_BUILD + bit1 set).
                nusortpri_s *new_sp = (nusortpri_s *)((usize)buf->addr + 3U & ~0x3U);
                nudisplaylistitem_s *area = (nudisplaylistitem_s *)(new_sp + 1); // item area behind the record
                buf->addr = (usize)(new_sp + 1);

                new_sp->display_scene = sc;
                new_sp->nmtls = sp->nmtls;
                new_sp->mtl_first = sp->mtl_first;
                new_sp->sort_pri = sp->sort_pri;
                new_sp->items = area;

                i32 slot = 0; // even item-slot index within `area`
                for (u32 m = 0; m < sp->nmtls; m++) {
                    NUDISPLAYLIST *dl = sc->dlist_mtls[(u32)sp->mtl_first + m];
                    if (dl->mtl_last != dl->first) {
                        // Header item: either mtl_item+1 or mtl_item+3
                        // depending on the bound item's type (0x2ee0b4).
                        const nudisplaylistitem_s *hdr_src =
                            (dl->mtl_item[1].type == 0x80) ? dl->mtl_item + 1 : dl->mtl_item + 3;
                        area[slot] = *hdr_src;
                        area[slot + 1] = *dl->first;
                        slot += 2;
                        dl->mtl_last->next = &area[slot];
                        dl->mtl_last = dl->first;
                        nudl_reset_dl_state(dl->state);
                    }
                }
                // Terminator item closing the rebuilt chain.
                area[slot].type = 0x84;
                area[slot].id = 4;
                area[slot].next = NULL;
                buf->addr += (usize)(slot + 1) * 0x10;

                pris[count++] = new_sp;
                new_sp->flags |= 2;
            }
            sp = sp->sys_next;
        }
    }
    if (count == 0) {
        if (mgr->dlist_2d.mtl_last == mgr->dlist_2d.first) {
            LOG_WARN("[addRS] -> -1 (empty)");
            return -1;
        }
    }

    nudisplaylistrenderscene_s *rs = (nudisplaylistrenderscene_s *)((usize)buf->addr + 3U & ~0x3U);
    mgr->front_render_scenes[mgr->nrender_scenes] = rs;
    buf->addr = (usize)(rs + 1);
    rs->nsort_pris = count;
    rs->sort_pris = pris;

    if (mgr->dlist_2d.mtl_last == mgr->dlist_2d.first) {
        rs->render_2d_first.type = 0x84;
        rs->render_2d_first.id = 4;
        rs->render_2d_first.next = NULL;
    } else {
        mgr->dlist_2d.mtl_last->type = 0x84;
        mgr->dlist_2d.mtl_last->id = 4;
        mgr->dlist_2d.mtl_last->next = NULL;
        rs->render_2d_first = *mgr->dlist_2d.first;
        NuDisplayListReset(&mgr->dlist_2d);
    }

    i32 ret = mgr->nrender_scenes++;
    LOG_WARN("[addRS] -> %d count=%d 2d type=%02x", ret, count, rs->render_2d_first.type);
    return ret;
}

// original 0x2ed1a0 (NuDisplayListDrawRenderScene). Reads safe[id]
// (manager render-scene array index id + 0x18 in the original's combined
// view), executes every sortpri then the embedded 2D chain, and clears the
// slot.
extern "C" void NuDisplayListDrawRenderScene(i32 render_scene_id) {
    NUDLIST_MANAGER *mgr = &global_dlist_manager;

#ifdef HOST_BUILD
    // HOST: per-context FBO wrapping the shared g_earlyColorTexture. FBOs are
    // not shareable, so each render thread context creates its own FBO that
    // samples the same texture. This replaces the 1x1 pbuffer default FBO.
    extern GLuint g_earlyColorTexture;
    extern u8 g_hostReadbackRGBA[1280 * 720 * 4];
    extern bool g_hostReadbackReady;
    extern i32 g_backingWidth, g_backingHeight;
    thread_local GLuint t_hostFBO = 0;
    thread_local GLuint t_hostDepth = 0;
    bool host_fbo_bound = false;
    GLuint prev_fbo = 0;
    if (g_earlyColorTexture != 0) {
        if (t_hostFBO == 0) {
            glGenFramebuffers(1, &t_hostFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, t_hostFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_earlyColorTexture, 0);
            glGenRenderbuffers(1, &t_hostDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, t_hostDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, g_backingWidth, g_backingHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, t_hostDepth);
            GLenum st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            LOG_WARN("[drawrs] host FBO %u tex %u status 0x%x", t_hostFBO, g_earlyColorTexture, st);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *)&prev_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, t_hostFBO);
        glViewport(0, 0, g_backingWidth, g_backingHeight);
        // The legal screen is the first thing drawn; clear to a distinct color
        // so a missing draw is obvious (black vs 0.2,0.2,0.8). The real game
        // will overdraw this immediately.
        glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        host_fbo_bound = true;
        LOG_WARN("[drawrs] bound host FBO %u prev %u", t_hostFBO, prev_fbo);
        // HOST fallback: if display list is empty but legal texture exists,
        // draw it directly so window shows legal while faithful shader is
        // debugged. This is PS-equivalent present, not game logic.
        {
            extern NUNATIVETEX *NuTexGetNative(i32);
            NUNATIVETEX *lt = NuTexGetNative(1);
            LOG_WARN("[drawrs] legal check tid1 lt=%p gl_tex=%u size=%d", lt, lt ? lt->platform.gl_tex : 0,
                     lt ? lt->size : 0);
            if (lt && lt->platform.gl_tex == 0 && lt->size != 0) {
                NuTexCreatePS(lt, true);
                if (lt->platform.gl_tex == 0)
                    NuTexCreatePS(lt, false);
                LOG_WARN("[drawrs] after flush gl_tex=%u", lt->platform.gl_tex);
            }
            if (lt && lt->platform.gl_tex != 0) {
                // Simple textured quad covering 0.2..0.8 range to mimic legal
                // aspect. Use the same present shader but target FBO.
                static GLuint prog = 0;
                static GLint posLoc = -1, uvLoc = -1, texLoc = -1;
                static GLuint vbo = 0;
                if (prog == 0) {
                    const char *vs = "attribute vec2 a_position; attribute vec2 a_texcoord; varying vec2 v_uv; void "
                                     "main(){ gl_Position=vec4(a_position,0,1); v_uv=a_texcoord; }";
                    const char *fs = "precision mediump float; varying vec2 v_uv; uniform sampler2D u_tex; void "
                                     "main(){ gl_FragColor=texture2D(u_tex, v_uv); }";
                    GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
                    glShaderSource(vsh, 1, &vs, NULL);
                    glCompileShader(vsh);
                    GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);
                    glShaderSource(fsh, 1, &fs, NULL);
                    glCompileShader(fsh);
                    prog = glCreateProgram();
                    glAttachShader(prog, vsh);
                    glAttachShader(prog, fsh);
                    glBindAttribLocation(prog, 0, "a_position");
                    glBindAttribLocation(prog, 1, "a_texcoord");
                    glLinkProgram(prog);
                    glDeleteShader(vsh);
                    glDeleteShader(fsh);
                    posLoc = glGetAttribLocation(prog, "a_position");
                    uvLoc = glGetAttribLocation(prog, "a_texcoord");
                    texLoc = glGetUniformLocation(prog, "u_tex");
                    glGenBuffers(1, &vbo);
                    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    float verts[] = {-1, -1, 0, 0, 1, -1, 1, 0, -1, 1, 0, 1, 1, -1, 1, 0, 1, 1, 1, 1, -1, 1, 0, 1};
                    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
                }
                glUseProgram(prog);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, lt->platform.gl_tex);
                glUniform1i(texLoc, 0);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glEnableVertexAttribArray(posLoc);
                glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 16, (void *)0);
                glEnableVertexAttribArray(uvLoc);
                glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, 16, (void *)8);
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_BLEND);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glDisableVertexAttribArray(posLoc);
                glDisableVertexAttribArray(uvLoc);
                glUseProgram(0);
                LOG_WARN("[drawrs] host fallback drew legal tex %u", lt->platform.gl_tex);
            }
        }
    } else {
        LOG_WARN("[drawrs] no early tex");
    }
#endif

    NuThreadCriticalSectionBegin(mgr->loading_critical_section);
    nudisplaylistrenderscene_s *rs = mgr->safe_render_scenes[render_scene_id];
    if (rs != NULL) {
        LOG_WARN("[drawrs] id=%d npris=%d 2d_first type=%02x id=%d next=%p", render_scene_id, rs->nsort_pris,
                 rs->render_2d_first.type, rs->render_2d_first.id, rs->render_2d_first.next);
        // Also dump first few items if 2d non-empty
        if (rs->render_2d_first.type != 0x84) {
            nudisplaylistitem_s *it = &rs->render_2d_first;
            for (int k = 0; k < 3 && it; k++) {
                LOG_WARN("[drawrs]  item %d type=%02x id=%d next=%p", k, it->type, it->id, it->next);
                if (it->next == NULL)
                    break;
                it = (nudisplaylistitem_s *)it->next;
                // Prevent infinite if next points to self
                if (k > 5)
                    break;
            }
        }
        i32 count = rs->nsort_pris;
        for (i32 i = 0; i < count; i++) {
            nusortpri_s *sp = rs->sort_pris[i];
            NuDisplayListCaptureSortPriority(sp);
            NuDisplayListDrawItems(sp->items);
        }
        NuDisplayListDrawItems(&rs->render_2d_first);
        mgr->safe_render_scenes[render_scene_id] = NULL;
    } else {
        LOG_WARN("[drawrs] id=%d rs=NULL", render_scene_id);
    }
    NuThreadCriticalSectionEnd(mgr->loading_critical_section);

#ifdef HOST_BUILD
    if (host_fbo_bound) {
        // Read back to shared CPU buffer for HostReadbackPixels fast path and
        // for debugging. Flipping is handled in HostReadbackPixels.
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, g_backingWidth, g_backingHeight, GL_RGBA, GL_UNSIGNED_BYTE, g_hostReadbackRGBA);
        g_hostReadbackReady = true;
        LOG_WARN("[drawrs] readback ready %d first %d %d %d", g_hostReadbackReady, g_hostReadbackRGBA[0],
                 g_hostReadbackRGBA[1], g_hostReadbackRGBA[2]);
        glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
    }
#endif
}

// ---------------------------------------------------------------------------
// Dynamic material display-list creation
// ---------------------------------------------------------------------------

// original 0x2e8a80 (_Z27NuDisplayListCreateMtlDlistP19nudisplaylistitem_sP7numtl_sP9variptr_uS3_).
// C++ linkage matches the original mangled symbol. `item` (param 1) and
// `buff_end` are not used by the body.
nudisplaylistitem_s *NuDisplayListCreateMtlDlist(nudisplaylistitem_s *item, NUMTL *mtl, VARIPTR *buff,
                                                 VARIPTR buff_end) {
    (void)item;
    (void)buff_end;

    nudisplaylistitem_s *out = (nudisplaylistitem_s *)buff->addr;
    buff->addr += 8 * sizeof(nudisplaylistitem_s);

    out->type = 0x85;
    out->id = 1; // NEXT
    out->next = out + 7;

    NuDisplayListAddClut(out + 1, mtl->tex_id);
    NuDisplayListAddTexture(out + 2, mtl->tex_id);
    NuDisplayListAddMaterialState(out + 3, mtl);
    NuDisplayListAddMicrocode(out + 4, mtl);
    NuDisplayListAddLightState(out + 5, mtl);

    out[6].type = 0x8b;
    out[6].id = 1; // NEXT
    out[6].next = out + 7;
    out[7].type = 0x84;
    out[7].id = 4; // RETURN
    out[7].next = NULL;

    return out;
}

// ---------------------------------------------------------------------------
// DisplayListLinkDynamicMtls
// ---------------------------------------------------------------------------

// Sort key used by the dynamic material lists:
//   ((char)(u16)mtl[0] >> 4) * 0x20000 + mtl->sort_pri
static i32 nudl_mtl_sort_key(const NUMTL *mtl) {
    return (i32)(i8)(((u32) * (const u16 *)mtl >> 4) & 0xff) * 0x20000 + mtl->sort_pri;
}

// original 0x2e8dc0 (_Z26DisplayListLinkDynamicMtlsv). C++ linkage matches the
// original mangled symbol.
//
// NOTE: dyn_mtl_dlist.nmtls is a fixed slot CAPACITY here (never modified by
// this function in the original - verified: no store to manager+0x450 exists
// between 0x2e8dc0 and 0x2e9a90); occupancy is tracked purely through the
// material_used[] byte array.
void DisplayListLinkDynamicMtls(void) {
    NUDLIST_MANAGER *mgr = &global_dlist_manager;

    if (mgr->nnew_materials == 0 && mgr->ndel_materials == 0) {
        return;
    }

    // ------------------------------------------------------------------
    // Removal pass (0x2e8df0..0x2e93df)
    // ------------------------------------------------------------------
    for (i32 di = 0; di < mgr->ndel_materials; di++) {
        NUMTL *mtl = mgr->del_materials[di];

        if (!(mtl->display_list != NULL && mgr->dyn_mtl_dlist.nmtls > 0)) {
            continue;
        }

        // Locate the material's slot (0x2e8e38).
        i32 idx;
        if (mgr->dyn_mtl_dlist.mtls[0] == mtl) {
            idx = 0;
        } else {
            idx = -1;
            for (u32 j = 1; j < mgr->dyn_mtl_dlist.nmtls; j++) {
                if (mgr->dyn_mtl_dlist.mtls[j] == mtl) {
                    idx = j;
                    break;
                }
            }
            if (idx < 0) {
                continue;
            }
        }

        if (--mgr->material_used[idx] != 0) {
            continue; // still referenced elsewhere
        }

        // Locate the sortpri covering this material (last key match wins,
        // 0x2e9011). nsort_pris < 1 yields sp_idx -1 in the original, whose
        // subsequent accesses run off the front of sort_pris[]; that state is
        // unreachable while every live material owns a sortpri, so it is
        // guarded here.
        const i32 key = nudl_mtl_sort_key(mtl);
        i32 sp_idx = -1;
        if (mgr->dyn_mtl_dlist.nsort_pris >= 1) {
            for (i32 j = 0; j < mgr->dyn_mtl_dlist.nsort_pris; j++) {
                if (key == mgr->dyn_mtl_dlist.sort_pris[j].sort_pri) {
                    sp_idx = j;
                }
            }
        }
        nusortpri_s *sp = (sp_idx >= 0) ? &mgr->dyn_mtl_dlist.sort_pris[sp_idx] : NULL;

        nudisplaylistitem_s *mtl_item = mtl->display_list->mtl_item;

        if (sp != NULL) {
            // Unlink the material's item block from its sortpri chain.
            if (sp->items == mtl_item) { // head case (0x2e98d5)
                sp->items = (nudisplaylistitem_s *)mtl_item[7].next;
                mtl_item[7].type = 0x84;
                mtl_item[7].id = 4;
                mtl_item[7].next = NULL;
            } else if (sp->nmtls != 0) {
                nudisplaylistitem_s *prev = mgr->dyn_mtl_dlist.mtls[sp->mtl_first]->display_list->mtl_item;
                bool found = (mtl_item == prev[7].next);
                for (u32 j = 0; !found && j + 1 < sp->nmtls; j++) {
                    prev = mgr->dyn_mtl_dlist.mtls[(u32)sp->mtl_first + j + 1]->display_list->mtl_item;
                    found = (mtl_item == prev[7].next);
                }
                if (found) { // sibling splice (0x2e9411)
                    prev[7].next = mtl_item[7].next;
                    mtl_item[7].type = 0x84;
                    mtl_item[7].id = 4;
                    mtl_item[7].next = NULL;
                }
                // not found: fall through without touching the chains
            }
        }

        // Free the material's 0x80-byte stream buffer (0x2e90c6).
        for (u32 j = 0; j < mgr->dyn_mtl_dlist.nmtls; j++) {
            if (mgr->mtl_buffers_used[j] != 0 &&
                mtl_item == (nudisplaylistitem_s *)((u8 *)mgr->mtlbuff.addr + j * 0x80)) {
                mgr->mtl_buffers_used[j] = 0;
                break;
            }
        }

        if (sp != NULL) {
            sp->nmtls--; // 0x2e9131
            if (sp->nmtls == 0) {
                // Unlink the empty sortpri from the global sort_list.
                if (mgr->sort_list == sp) {
                    mgr->sort_list = sp->sys_next;
                } else {
                    nusortpri_s *cur = mgr->sort_list;
                    while (cur != NULL && cur->sys_next != sp) {
                        cur = cur->sys_next;
                    }
                    if (cur != NULL) {
                        cur->sys_next = sp->sys_next;
                    }
                }
                mgr->nused_sort_pris--;

                // Compact the sortpri array (0x2e918d, 0x24-byte records).
                for (i32 j = sp_idx; j < mgr->dyn_mtl_dlist.nsort_pris - 1; j++) {
                    mgr->dyn_mtl_dlist.sort_pris[j] = mgr->dyn_mtl_dlist.sort_pris[j + 1];
                }
                mgr->dyn_mtl_dlist.nsort_pris--;
            }

            // Remove the slots at idx inside the fixed capacity: shift left
            // and re-place the evicted tail dlist record at the end
            // (0x2e9209..0x2e92a2).
            NUDISPLAYLIST *tail_dl = mgr->dyn_mtl_dlist.dlist_mtls[mgr->dyn_mtl_dlist.nmtls - 1];
            for (u32 j = idx; j + 1 < mgr->dyn_mtl_dlist.nmtls; j++) {
                mgr->dyn_mtl_dlist.mtls[j] = mgr->dyn_mtl_dlist.mtls[j + 1];
                mgr->dyn_mtl_dlist.dlist_mtls[j] = mgr->dyn_mtl_dlist.dlist_mtls[j + 1];
                mgr->material_used[j] = mgr->material_used[j + 1];
            }
            mgr->dyn_mtl_dlist.dlist_mtls[mgr->dyn_mtl_dlist.nmtls - 1] = tail_dl;
            mgr->material_used[mgr->dyn_mtl_dlist.nmtls - 1] = 0;

            // Reassign material ids of live records (0x2e92ae).
            for (u32 j = 0; j < mgr->dyn_mtl_dlist.nmtls; j++) {
                if (mgr->material_used[j] != 0) {
                    mgr->dyn_mtl_dlist.mtls[j]->display_list->mtl_id = j;
                }
            }

            // Recompute each sortpri's mtl_first prefix sum (0x2e92e6).
            u32 total = 0;
            for (i32 j = 0; j < mgr->dyn_mtl_dlist.nsort_pris; j++) {
                total += mgr->dyn_mtl_dlist.sort_pris[j].nmtls;
            }
            if (total != 0) {
                for (i32 j = 0; j < mgr->dyn_mtl_dlist.nsort_pris; j++) {
                    nusortpri_s *s2 = &mgr->dyn_mtl_dlist.sort_pris[j];
                    if (s2->nmtls == 0) {
                        continue;
                    }
                    for (u32 k = 0; k < total; k++) {
                        if (nudl_mtl_sort_key(mgr->dyn_mtl_dlist.mtls[k]) == s2->sort_pri) {
                            s2->mtl_first = (u16)k;
                            break;
                        }
                    }
                }
            }
        }

        mtl->display_list = NULL; // 0x2e93c6
    }
    mgr->ndel_materials = 0; // 0x2e8e8d

    // ------------------------------------------------------------------
    // Addition pass (0x2e8eb0..0x2e982f)
    // ------------------------------------------------------------------
    for (i32 ni = 0; ni < mgr->nnew_materials; ni++) {
        NUMTL *mtl = mgr->new_materials[ni];

        if (mgr->dyn_mtl_dlist.nmtls <= 0) {
            continue;
        }

        // Already present? Bump the refcount and move on (0x2e8ecd/0x2e8f50).
        bool present = false;
        i32 idx = -1;
        if (mgr->dyn_mtl_dlist.mtls[0] == mtl) {
            idx = 0;
            present = true;
        } else {
            for (u32 j = 1; j < mgr->dyn_mtl_dlist.nmtls; j++) {
                if (mgr->dyn_mtl_dlist.mtls[j] == mtl) {
                    idx = j;
                    present = true;
                    break;
                }
            }
        }
        if (present) {
            mgr->material_used[idx]++;
            continue;
        }

        // Find a free material slot (0x2e8eee/0x2e9a1e).
        i32 pos = -1;
        if (mgr->material_used[0] == 0) {
            pos = 0;
        } else {
            for (u32 j = 1; j < mgr->dyn_mtl_dlist.nmtls; j++) {
                if (mgr->material_used[j] == 0) {
                    pos = (i32)j;
                    break;
                }
            }
        }
        if (pos < 0) {
            continue;
        }

        // Find a free 0x80-byte stream buffer; mark it used immediately
        // (0x2e9442). No free buffer leaves buf_off at the -0x80 sentinel.
        i32 buf_off = -0x80;
        for (u32 j = 0; j < mgr->dyn_mtl_dlist.nmtls; j++) {
            if (mgr->mtl_buffers_used[j] == 0) {
                mgr->mtl_buffers_used[j] = 1;
                buf_off = j * 0x80;
                break;
            }
        }

        const i32 key = nudl_mtl_sort_key(mtl);

        // Existing sortpri with the same key? (last match wins, 0x2e94d0)
        i32 sp_idx = -1;
        if (mgr->dyn_mtl_dlist.nsort_pris > 0) {
            for (i32 j = 0; j < mgr->dyn_mtl_dlist.nsort_pris; j++) {
                if (key == mgr->dyn_mtl_dlist.sort_pris[j].sort_pri) {
                    sp_idx = j;
                }
            }
        }

        // Insertion position: before the first USED material whose key is
        // greater or equal, otherwise into the free slot found above
        // (0x2e9520; equality inserts before the existing entry).
        i32 insert = pos;
        for (u32 j = 0; j < mgr->dyn_mtl_dlist.nmtls; j++) {
            if (mgr->material_used[j] != 0 && key <= nudl_mtl_sort_key(mgr->dyn_mtl_dlist.mtls[j])) {
                insert = (i32)j;
                break;
            }
        }

        // Shift [insert..nmtls-2] right one slot inside the capacity and
        // place the evicted tail record at insert (0x2e9575..0x2e9614).
        NUDISPLAYLIST *old_tail_dl = mgr->dyn_mtl_dlist.dlist_mtls[mgr->dyn_mtl_dlist.nmtls - 1];
        for (i32 j = mgr->dyn_mtl_dlist.nmtls - 2; j >= insert; j--) {
            mgr->dyn_mtl_dlist.mtls[j + 1] = mgr->dyn_mtl_dlist.mtls[j];
            mgr->dyn_mtl_dlist.dlist_mtls[j + 1] = mgr->dyn_mtl_dlist.dlist_mtls[j];
            mgr->material_used[j + 1] = mgr->material_used[j];
        }
        mgr->dyn_mtl_dlist.dlist_mtls[insert] = old_tail_dl;
        mgr->material_used[insert] = 0; // 0x2e9627

        // Reassign ids of live records, then claim the new slot (0x2e967e).
        for (u32 j = 0; j < mgr->dyn_mtl_dlist.nmtls; j++) {
            if (mgr->material_used[j] != 0) {
                mgr->dyn_mtl_dlist.mtls[j]->display_list->mtl_id = j;
            }
        }
        mgr->material_used[insert] = 1;

        NUDISPLAYLIST *dl = mgr->dyn_mtl_dlist.dlist_mtls[insert];
        mtl->display_list = dl; // 0x2e9695

        // Build the material's item chain in its stream buffer (0x2e96be).
        VARIPTR buf_ptr;
        buf_ptr.addr = (usize)((u8 *)mgr->mtlbuff.addr + buf_off);
        nudisplaylistitem_s *items = NuDisplayListCreateMtlDlist(dl->mtl_item, mtl, &buf_ptr, mgr->mtlbuffend);

        mgr->dyn_mtl_dlist.mtls[insert] = mtl;          // 0x2e96d6
        dl->dyn_geom = items + 6;                       // 0x2e96e2
        dl->dlist_id = (u32)(usize)&mgr->dyn_mtl_dlist; // 0x2e96f2 stores &dyn scene
        dl->mtl_item = items;                           // 0x2e96f8
        dl->mtl_id = insert;                            // 0x2e96fb

        nusortpri_s *sp;
        u16 sp_nmtls;
        if (sp_idx == -1) {
            // Create a new sortpri and splice it into sort_list ordered by
            // ascending sort_pri (0x2e9924).
            sp = &mgr->dyn_mtl_dlist.sort_pris[mgr->dyn_mtl_dlist.nsort_pris];
            sp->items = items;
            sp->field_18 = mgr->field_4a8;
            sp->sort_pri = key;
            sp->nmtls = 0;

            if (mgr->sort_list == NULL || mgr->sort_list->sort_pri >= key) {
                sp->sys_next = mgr->sort_list;
                mgr->sort_list = sp;
            } else {
                nusortpri_s *cur = mgr->sort_list;
                while (cur->sys_next != NULL && cur->sys_next->sort_pri < key) {
                    cur = cur->sys_next;
                }
                sp->sys_next = cur->sys_next;
                cur->sys_next = sp;
            }
            mgr->nused_sort_pris++;
            mgr->dyn_mtl_dlist.nsort_pris++;
            sp_nmtls = 1;
        } else {
            sp = &mgr->dyn_mtl_dlist.sort_pris[sp_idx];
            if (insert != 0 && nudl_mtl_sort_key(mgr->dyn_mtl_dlist.mtls[insert - 1]) == key) {
                // Chain after the previous material's item block (0x2e99af).
                NUMTL *prev_mtl = mgr->dyn_mtl_dlist.mtls[insert - 1];
                nudisplaylistitem_s *anchor = prev_mtl->display_list->mtl_item;
                nudisplaylistitem_s *follow = (nudisplaylistitem_s *)anchor[7].next;
                items[7].type = 0x8d;
                items[7].id = 1; // NEXT
                items[7].next = follow;
                anchor[7].next = items;
            } else {
                // Head insertion into the sortpri's item chain (0x2e9732).
                items[7].type = 0x8d;
                items[7].id = 1; // NEXT
                items[7].next = sp->items;
                sp->items = items;
            }
            sp_nmtls = sp->nmtls + 1;
        }
        sp->nmtls = sp_nmtls; // shared tail store at 0x2e9774

        // Reassign ids again, then fix up every sortpri's mtl_first
        // (0x2e9772..0x2e9818).
        for (u32 j = 0; j < mgr->dyn_mtl_dlist.nmtls; j++) {
            if (mgr->material_used[j] != 0) {
                mgr->dyn_mtl_dlist.mtls[j]->display_list->mtl_id = j;
            }
        }
        u32 total = 0;
        for (i32 j = 0; j < mgr->dyn_mtl_dlist.nsort_pris; j++) {
            total += mgr->dyn_mtl_dlist.sort_pris[j].nmtls;
        }
        if (total != 0) {
            for (i32 j = 0; j < mgr->dyn_mtl_dlist.nsort_pris; j++) {
                nusortpri_s *s2 = &mgr->dyn_mtl_dlist.sort_pris[j];
                if (s2->nmtls == 0) {
                    continue;
                }
                for (u32 k = 0; k < total; k++) {
                    if (nudl_mtl_sort_key(mgr->dyn_mtl_dlist.mtls[k]) == s2->sort_pri) {
                        s2->mtl_first = (u16)k;
                        break;
                    }
                }
            }
        }
    }
    mgr->nnew_materials = 0; // 0x2e8f28
}

// ---------------------------------------------------------------------------
// End-of-frame processing
// ---------------------------------------------------------------------------

#define NUDL_MTL_ATTRIB_DWORD1(mtl) (*(const u32 *)((const u8 *)(mtl) + 0x44))

// original 0x2eaef0 (NuDisplayListSwapBuffersEndFrame)
extern "C" void NuDisplayListSwapBuffersEndFrame(void) {
    NUDLIST_MANAGER *mgr = &global_dlist_manager;

    NuThreadCriticalSectionBegin(mgr->loading_critical_section);
    DisplayListLinkDynamicMtls();
    DisplayListSwapBuffersPS();
    DisplayListMaterialClipUpdate(&mgr->dyn_mtl_dlist);

    for (i32 i = 0; i < mgr->ndisplay_lists; i++) {
        nudldlistscene_s *sc = mgr->dlists[i];
        u8 flags = sc->flags;
        if ((flags & 4) != 0) {
            // Clip-word diff pass over the double-buffered clip_used arrays
            // (u32 words, 4 two-bit codes per byte, 16 objects per word).
            u32 buf = (u32)(u8)(sc->render_buffer >> 7);
            const u8 *cur_words = sc->clip_used[buf];
            const u8 *other_words = sc->clip_used[buf ^ 1];
            i32 nclip = sc->nclip_objects;

            i32 words = nclip + 15; // ceil(nclip/16), signed-rounding kept
            if (nclip + 15 < 0) {
                words = nclip + 30;
            }
            words >>= 4;
            if (words > 0) {
                i32 obj = 0;
                for (i32 w = 0; w < words; w++) {
                    const u8 *curw = cur_words + w * 4;
                    if (*(const u32 *)curw == *(const u32 *)(other_words + w * 4)) {
                        obj += 16; // identical word: skip its 16 objects
                        continue;
                    }
                    bool done = false;
                    for (i32 b = 0; b < 4 && !done; b++) {
                        u8 byte_val = curw[b];
                        for (i32 f = 0; f < 8 && !done; f += 2) {
                            NUCLIPOBJECT *co = &sc->clip_objects[obj];
                            u32 code = (byte_val >> f) & 3;
                            u16 cnt = sc->clip_counts[obj];
                            for (u16 j = 0; j < cnt; j++) {
                                i32 idx = co->indices[j];
                                if (code == 0) {
                                    sc->items[idx].id = 0; // CNT
                                    sc->items[idx - 1].id = 0;
                                } else {
                                    sc->items[idx].id = 3; // CALL
                                    sc->items[idx - 1].id = 3;
                                }
                            }
                            obj++;
                            if (obj >= nclip) { // original exits both loops here
                                done = true;
                            }
                        }
                    }
                }
            }

            // Alpha fade pass (0x2eb164 region): per clip object, patch alpha
            // into every referenced geom item.
            if (sc->alpha_values != NULL && nclip > 0) {
                for (i32 o = 0; o < nclip; o++) {
                    const i32 *indices = sc->clip_objects[o].indices;
                    u16 cnt = sc->clip_counts[o];
                    for (u16 j = 0; j < cnt; j++) {
                        nudisplaylistitem_s *item = &sc->items[indices[j]];
                        if (item->id != 0 && item->type == 0x82) {
                            DisplayListSetAlphaPS(item - 1, item, sc->alpha_values[o]);
                        }
                    }
                }
            }

            flags = sc->flags; // refreshed after the passes above
        }
        if ((flags & 6) != 0) {
            DisplayListMaterialClipUpdate(sc);
        }
    }

    // Move every queued front render scene to its safe slot (0x2eb1xx). The
    // original special-cases slot 0, unrolls the body by four for k=0..4 and
    // keeps a tail loop for slots 21..23; the net effect is this loop.
    for (i32 s = 0; s < 24; s++) {
        mgr->safe_render_scenes[s] = mgr->front_render_scenes[s];
        mgr->front_render_scenes[s] = NULL;
    }

    // Per-sortpri render-state update walk (stack rndrstate initialised like
    // NuDisplayListBeforeFrame's reset block).
    nurndrstate_s tmp_state;
    tmp_state.mtl = NULL;
    tmp_state.tex_id = -1;
    tmp_state.konst_id = -1;
    tmp_state.global_id = -1;
    tmp_state.lights_id = -1;
    tmp_state.camera_id = -1;
    tmp_state.fog_id = -1;
    tmp_state.reflection_id = -1;
    struct nuglobalrndrstate_s *last_local_state = NULL;

    for (nusortpri_s *sp = mgr->sort_list; sp != NULL;) {
        if (sp == NULL) {
            break;
        }
        nudldlistscene_s *sc = sp->display_scene;
        if (sc == NULL) {
            RndrStateUpdateFx(&tmp_state, sp->items);
        } else if (((sc->flags & 6) != 0) && sp->nmtls != 0) {
            for (u32 m = 0; m < sp->nmtls; m++) {
                i32 mi = (i32)(u32)sp->mtl_first + (i32)m;
                NUDISPLAYLIST *dl = sc->dlist_mtls[mi];
                if (dl->mtl_item->id == 0) {
                    NUMTL *mtl = sc->mtls[mi];
                    nuglobalrndrstate_s *ls = sc->local_state;
                    nudisplaylistitem_s *first;

                    if (ls == NULL || (sc->flags & 4) == 0) {
                        first = dl->first;
                        last_local_state = NULL;
                        if (dl->mtl_last != first) {
                            goto copy_dyn; // LAB_002fb382
                        }
                    } else {
                        if (!(dl->mtl_last == dl->first && ls == last_local_state)) {
                            DisplayListUpdateRenderState(dl, ls);
                            last_local_state = sc->local_state;
                            if (dl->mtl_last != dl->first) {
                                first = dl->first;
                                goto copy_dyn; // into LAB_002fb382 above
                            }
                            goto dyn_geom_link; // LAB_002fb4a0
                        }
                    dyn_geom_link:
                        if (dl->scene_first[dl->scene_buffer] == dl->scene_next) {
                            dl->dyn_geom->next = dl->dyn_geom + 1;
                        } else {
                            dl->dyn_geom->next = dl->scene_first[dl->scene_buffer];
                            dl->scene_next->type = 0x8d;
                            dl->scene_next->id = 1;
                            dl->scene_next->next = dl->dyn_geom + 1;
                        }
                        goto state_update;
                    }
                    goto state_update;

                copy_dyn:
                    *dl->dyn_geom = *first;
                    if ((sc->flags & 4) == 0) {
                        dl->mtl_last->next = dl->mtl_item->next;
                    } else if (dl->scene_first[dl->scene_buffer] != dl->scene_next) {
                        dl->mtl_last->next = dl->scene_first[dl->scene_buffer];
                        dl->scene_next->type = 0x8d;
                        dl->scene_next->id = 1;
                        dl->scene_next->next = dl->dyn_geom + 1;
                    }
                    dl->mtl_last = first;

                state_update:
                    RndrStateUpdate(&tmp_state, mtl, dl->mtl_item);

                    // Visibility cull: when any visibility bit is requested
                    // and this material fails the test, mark its item NEXT.
                    if ((*(const u16 *)&sc->flags & 0x1fe0) != 0 &&
                        ((u8)((u32) * (const u16 *)&sc->flags >> 5) & (u8)(NUDL_MTL_ATTRIB_DWORD1(mtl) >> 0xe)) != 0) {
                        dl->mtl_item->id = 1;
                        i32 mid = dl->mtl_id;
                        i32 t = (mid >= 0) ? mid : mid + 7; // round-toward-zero /8
                        i32 mbit = mid & 7;
                        u8 *mu = sc->mtl_used[(u32)(u8)(sc->render_buffer >> 7)];
                        mu[t >> 3] &= (u8)(((u32)-2 << mbit) | ((u32)0xfffffffe >> (32 - mbit)));
                    }
                }
            }
        }
        sp = sp->sys_next;
    }

    // Early-exit path of the original loop head: nothing to draw - reset the
    // local render states of all display lists and leave.
    for (i32 i = 0; i < mgr->ndisplay_lists; i++) {
        nudldlistscene_s *sc = mgr->dlists[i];
        if (sc->local_state != NULL) {
            RndrStateResetGlobalState(sc->local_state);
        }
    }

    NuThreadCriticalSectionEnd(mgr->loading_critical_section);
}

// original 0x2e9d50 — the Android build's 2D display-list draw is empty
// (9 bytes of padding); 2D content reaches the screen through the same
// render-scene executor.
extern "C" void NuDisplayListDraw2D(void) {
}
