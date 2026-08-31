// nudlist.cpp — Display-list manager
//
// Transcribed from libTTapp.so (Android x86). Builds and executes the
// per-material / per-scene display-list chains the renderer consumes.
//
// Each chain is a sequence of 16-byte `nudisplaylistitem_s` records:
//   id = 0 (CNT)  — no-op, advance to next record
//   id = 1 (NEXT) — jump to `item->next`
//   id = 3 (CALL) — dispatch `handler[item->type](item->next)`
//   id >=4 (RET)  — terminate execution
// `NuDisplayListExecute` walks a chain until RET or a NEXT that leaves the
// linear run. Double-buffered `clip_used` / `mtl_used` bitsets and the
// `render_buffer` toggle drive the per-frame visibility updates.

#include "nu2api/nu3d/nudlist.h"

#include "decomp.h"
#include "nu2api/nu3d/nutex.h"

#include "nu2api/nucore/numem.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/numtl.h"

extern i32 numtl_renderplane;

// ──────────────────────────────────────────────────────────────────────────────
// Constants
// ──────────────────────────────────────────────────────────────────────────────

static constexpr usize kItemSize = 0x10;

static constexpr u8 kItemId_Cnt = 0;
static constexpr u8 kItemId_Next = 1;
static constexpr u8 kItemId_Call = 3;
static constexpr u8 kItemId_Ret = 4;

static constexpr u8 kItemType_Mtl = 0x80;
static constexpr u8 kItemType_Terminator = 0x84;
static constexpr u8 kItemType_Nop = 0x87;
static constexpr u8 kItemType_Link = 0x8d;

// ──────────────────────────────────────────────────────────────────────────────
// Globals
// ──────────────────────────────────────────────────────────────────────────────

extern VARIPTR rndrstream_end;
extern VARIPTR rndrstream_free;

extern "C" {
    VARIPTR *display_list_buffer = nullptr;
    VARIPTR *display_list_buffer_end = nullptr;

    // Manager lives in original bss @0x11a0080 (0x604 bytes).
    NUDLIST_MANAGER global_dlist_manager = {0};
}

// Scratch backing for the 2D list's item cursor so AddItem is safe before
// NuDisplayListInit has run (host safety net).
static u8 s_2d_item_scratch[0x100] = {0};
static u8 s_2d_state_storage[0x40] = {0};

// Capture debug state (bss @0x11a0070 / @0x11a0068).
static i32 s_capture_enabled; // do_capture
static i32 s_capture_fh;      // capture file handle

// ──────────────────────────────────────────────────────────────────────────────
// Dispatch tables
// ──────────────────────────────────────────────────────────────────────────────

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

// Primary table — __ItemFnTable @0x625ae0 (sparse, indexed by absolute type).
nudl_handler_fn g_nudl_dispatch_table[0x100] = {nullptr};
static nudl_handler_fn s_shadow_table_storage[0x100] = {nullptr};
static const nudl_handler_fn *s_shadow_table = s_shadow_table_storage;

struct NudlTableInit {
    NudlTableInit() {
        auto *t = g_nudl_dispatch_table;
        auto *s = s_shadow_table_storage;
        t[0x80] = NuIOSDLMtlCallback;
        t[0x82] = NuIOSDLGeomCallback;
        t[0x83] = NuIOSDLTransformCallback;
        t[0x8b] = NuIOSDLGeomCallback;
        t[0x8c] = NuIOSDLTransformParamsCallback;
        t[0x8f] = NuIOSDLFaceOnCallback;
        t[0x90] = NuIOSDLFaceOnTransformCallback;
        t[0x93] = NuIOSDLGeom2DCallback;
        t[0x94] = NuIOSDLLightsCallback;
        t[0x98] = NuIOSDLGeomCallback;
        t[0x99] = NuIOSDLSkinMtxCallback;
        t[0x9a] = NuIOSDLCameraCallback;
        t[0xa5] = NuIOSDLKonstCallback;
        t[0xa6] = NuIOSDLFogCallback;
        t[0xa7] = NuIOSDLDebrisCallback;
        t[0xa9] = NuIOSDLVertexGroupsCallback;
        t[0xaa] = NuIOSDLVertexOffsetsCallback;
        t[0xab] = NuIOSDLReflectionCallback;
        t[0xae] = NuIOSDLLightmapOld;
        t[0xaf] = NuIOSDLLightmapOffsetOld;
        t[0xb0] = NuIOSDLLightmap;

        s[0x80] = NuIOSDLDeferredMtlCallback;
        s[0x82] = NuIOSDLGeomCallback;
        s[0x83] = NuIOSDLDeferredTransformCallback;
        s[0x8b] = NuIOSDLGeomCallback;
        s[0x8c] = NuIOSDLDeferredTransformParamsCallback;
        s[0x98] = NuIOSDLGeomCallback;
        s[0x99] = NuIOSDLSkinMtxCallback;
        s[0xa9] = NuIOSDLVertexGroupsCallback;
        s[0xaa] = NuIOSDLVertexOffsetsCallback;
    }
};
static NudlTableInit s_nudl_table_init;

// CurrentItemTable @0x625c84 — points at entry for type 0x80.
static const nudl_handler_fn *s_current_table = nullptr;
struct NudlCurrentTableInit {
    NudlCurrentTableInit() {
        s_current_table = &g_nudl_dispatch_table[0x80];
    }
};
static NudlCurrentTableInit s_nudl_current_init;

// ──────────────────────────────────────────────────────────────────────────────
// Item helpers
// ──────────────────────────────────────────────────────────────────────────────

static inline void SetItemId(nudisplaylistitem_s *item, u8 id) {
    item->id = id;
}

static inline void SetItemNext(nudisplaylistitem_s *item, void *next) {
    item->next = next;
}

static void SetItemWithId(nudisplaylistitem_s *item, u8 type, u8 id, void *next) {
    item->type = type;
    item->next = next;
    item->id = id;
}

