#ifndef NU2API_NU3D_NUSPECIAL_H
#define NU2API_NU3D_NUSPECIAL_H

#include "decomp.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/numtx.h"

struct nuhspecial_s;

// Named-scene-object ("special") query / manipulation API. Declarations live
// here (module nu2api/nu3d) so level and gameplay code includes one header
// instead of hand-declaring these C-linkage helpers.

extern "C" {
    i32 NuSpecialFind(NUGSCN *scene, void **dest, char *name, i32 flags);
    i32 NuSpecialExistsFn(void *special);
    void NuSpecialSetVisibility(void *special, i32 visible);
    void NuSpecialSetDrawMtx(void *special, NUMTX *mtx);
    NUMTX *NuSpecialGetDrawMtx(void *special);
    struct nuvec_s *NuSpecialGetPos(void *special);
    i32 NuSpecialDrawAt(void *special, NUMTX *mtx);
    i32 NuSpecialGetVisibilityFn(void *special);
    void NuSpecialSetDrawPos(void *special, void *pos);
    i32 NuSpecialClipTestExtents(void *special, void *mtx);
    i32 NuSpecialSetClipping(i32 enabled, i32 state);
    float NuSpecialGetOriginRadius(void *special);
    i32 NuSpecialDrawAtAlpha(void *special, NUMTX *mtx, f32 alpha);
}

#endif
