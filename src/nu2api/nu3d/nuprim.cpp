#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"

char g_NuPrim_NeedsHalfUVs;
char g_NuPrim_NeedsOverbrightening;

i32 NuPrimCSPos;
NUPRIMSCALEMODE NuPrimCoordSystemStack[16];
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
    VARIPTR *buf;

    if (mtl == NULL) {
        mtl = numtl_defaultmtl3d;
    }

    g_NuPrim_NeedsOverbrightening = mtl->tex_id != 0;
    g_NuPrim_NeedsHalfUVs = mtl->shader_desc.vtx_desc.has_half_uvs;

    buf = NuDisplayListGetBuffer();
}