static nudisplaylistitem_s *AddCallItem(nudisplaylist_s *list, u8 type, void *next) {
    nudisplaylistitem_s *item = list->items;
    item->type = type;
    item->id = kItemId_Call;
    item->next = next;
    list->items = reinterpret_cast<nudisplaylistitem_s *>(reinterpret_cast<u8 *>(list->items) + kItemSize);
    return reinterpret_cast<nudisplaylistitem_s *>(reinterpret_cast<u8 *>(list->items) - kItemSize);
}

extern "C" void NuDisplayListCheckBuffer(void) {
}

extern "C" nudisplaylist_s *NuDisplayListGet2dList(void) {
    nudisplaylist_s *list = &global_dlist_manager.dlist_2d;
    if (list->items == nullptr) {
        list->items = reinterpret_cast<nudisplaylistitem_s *>(s_2d_item_scratch);
        if (list->state == nullptr) {
            list->state = reinterpret_cast<nurndrstate_s *>(s_2d_state_storage);
        }
    }
    return list;
}

extern "C" void NuDisplayListResetBuffer(void) {
    display_list_buffer = reinterpret_cast<VARIPTR *>(&rndrstream_free);
    display_list_buffer_end = reinterpret_cast<VARIPTR *>(rndrstream_end.addr);
}

// Small builders used by NuDisplayListCreateMtlDlist — second args are
// ignored in this build (verified against objdump).
extern "C" void NuDisplayListAddClut(nudisplaylistitem_s *item, i32 /*clut_id*/) {
    SetItemWithId(item, kItemType_Nop, kItemId_Cnt, nullptr);
}
extern "C" void NuDisplayListAddTexture(nudisplaylistitem_s *item, i32 /*tex_id*/) {
    SetItemWithId(item, kItemType_Nop, kItemId_Cnt, nullptr);
}
extern "C" void NuDisplayListAddMaterialState(nudisplaylistitem_s *item, void *mtl) {
    SetItemWithId(item, kItemType_Mtl, kItemId_Call, mtl);
}
extern "C" void NuDisplayListAddMicrocode(nudisplaylistitem_s *item, void * /*mtl*/) {
    SetItemWithId(item, kItemType_Nop, kItemId_Cnt, nullptr);
}
extern "C" void NuDisplayListAddLightState(nudisplaylistitem_s *item, void * /*mtl*/) {
    SetItemWithId(item, kItemType_Nop, kItemId_Cnt, nullptr);
}

// ──────────────────────────────────────────────────────────────────────────────
// Executor
// ──────────────────────────────────────────────────────────────────────────────

extern "C" void NuDisplayListExecute(nudisplaylistitem_s *item, const nudl_handler_fn *item_table) {
    // `item_table` points at the entry for type 0x80.
    for (;;) {
        // Walk the linear run until a NEXT links elsewhere.
        while (item->id != kItemId_Next) {
            if (item->id == kItemId_Call) {
                auto handler = item_table[static_cast<u32>(item->type) - kItemType_Mtl];
                if (handler) {
                    handler(item->next);
                }
            } else if (item->id != kItemId_Cnt) {
                // RET (and any unexpected id >=2 other than CALL) terminates.
                return;
            }
            ++item;
        }
        item = static_cast<nudisplaylistitem_s *>(item->next);
    }
}

extern "C" void NuDisplayListDrawItems(nudisplaylistitem_s *items) {
    NuDisplayListExecute(items, s_current_table);
}

extern "C" void NuDisplayListSetItemTable(i32 which) {
    if (which == 0) {
        s_current_table = &g_nudl_dispatch_table[0x80];
    } else if (which == 1) {
        s_current_table = &s_shadow_table[0x80];
    }
}

extern "C" void DisplayListSwapBuffersPS(void) {
}

extern "C" void DisplayListSetAlphaPS(nudisplaylistitem_s *prev_item, nudisplaylistitem_s *item, f32 alpha) {
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    *reinterpret_cast<f32 *>(reinterpret_cast<u8 *>(prev_item->next) + 0x3c) = alpha;
    (void)item; // second param kept for original signature parity
}

// ──────────────────────────────────────────────────────────────────────────────
// Debug capture
// ──────────────────────────────────────────────────────────────────────────────

extern "C" void NuDisplayListCaptureSortPriority(nusortpri_s *sort_pri) {
    if (!s_capture_enabled) {
        return;
    }

    const char *name = nullptr;
    if (sort_pri->display_scene) {
        name = sort_pri->display_scene->name;
    }
    if (name == nullptr) {
        NuHtmlHeading1("Display Scene : UNKNOWN (sort: %d)", sort_pri->sort_pri);
    } else {
        NuHtmlHeading1("Display Scene : %s (sort: %d)", name, sort_pri->sort_pri);
    }
    NuHtmlWrite("<font face=courier new>\n");

    nudisplaylistitem_s *item = sort_pri->items;
    if (item == nullptr) {
        return;
    }

    i32 idx = 0;
    i32 printed_idx = 0;
    bool hit_terminator = false;
    do {
        while (true) {
            printed_idx = idx;
            DisplayListPrintItem(item, printed_idx, 0, nullptr, s_capture_fh);
            if (item->id != kItemId_Next) {
                break;
            }
            item = static_cast<nudisplaylistitem_s *>(item->next);
            ++idx;
            if (item->type == kItemType_Terminator) {
                hit_terminator = true;
                break;
            }
        }
        if (hit_terminator) {
            break;
        }
        ++item;
        idx = printed_idx + 1;
    } while (item->type != kItemType_Terminator);
    DisplayListPrintItem(item, printed_idx + 1, 0, nullptr, s_capture_fh);
}

// ──────────────────────────────────────────────────────────────────────────────
// Per-material reset
// ──────────────────────────────────────────────────────────────────────────────

extern "C" void NuDisplayListReset(nudisplaylist_s *dl) {
    nurndrstate_s *st = dl->state;
    st->mtl = nullptr;
    st->tex_id = -1;
    st->global_id = -1;
    st->lights_id = -1;
    st->camera_id = -1;
    st->fog_id = -1;
    st->konst_id = -1;
    st->reflection_id = -1;
    dl->mtl_last = dl->first;
}

