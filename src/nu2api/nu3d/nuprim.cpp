#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndrstat.h"

extern "C" {
    void *DisplayListCreateGeomTransformPS(VARIPTR *buffer, NUMTX *transform, NUMTL *mtl, void *next, void *tx);
}

char g_NuPrim_NeedsHalfUVs;
char g_NuPrim_NeedsOverbrightening;

// Points at the VARIPTR cursor of the current display-list vertex buffer
// (original bss @0x99b540; set by NuPrim2DBegin/NuPrim3DBegin).
VARIPTR *g_NuPrim_StreamBufferPtr;

i32 NuPrimCSPos;
NUPRIMSCALEMODE NuPrimCoordSystemStack[16];

static NUDISPLAYLISTITEM *NuDisplayListAddItem(NUDISPLAYLIST *list, u8 type, void *next) {
    NUDISPLAYLISTITEM *item = list->items;
    item->type = type;
    item->id = 3;
    item->next = next;
    list->items = reinterpret_cast<NUDISPLAYLISTITEM *>(reinterpret_cast<u8 *>(list->items) + 0x10);
    return reinterpret_cast<NUDISPLAYLISTITEM *>(reinterpret_cast<u8 *>(list->items) - 0x10);
}
f32 NuPrim_XScale = 1.0f;
f32 NuPrim_YScale = 1.0f;
f32 NuPrim_XBias;
f32 NuPrim_YBias;

extern "C" {
    // It's not clear how this function is declared. It has local linkage and no
    // C++ mangling, but appears to be part of the same translation unit as
    // `NuPrim3DBegin`, which references functions (presumably drawn from the
    // same header) with local linkage and a mix of mangling.
    static void NuPrimPushCoordSystem(NUPRIMSCALEMODE scale_mode) {
        NuPrimCSPos++;

        NuPrimSetCoordinateSystem(scale_mode);
    }
}

void NuPrimInit(VARIPTR *buffer, VARIPTR buffer_end) {
    NuPrimCSPos = -1;

    NuPrimPushCoordSystem(NUPRIM_SCALEMODE_PS2);
}

void NuPrimSetCoordinateSystem(NUPRIMSCALEMODE scale_mode) {
    NuPrimCoordSystemStack[NuPrimCSPos] = scale_mode;

    switch (scale_mode) {
        case NUPRIM_SCALEMODE_PS2:
            NuPrim_XScale = 0.003125f;
            NuPrim_YScale = -0.008928572f;
            NuPrim_XBias = -1.0f;
            NuPrim_YBias = 1.0f;
            break;
        case NUPRIM_SCALEMODE_NORMALISED:
            NuPrim_XScale = 1.0f;
            NuPrim_YScale = -1.0f;
            NuPrim_XBias = 0.0f;
            NuPrim_YBias = 0.0f;
            break;
        case NUPRIM_SCALEMODE_ABSOLUTE:
            NuPrim_XScale = 2.0f;
            NuPrim_YScale = -2.0f;
            NuPrim_XBias = -1.0f;
            NuPrim_YBias = 1.0f;
            break;
    }
}

void NuPrim3DBegin(u32 prim_type, u32 vtx_fmt, NUMTL *mtl, NUMTX *world_mtx) {
    if (mtl == NULL) {
        mtl = numtl_defaultmtl3d;
    }

    g_NuPrim_NeedsOverbrightening = mtl->tex_id != 0;
    g_NuPrim_NeedsHalfUVs = mtl->shader_desc.vtx_desc.has_half_uvs;

    VARIPTR *buf = NuDisplayListGetBuffer();
    NUDISPLAYLIST *list;
    if (mtl->display_list != NULL) {
        list = mtl->display_list;
        NUDLDLISTSCENE *scene = list->dlist;
        scene->flags |= NUDL_SCENE_FLAG_CLIP_MATERIALS;
        u8 *used = scene->mtl_used[(scene->render_buffer >> 7) & 1];
        used[list->mtl_id >> 3] |= static_cast<u8>(1 << (list->mtl_id & 7));
    } else {
        list = numtl_defaultmtl3d->display_list;
    }

    DisplayListUpdateRenderState(list, &render_state);
    NuDisplayListLinkItems(list, 2);
    g_NuPrim_StreamBufferPtr = buf;

    NUMTX *transform = static_cast<NUMTX *>(
        DisplayListCreateGeomTransformPS(buf, world_mtx != NULL ? world_mtx : &numtx_identity, NULL, NULL, NULL));

    NUDISPLAYLISTGEOM *geometry = reinterpret_cast<NUDISPLAYLISTGEOM *>(buf->addr);
    geometry->primitive_type = static_cast<i32>(prim_type);
    geometry->vertex_count = 0;
    buf->addr += sizeof(NUDISPLAYLISTGEOM);

    g_NuPrim_PendingVertexCount = reinterpret_cast<u16 *>(&geometry->vertex_count);
    g_NuPrim_ActivePrimType = static_cast<u16>(prim_type);

    NuDisplayListAddItem(list, 0x8c, transform);
    NuDisplayListAddItem(list, 0x82, geometry);

    (void)vtx_fmt;
}
