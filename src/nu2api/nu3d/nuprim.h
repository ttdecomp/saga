#pragma once

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numath/numtx.h"

typedef enum {
    NUPRIM_SCALEMODE_PS2 = 0,
    NUPRIM_SCALEMODE_NORMALISED = 1,
    NUPRIM_SCALEMODE_ABSOLUTE = 2
} NUPRIMSCALEMODE;

#ifdef __cplusplus
extern "C" {
#endif
    void NuPrimInit(VARIPTR *buffer, VARIPTR buffer_end);

    void NuPrim3DBegin(u32 prim_type, u32 vtx_fmt, NUMTL *mtl, NUMTX *world_mtx);

    void NuPrimSetCoordinateSystem(NUPRIMSCALEMODE scale_mode);
#ifdef __cplusplus
}
#endif