static void ResetRenderStateCache(nurndrstate_s *st) {
    st->mtl = nullptr;
    st->tex_id = -1;
    st->global_id = -1;
    st->lights_id = -1;
    st->camera_id = -1;
    st->fog_id = -1;
    st->konst_id = -1;
    st->reflection_id = -1;
}

// ──────────────────────────────────────────────────────────────────────────────
// Frame setup
// ──────────────────────────────────────────────────────────────────────────────

// Clip bitsets are double-buffered; render_buffer bit7 selects the active one.
// Each clip object uses 2 bits, packed into bytes. The block count is the
// number of 16-byte blocks needed to hold the bits plus one sentinel block.
static i32 ClipUsedBlockCount(i32 nclip) {
    // Original keeps signed arithmetic for the (-15) path, but nclip is
    // always >=0 here. Simplified to the unsigned ceil form:
    //   blocks = ((ceil(nclip/8)*2 + 15)/16) + 1
    if (nclip <= 0)
        return 0;
    i32 words16 = ((nclip + 7) >> 3) * 2; // bytes rounded to 8, doubled
    return (words16 + 15) / 16 + 1;       // 16-byte blocks + sentinel
}

static void ResetSceneBeforeFrame(nudldlistscene_s *scene, bool gated) {
    u8 buf = static_cast<u8>(scene->render_buffer >> 7);

    if (gated && (scene->flags & 6) == 0) {
        return;
    }

    if (scene->nmtls != 0) {
        NuMemSet128(scene->mtl_used[buf], 0, ((scene->nmtls + 7) >> 7) + 1);
    }

    if (scene->nclip_objects != 0) {
        i32 blocks = ClipUsedBlockCount(scene->nclip_objects);
        NuMemSet128(scene->clip_used[buf], 0, blocks);
        if (!gated) {
            // Ungated variant re-initialises the back buffer to all-ones.
            NuMemSet128(scene->clip_used[buf ^ 1], 0xff, blocks);
        }
    }

    for (u32 i = 0; i < scene->nmtls; ++i) {
        NUDISPLAYLIST *dl = scene->dlist_mtls[i];
        if (!gated) {
            dl->scene_buffer = 0;
        } else {
            u32 new_buf = (dl->scene_buffer == 0) ? 1 : 0;
            dl->scene_buffer = new_buf;
        }
        dl->mtl_last = dl->first;
        dl->scene_next = dl->scene_first[dl->scene_buffer];
        ResetRenderStateCache(dl->state);
    }
    scene->flags &= 0xf1;
}

// NuDisplaySceneAdd @ 0x2f9e30
extern "C" void NuDisplaySceneAdd(NUDLDLISTSCENE *scene) {
    NuThreadCriticalSectionBegin(global_dlist_manager.loading_critical_section);

    global_dlist_manager.dlists[global_dlist_manager.ndisplay_lists++] = scene;
    ResetSceneBeforeFrame(scene, /*gated=*/false);

    NUSORTPRI *sort_list = global_dlist_manager.sort_list;
    for (i32 i = 0; i < scene->nsort_pris; ++i) {
        NUSORTPRI *sort_pri = &scene->sort_pris[i];
        if (numtl_renderplane != 0) {
            sort_pri->sort_pri += numtl_renderplane * 0x20000;
        }

        NUSORTPRI *previous = nullptr;
        NUSORTPRI *current = sort_list;
        while (current != nullptr && current->sort_pri < sort_pri->sort_pri) {
            previous = current;
            current = current->sys_next;
        }
        sort_pri->sys_next = current;
        if (previous == nullptr) {
            sort_list = sort_pri;
        } else {
            previous->sys_next = sort_pri;
        }
        ++global_dlist_manager.nused_sort_pris;
    }
    global_dlist_manager.sort_list = sort_list;

    if (scene->field_8c != nullptr) {
        *reinterpret_cast<void **>(scene->field_8c) = global_dlist_manager.mtlanim_list;
        global_dlist_manager.mtlanim_list = scene->field_8c;
    }
    scene->flags &= 0xef;
    scene->render_buffer &= 0xdf;
    scene->alpha_values = nullptr;
    NuDisplaySceneAddPS(scene);

    NuThreadCriticalSectionEnd(global_dlist_manager.loading_critical_section);
}

// NuDisplaySceneAddPS @ 0x2ab7aa.  The apparently redundant assignment is
// present in the Android original.
extern "C" void NuDisplaySceneAddPS(NUDLDLISTSCENE *scene) {
    for (i32 i = 0; i < scene->nitems; ++i) {
        if (scene->items[i].type == 0x82) {
            scene->items[i].type = 0x82;
        }
    }
}

// NuDisplaySceneDestroy @ 0x2f9fd0
extern "C" void NuDisplaySceneDestroy(NUDLDLISTSCENE *scene) {
    if (scene == nullptr) {
        return;
    }

    NuThreadCriticalSectionBegin(global_dlist_manager.loading_critical_section);
    NuDisplaySceneDestroyPS(scene);

    i32 scene_index = 0;
    while (global_dlist_manager.dlists[scene_index] != scene) {
        ++scene_index;
    }

    NUSORTPRI *sort_list = global_dlist_manager.sort_list;
    for (i32 i = 0; i < scene->nsort_pris; ++i) {
        NUSORTPRI *sort_pri = &scene->sort_pris[i];
        if (sort_list == sort_pri) {
            sort_list = sort_pri->sys_next;
            continue;
        }

        NUSORTPRI *previous = sort_list;
        while (previous->sys_next != sort_pri) {
            previous = previous->sys_next;
        }
        previous->sys_next = sort_pri->sys_next;
    }
    global_dlist_manager.sort_list = sort_list;
    global_dlist_manager.nused_sort_pris -= scene->nsort_pris;

    if (scene->field_8c != nullptr) {
        void *node = global_dlist_manager.mtlanim_list;
        if (node == scene->field_8c) {
            global_dlist_manager.mtlanim_list =
                *reinterpret_cast<void **>(reinterpret_cast<u8 *>(scene->field_8c) + 0xc);
        } else {
            while (*reinterpret_cast<void **>(reinterpret_cast<u8 *>(node) + 0xc) != scene->field_8c) {
                node = *reinterpret_cast<void **>(reinterpret_cast<u8 *>(node) + 0xc);
            }
            *reinterpret_cast<void **>(reinterpret_cast<u8 *>(node) + 0xc) =
                *reinterpret_cast<void **>(reinterpret_cast<u8 *>(scene->field_8c) + 0xc);
        }
    }

    i32 removed = 0;
    for (i32 i = 0; i < global_dlist_manager.ndisplay_lists; ++i) {
        global_dlist_manager.dlists[i - removed] = global_dlist_manager.dlists[i];
        if (global_dlist_manager.dlists[i] == scene) {
            ++removed;
        }
    }
    if (removed != 0) {
        --global_dlist_manager.ndisplay_lists;
    }

    NuThreadCriticalSectionEnd(global_dlist_manager.loading_critical_section);
}

// NuDisplaySceneDestroyPS @ 0x2ab7f9
extern "C" void NuDisplaySceneDestroyPS(NUDLDLISTSCENE *) {
}

extern "C" void NuDisplayListSwapBuffersBeginFrame(void) {
    NuThreadCriticalSectionBegin(global_dlist_manager.loading_critical_section);

    // Flip the dynamic-material scene's render buffer.
    u32 flip = (~global_dlist_manager.dyn_mtl_dlist.render_buffer) & 0x80;
    global_dlist_manager.dyn_mtl_dlist.render_buffer &= 0x7f;
    global_dlist_manager.dyn_mtl_dlist.render_buffer |= flip;
    ResetSceneBeforeFrame(&global_dlist_manager.dyn_mtl_dlist, /*gated=*/true);

    for (i32 i = 0; i < global_dlist_manager.ndisplay_lists; ++i) {
        nudldlistscene_s *sc = global_dlist_manager.dlists[i];
        u32 nv = (~static_cast<u32>(static_cast<u8>(sc->render_buffer))) & 0xffffff80;
        sc->render_buffer &= 0x7f;
        sc->render_buffer |= nv;
        ResetSceneBeforeFrame(sc, /*gated=*/true);
        if (sc->gscene) {
            *reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(sc->gscene) + 0x44) = 0;
        }
    }

    global_dlist_manager.nrender_scenes = 0;
    for (nusortpri_s *sp = global_dlist_manager.sort_list; sp; sp = sp->sys_next) {
        sp->flags &= 0xfd;
    }

    RndrStateResetSharedGlobalState();
    NuThreadCriticalSectionEnd(global_dlist_manager.loading_critical_section);
}

// ──────────────────────────────────────────────────────────────────────────────
// Material clipping
// ──────────────────────────────────────────────────────────────────────────────

// NUMTL layout helpers — these fields are at fixed offsets in the original
// binary (verified against NuDisplayListSwapBuffersEndFrame visibility test).
#define MTL_BLEND_FLAG(mtl) (*(const u32 *)((const u8 *)(mtl) + 0xb0))
#define MTL_BLEND_OP2(mtl) (*(const u8 *)((const u8 *)(mtl) + 0xf8))
#define MTL_ATTRIB_DWORD1(mtl) (*(const u32 *)((const u8 *)(mtl) + 0x44))

static void UpdateMaterialClipBits(nudldlistscene_s *scene) {
    if (!scene || !scene->mtls || !scene->mtls[0]) {
        return;
    }
    u8 buf = static_cast<u8>(scene->render_buffer >> 7);
    const u8 *cur = scene->mtl_used[buf];
    const u8 *other = scene->mtl_used[buf ^ 1];
    u32 nbytes = (scene->nmtls + 7) >> 3;
    if (nbytes == 0)
        return;

    for (u32 byte_i = 0; byte_i < nbytes; ++byte_i) {
        u8 cur_byte = cur[byte_i];
        if (other[byte_i] == cur_byte)
            continue;
        for (u32 bit = 0; bit < 8; ++bit) {
            i32 idx = static_cast<i32>(byte_i * 8 + bit);
            if (idx >= static_cast<i32>(scene->nmtls))
                return;
            NUDISPLAYLIST *dl = scene->dlist_mtls[idx];
            NUMTL *mtl = scene->mtls[dl->mtl_id];
            bool enabled = (cur_byte >> bit) & 1;
            // Materials with blend-op 0xff are never drawn via this path.
            if (enabled && mtl && MTL_BLEND_FLAG(mtl) != 0 && MTL_BLEND_OP2(mtl) == 0xff) {
                enabled = false;
            }
            dl->mtl_item->id = enabled ? kItemId_Cnt : kItemId_Next;
        }
    }
}

// ──────────────────────────────────────────────────────────────────────────────
// Render-scene queueing
// ──────────────────────────────────────────────────────────────────────────────

extern "C" i32 NuDisplayListAddRenderScene(void) {
    NUDLIST_MANAGER *mgr = &global_dlist_manager;
    VARIPTR *buf = display_list_buffer;

    nusortpri_s **pris = reinterpret_cast<nusortpri_s **>((buf->addr + 0xfU) & ~0xfU);
    buf->addr = reinterpret_cast<usize>(pris + mgr->nused_sort_pris + 1);

    i32 count = 0;
    for (nusortpri_s *sp = mgr->sort_list; sp; sp = sp->sys_next) {
        nudldlistscene_s *sc = sp->display_scene;
        if (sc == nullptr) {
            // FX sortpri — carry over unless already captured this frame.
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
            // Rebuild this sortpri's material chains into the stream buffer.
            nusortpri_s *new_sp = reinterpret_cast<nusortpri_s *>((buf->addr + 3U) & ~0x3U);
            nudisplaylistitem_s *area = reinterpret_cast<nudisplaylistitem_s *>(new_sp + 1);
            buf->addr = reinterpret_cast<usize>(new_sp + 1);

            new_sp->display_scene = sc;
            new_sp->nmtls = sp->nmtls;
            new_sp->mtl_first = sp->mtl_first;
            new_sp->sort_pri = sp->sort_pri;
            new_sp->items = area;

            i32 slot = 0;
            for (u32 m = 0; m < sp->nmtls; ++m) {
                NUDISPLAYLIST *dl = sc->dlist_mtls[static_cast<u32>(sp->mtl_first) + m];
                if (dl->mtl_last == dl->first)
                    continue;

                const nudisplaylistitem_s *hdr =
                    (dl->mtl_item[1].type == kItemType_Mtl) ? dl->mtl_item + 1 : dl->mtl_item + 3;
                area[slot] = *hdr;
                area[slot + 1] = *dl->first;
                slot += 2;
                dl->mtl_last->next = &area[slot];
                dl->mtl_last = dl->first;
                ResetRenderStateCache(dl->state);
            }
            area[slot].type = kItemType_Terminator;
            area[slot].id = kItemId_Ret;
            area[slot].next = nullptr;
            buf->addr += static_cast<usize>(slot + 1) * kItemSize;

            pris[count++] = new_sp;
            new_sp->flags |= 2;
        }
    }

    if (count == 0 && mgr->dlist_2d.mtl_last == mgr->dlist_2d.first) {
        return -1;
    }

    auto *rs = reinterpret_cast<nudisplaylistrenderscene_s *>((buf->addr + 3U) & ~0x3U);
    mgr->front_render_scenes[mgr->nrender_scenes] = rs;
    buf->addr = reinterpret_cast<usize>(rs + 1);
    rs->nsort_pris = count;
    rs->sort_pris = pris;

    if (mgr->dlist_2d.mtl_last == mgr->dlist_2d.first) {
        rs->render_2d_first.type = kItemType_Terminator;
        rs->render_2d_first.id = kItemId_Ret;
        rs->render_2d_first.next = nullptr;
    } else {
        mgr->dlist_2d.mtl_last->type = kItemType_Terminator;
        mgr->dlist_2d.mtl_last->id = kItemId_Ret;
        mgr->dlist_2d.mtl_last->next = nullptr;
        rs->render_2d_first = *mgr->dlist_2d.first;
        NuDisplayListReset(&mgr->dlist_2d);
    }

    return mgr->nrender_scenes++;
}

extern "C" __attribute__((weak)) void NuDisplayListDrawRenderScene(i32 render_scene_id) {
    NUDLIST_MANAGER *mgr = &global_dlist_manager;

    NuThreadCriticalSectionBegin(mgr->loading_critical_section);
    nudisplaylistrenderscene_s *rs = mgr->safe_render_scenes[render_scene_id];
    if (rs) {
        for (i32 i = 0; i < rs->nsort_pris; ++i) {
            nusortpri_s *sp = rs->sort_pris[i];
            NuDisplayListCaptureSortPriority(sp);
            NuDisplayListDrawItems(sp->items);
        }
        NuDisplayListDrawItems(&rs->render_2d_first);
        mgr->safe_render_scenes[render_scene_id] = nullptr;
    }
    NuThreadCriticalSectionEnd(mgr->loading_critical_section);
}

// ──────────────────────────────────────────────────────────────────────────────
// Dynamic material display lists
// ──────────────────────────────────────────────────────────────────────────────

nudisplaylistitem_s *NuDisplayListCreateMtlDlist(nudisplaylistitem_s * /*item*/, NUMTL *mtl, VARIPTR *buff,
                                                 VARIPTR /*buff_end*/) {
    auto *out = reinterpret_cast<nudisplaylistitem_s *>(buff->addr);
    buff->addr += 8 * kItemSize;

    out->type = 0x85;
    out->id = kItemId_Next;
    out->next = out + 7;

    NuDisplayListAddClut(out + 1, mtl->tex_id);
    NuDisplayListAddTexture(out + 2, mtl->tex_id);
    NuDisplayListAddMaterialState(out + 3, mtl);
    NuDisplayListAddMicrocode(out + 4, mtl);
    NuDisplayListAddLightState(out + 5, mtl);

    out[6].type = 0x8b;
    out[6].id = kItemId_Next;
    out[6].next = out + 7;
    out[7].type = kItemType_Terminator;
    out[7].id = kItemId_Ret;
    out[7].next = nullptr;

    return out;
}

// ──────────────────────────────────────────────────────────────────────────────
// Dynamic material link / unlink
// ──────────────────────────────────────────────────────────────────────────────

static i32 MtlSortKey(const NUMTL *mtl) {
    // Original: ((char)(u16)mtl[0] >> 4) * 0x20000 + sort_pri
    return static_cast<i32>(static_cast<i8>(((*reinterpret_cast<const u16 *>(mtl) >> 4) & 0xff))) * 0x20000 +
           mtl->sort_pri;
}

static void ReassignMtlIds(NUDLIST_MANAGER *mgr) {
    for (u32 i = 0; i < mgr->dyn_mtl_dlist.nmtls; ++i) {
        if (mgr->material_used[i] != 0) {
            mgr->dyn_mtl_dlist.mtls[i]->display_list->mtl_id = static_cast<i32>(i);
        }
    }
}

static void RecomputeMtlFirst(NUDLIST_MANAGER *mgr) {
    u32 total = 0;
    for (i32 i = 0; i < mgr->dyn_mtl_dlist.nsort_pris; ++i) {
        total += mgr->dyn_mtl_dlist.sort_pris[i].nmtls;
    }
    if (total == 0)
        return;
    for (i32 i = 0; i < mgr->dyn_mtl_dlist.nsort_pris; ++i) {
        nusortpri_s *sp = &mgr->dyn_mtl_dlist.sort_pris[i];
        if (sp->nmtls == 0)
            continue;
        for (u32 k = 0; k < total; ++k) {
            if (MtlSortKey(mgr->dyn_mtl_dlist.mtls[k]) == sp->sort_pri) {
                sp->mtl_first = static_cast<u16>(k);
                break;
            }
        }
    }
}

static void DisplayListLinkDynamicMtls(void) {
    NUDLIST_MANAGER *mgr = &global_dlist_manager;
    if (mgr->nnew_materials == 0 && mgr->ndel_materials == 0)
        return;

    // ── Removal pass ──
    for (i32 di = 0; di < mgr->ndel_materials; ++di) {
        NUMTL *mtl = mgr->del_materials[di];
        if (!mtl->display_list || mgr->dyn_mtl_dlist.nmtls == 0)
            continue;

        // Locate slot.
        i32 idx = -1;
        if (mgr->dyn_mtl_dlist.mtls[0] == mtl) {
            idx = 0;
        } else {
            for (u32 j = 1; j < mgr->dyn_mtl_dlist.nmtls; ++j) {
                if (mgr->dyn_mtl_dlist.mtls[j] == mtl) {
                    idx = static_cast<i32>(j);
                    break;
                }
            }
            if (idx < 0)
                continue;
        }

        if (--mgr->material_used[idx] != 0)
            continue;

        // Find owning sortpri (last key match wins).
        i32 key = MtlSortKey(mtl);
        i32 sp_idx = -1;
        if (mgr->dyn_mtl_dlist.nsort_pris >= 1) {
            for (i32 j = 0; j < mgr->dyn_mtl_dlist.nsort_pris; ++j) {
                if (key == mgr->dyn_mtl_dlist.sort_pris[j].sort_pri)
                    sp_idx = j;
            }
        }
        nusortpri_s *sp = (sp_idx >= 0) ? &mgr->dyn_mtl_dlist.sort_pris[sp_idx] : nullptr;
        nudisplaylistitem_s *mtl_item = mtl->display_list->mtl_item;

        if (sp) {
            if (sp->items == mtl_item) {
                sp->items = static_cast<nudisplaylistitem_s *>(mtl_item[7].next);
                mtl_item[7].type = kItemType_Terminator;
                mtl_item[7].id = kItemId_Ret;
                mtl_item[7].next = nullptr;
            } else if (sp->nmtls != 0) {
                nudisplaylistitem_s *prev = mgr->dyn_mtl_dlist.mtls[sp->mtl_first]->display_list->mtl_item;
                bool found = (mtl_item == prev[7].next);
                for (u32 j = 0; !found && j + 1 < sp->nmtls; ++j) {
                    prev = mgr->dyn_mtl_dlist.mtls[static_cast<u32>(sp->mtl_first) + j + 1]->display_list->mtl_item;
                    found = (mtl_item == prev[7].next);
                }
                if (found) {
                    prev[7].next = mtl_item[7].next;
                    mtl_item[7].type = kItemType_Terminator;
                    mtl_item[7].id = kItemId_Ret;
                    mtl_item[7].next = nullptr;
                }
            }
        }

        // Free 0x80-byte stream buffer.
        for (u32 j = 0; j < mgr->dyn_mtl_dlist.nmtls; ++j) {
            if (mgr->mtl_buffers_used[j] != 0 &&
                mtl_item ==
                    reinterpret_cast<nudisplaylistitem_s *>(reinterpret_cast<u8 *>(mgr->mtlbuff.addr) + j * 0x80)) {
                mgr->mtl_buffers_used[j] = 0;
                break;
            }
        }

        if (sp) {
            if (--sp->nmtls == 0) {
                // Unlink empty sortpri.
                if (mgr->sort_list == sp) {
                    mgr->sort_list = sp->sys_next;
                } else {
                    for (nusortpri_s *cur = mgr->sort_list; cur; cur = cur->sys_next) {
                        if (cur->sys_next == sp) {
                            cur->sys_next = sp->sys_next;
                            break;
                        }
                    }
                }
                --mgr->nused_sort_pris;
                for (i32 j = sp_idx; j < mgr->dyn_mtl_dlist.nsort_pris - 1; ++j) {
                    mgr->dyn_mtl_dlist.sort_pris[j] = mgr->dyn_mtl_dlist.sort_pris[j + 1];
                }
                --mgr->dyn_mtl_dlist.nsort_pris;
            }

            // Compact material slots: shift left and re-place evicted tail.
            NUDISPLAYLIST *tail_dl = mgr->dyn_mtl_dlist.dlist_mtls[mgr->dyn_mtl_dlist.nmtls - 1];
            for (u32 j = static_cast<u32>(idx); j + 1 < mgr->dyn_mtl_dlist.nmtls; ++j) {
                mgr->dyn_mtl_dlist.mtls[j] = mgr->dyn_mtl_dlist.mtls[j + 1];
                mgr->dyn_mtl_dlist.dlist_mtls[j] = mgr->dyn_mtl_dlist.dlist_mtls[j + 1];
                mgr->material_used[j] = mgr->material_used[j + 1];
            }
            mgr->dyn_mtl_dlist.dlist_mtls[mgr->dyn_mtl_dlist.nmtls - 1] = tail_dl;
            mgr->material_used[mgr->dyn_mtl_dlist.nmtls - 1] = 0;

            ReassignMtlIds(mgr);
            RecomputeMtlFirst(mgr);
        }

        mtl->display_list = nullptr;
    }
    mgr->ndel_materials = 0;

    // ── Addition pass ──
    for (i32 ni = 0; ni < mgr->nnew_materials; ++ni) {
        NUMTL *mtl = mgr->new_materials[ni];
        if (mgr->dyn_mtl_dlist.nmtls <= 0)
            continue;

        // Already present? bump refcount.
        bool present = false;
        i32 idx = -1;
        if (mgr->dyn_mtl_dlist.mtls[0] == mtl) {
            present = true;
            idx = 0;
        } else {
            for (u32 j = 1; j < mgr->dyn_mtl_dlist.nmtls; ++j) {
                if (mgr->dyn_mtl_dlist.mtls[j] == mtl) {
                    present = true;
                    idx = static_cast<i32>(j);
                    break;
                }
            }
        }
        if (present) {
            ++mgr->material_used[idx];
            continue;
        }

        // Find free slot.
        i32 pos = -1;
        if (mgr->material_used[0] == 0)
            pos = 0;
        else {
            for (u32 j = 1; j < mgr->dyn_mtl_dlist.nmtls; ++j) {
                if (mgr->material_used[j] == 0) {
                    pos = static_cast<i32>(j);
                    break;
                }
            }
        }
        if (pos < 0)
            continue;

        // Find free stream buffer.
        i32 buf_off = -0x80;
        for (u32 j = 0; j < mgr->dyn_mtl_dlist.nmtls; ++j) {
            if (mgr->mtl_buffers_used[j] == 0) {
                mgr->mtl_buffers_used[j] = 1;
                buf_off = static_cast<i32>(j * 0x80);
                break;
            }
        }

        i32 key = MtlSortKey(mtl);
        i32 sp_idx = -1;
        if (mgr->dyn_mtl_dlist.nsort_pris > 0) {
            for (i32 j = 0; j < mgr->dyn_mtl_dlist.nsort_pris; ++j) {
                if (key == mgr->dyn_mtl_dlist.sort_pris[j].sort_pri)
                    sp_idx = j;
            }
        }

        // Insertion position — before first used material with key >= ours.
        i32 insert = pos;
        for (u32 j = 0; j < mgr->dyn_mtl_dlist.nmtls; ++j) {
            if (mgr->material_used[j] != 0 && key <= MtlSortKey(mgr->dyn_mtl_dlist.mtls[j])) {
                insert = static_cast<i32>(j);
                break;
            }
        }

        // Make room: shift [insert .. nmtls-2] right, tail goes to insert.
        NUDISPLAYLIST *old_tail = mgr->dyn_mtl_dlist.dlist_mtls[mgr->dyn_mtl_dlist.nmtls - 1];
        for (i32 j = static_cast<i32>(mgr->dyn_mtl_dlist.nmtls) - 2; j >= insert; --j) {
            mgr->dyn_mtl_dlist.mtls[j + 1] = mgr->dyn_mtl_dlist.mtls[j];
            mgr->dyn_mtl_dlist.dlist_mtls[j + 1] = mgr->dyn_mtl_dlist.dlist_mtls[j];
            mgr->material_used[j + 1] = mgr->material_used[j];
        }
        mgr->dyn_mtl_dlist.dlist_mtls[insert] = old_tail;
        mgr->material_used[insert] = 0;

        ReassignMtlIds(mgr);
        mgr->material_used[insert] = 1;

        NUDISPLAYLIST *dl = mgr->dyn_mtl_dlist.dlist_mtls[insert];
        mtl->display_list = dl;

        VARIPTR buf_ptr{reinterpret_cast<void *>(reinterpret_cast<u8 *>(mgr->mtlbuff.addr) + buf_off)};
        nudisplaylistitem_s *items = NuDisplayListCreateMtlDlist(dl->mtl_item, mtl, &buf_ptr, mgr->mtlbuffend);

        mgr->dyn_mtl_dlist.mtls[insert] = mtl;
        dl->dyn_geom = items + 6;
        dl->dlist = &mgr->dyn_mtl_dlist;
        dl->mtl_item = items;
        dl->mtl_id = insert;

        nusortpri_s *sp;
        if (sp_idx == -1) {
            sp = &mgr->dyn_mtl_dlist.sort_pris[mgr->dyn_mtl_dlist.nsort_pris];
            sp->items = items;
            sp->field_18 = mgr->field_4a8;
            sp->sort_pri = key;
            sp->nmtls = 0;

            if (!mgr->sort_list || mgr->sort_list->sort_pri >= key) {
                sp->sys_next = mgr->sort_list;
                mgr->sort_list = sp;
            } else {
                nusortpri_s *cur = mgr->sort_list;
                while (cur->sys_next && cur->sys_next->sort_pri < key)
                    cur = cur->sys_next;
                sp->sys_next = cur->sys_next;
                cur->sys_next = sp;
            }
            ++mgr->nused_sort_pris;
            ++mgr->dyn_mtl_dlist.nsort_pris;
            sp->nmtls = 1;
        } else {
            sp = &mgr->dyn_mtl_dlist.sort_pris[sp_idx];
            if (insert != 0 && MtlSortKey(mgr->dyn_mtl_dlist.mtls[insert - 1]) == key) {
                // Splice after previous material's block.
                auto *anchor = mgr->dyn_mtl_dlist.mtls[insert - 1]->display_list->mtl_item;
                auto *follow = static_cast<nudisplaylistitem_s *>(anchor[7].next);
                items[7].type = kItemType_Link;
                items[7].id = kItemId_Next;
                items[7].next = follow;
                anchor[7].next = items;
            } else {
                // Head of chain.
                items[7].type = kItemType_Link;
                items[7].id = kItemId_Next;
                items[7].next = sp->items;
                sp->items = items;
            }
            ++sp->nmtls;
        }

        ReassignMtlIds(mgr);
        RecomputeMtlFirst(mgr);
    }
    mgr->nnew_materials = 0;
}

// ──────────────────────────────────────────────────────────────────────────────
// End-of-frame
// ──────────────────────────────────────────────────────────────────────────────

extern "C" void NuDisplayListSwapBuffersEndFrame(void) {
    NUDLIST_MANAGER *mgr = &global_dlist_manager;

    NuThreadCriticalSectionBegin(mgr->loading_critical_section);
    DisplayListLinkDynamicMtls();
    DisplayListSwapBuffersPS();
    UpdateMaterialClipBits(&mgr->dyn_mtl_dlist);

    for (i32 i = 0; i < mgr->ndisplay_lists; ++i) {
        nudldlistscene_s *sc = mgr->dlists[i];
        u8 flags = sc->flags;

        if ((flags & 4) != 0) {
            // Clip-word diff: 2 bits per object, 16 objects per u32 word.
            u32 buf = static_cast<u32>(static_cast<u8>(sc->render_buffer >> 7));
            const u8 *cur_words = sc->clip_used[buf];
            const u8 *other_words = sc->clip_used[buf ^ 1];
            i32 nclip = sc->nclip_objects;
            i32 words = (nclip + 15) >> 4; // ceil(nclip/16)

            for (i32 w = 0; w < words; ++w) {
                const u8 *curw = cur_words + w * 4;
                if (*reinterpret_cast<const u32 *>(curw) == *reinterpret_cast<const u32 *>(other_words + w * 4)) {
                    continue;
                }
                for (i32 b = 0; b < 4; ++b) {
                    u8 byte_val = curw[b];
                    for (i32 shift = 0; shift < 8; shift += 2) {
                        i32 obj = w * 16 + b * 4 + shift / 2;
                        if (obj >= nclip)
                            break;
                        u32 code = (byte_val >> shift) & 3;
                        NUCLIPOBJECT *co = &sc->clip_objects[obj];
                        u16 cnt = sc->clip_counts[obj];
                        u8 new_id = (code == 0) ? kItemId_Cnt : kItemId_Call;
                        for (u16 j = 0; j < cnt; ++j) {
                            i32 idx = co->indices[j];
                            sc->items[idx].id = new_id;
                            sc->items[idx - 1].id = new_id;
                        }
                    }
                }
            }

            // Alpha fade — patch alpha into every 0x82 geom item.
            if (sc->alpha_values && nclip > 0) {
                for (i32 o = 0; o < nclip; ++o) {
                    u16 cnt = sc->clip_counts[o];
                    i32 *indices = sc->clip_objects[o].indices;
                    for (u16 j = 0; j < cnt; ++j) {
                        auto *item = &sc->items[indices[j]];
                        if (item->id != kItemId_Cnt && item->type == 0x82) {
                            DisplayListSetAlphaPS(item - 1, item, sc->alpha_values[o]);
                        }
                    }
                }
            }
            flags = sc->flags;
        }
        if ((flags & 6) != 0) {
            UpdateMaterialClipBits(sc);
        }
    }

    // Publish front → safe render scenes.
    for (i32 s = 0; s < 24; ++s) {
        mgr->safe_render_scenes[s] = mgr->front_render_scenes[s];
        mgr->front_render_scenes[s] = nullptr;
    }

    // Per-sortpri render-state walk.
    nurndrstate_s tmp_state{};
    tmp_state.mtl = nullptr;
    tmp_state.tex_id = -1;
    tmp_state.konst_id = -1;
    tmp_state.global_id = -1;
    tmp_state.lights_id = -1;
    tmp_state.camera_id = -1;
    tmp_state.fog_id = -1;
    tmp_state.reflection_id = -1;
    nuglobalrndrstate_s *last_local = nullptr;

    auto linkSceneGeometry = [](NUDISPLAYLIST *dl) {
        if (dl->scene_first[dl->scene_buffer] == dl->scene_next) {
            dl->dyn_geom->next = dl->dyn_geom + 1;
        } else {
            dl->dyn_geom->next = dl->scene_first[dl->scene_buffer];
            dl->scene_next->type = kItemType_Link;
            dl->scene_next->id = kItemId_Next;
            dl->scene_next->next = dl->dyn_geom + 1;
        }
    };

    auto copyMaterialGeometry = [&](NUDISPLAYLIST *dl, nudisplaylistitem_s *first, nudldlistscene_s *sc) {
        *dl->dyn_geom = *first;
        if ((sc->flags & 4) == 0) {
            dl->mtl_last->next = dl->mtl_item->next;
        } else if (dl->scene_first[dl->scene_buffer] != dl->scene_next) {
            dl->mtl_last->next = dl->scene_first[dl->scene_buffer];
            dl->scene_next->type = kItemType_Link;
            dl->scene_next->id = kItemId_Next;
            dl->scene_next->next = dl->dyn_geom + 1;
        }
        dl->mtl_last = first;
    };

    for (nusortpri_s *sp = mgr->sort_list; sp; sp = sp->sys_next) {
        nudldlistscene_s *sc = sp->display_scene;
        if (!sc) {
            RndrStateUpdateFx(&tmp_state, sp->items);
            continue;
        }
        if ((sc->flags & 6) == 0 || sp->nmtls == 0)
            continue;

        for (u32 m = 0; m < sp->nmtls; ++m) {
            i32 mi = static_cast<i32>(static_cast<u32>(sp->mtl_first) + m);
            NUDISPLAYLIST *dl = sc->dlist_mtls[mi];
            if (dl->mtl_item->id != kItemId_Cnt)
                continue;

            NUMTL *mtl = sc->mtls[mi];
            nuglobalrndrstate_s *ls = sc->local_state;

            if (!ls || (sc->flags & 4) == 0) {
                // No local state or no clipping — copy geometry if needed.
                if (dl->mtl_last != dl->first) {
                    copyMaterialGeometry(dl, dl->first, sc);
                } else {
                    linkSceneGeometry(dl);
                }
                last_local = nullptr;
            } else {
                bool same_as_last = (dl->mtl_last == dl->first && ls == last_local);
                if (!same_as_last) {
                    DisplayListUpdateRenderState(dl, ls);
                    last_local = ls;
                    if (dl->mtl_last != dl->first) {
                        copyMaterialGeometry(dl, dl->first, sc);
                    } else {
                        linkSceneGeometry(dl);
                    }
                } else {
                    linkSceneGeometry(dl);
                }
            }

            RndrStateUpdate(&tmp_state, mtl, dl->mtl_item);

            // Visibility cull — hide materials that fail the scene's flag test.
            u16 scene_flags = *reinterpret_cast<const u16 *>(&sc->flags);
            if ((scene_flags & 0x1fe0) != 0 &&
                ((static_cast<u8>(scene_flags >> 5) & static_cast<u8>(MTL_ATTRIB_DWORD1(mtl) >> 0xe)) != 0)) {
                dl->mtl_item->id = kItemId_Next;
                i32 mid = dl->mtl_id;
                i32 word = (mid >= 0) ? mid >> 3 : (mid + 7) >> 3;
                i32 bit = mid & 7;
                u8 *mu = sc->mtl_used[static_cast<u32>(static_cast<u8>(sc->render_buffer >> 7))];
                mu[word] &= static_cast<u8>((static_cast<u32>(-2) << bit) | (0xfffffffeu >> (32 - bit)));
            }
        }
    }

    for (i32 i = 0; i < mgr->ndisplay_lists; ++i) {
        nudldlistscene_s *sc = mgr->dlists[i];
        if (sc->local_state) {
            RndrStateResetGlobalState(sc->local_state);
        }
    }

    NuThreadCriticalSectionEnd(mgr->loading_critical_section);
}

extern "C" void NuDisplayListDraw2D(void) {
}
