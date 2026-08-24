#ifndef NU2API_NU3D_NUSPECIAL_H
#define NU2API_NU3D_NUSPECIAL_H

#include "decomp.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/nuvec.h"

// Named-scene-object ("special") query / manipulation API. Declarations live
// here (module nu2api/nu3d) so level and gameplay code includes one header
// instead of hand-declaring these C-linkage helpers.

extern "C" {
    i32 NuSpecialFind(NUGSCN *scene, void **dest, char *name, i32 flags);
    i32 NuSpecialExistsFn(void *special);
    void NuSpecialSetVisibility(void *special, i32 visible);
    void NuSpecialSetDrawMtx(void *special, void *mtx);
    void *NuSpecialGetDrawMtx(void *special);
    struct nuvec_s *NuSpecialGetPos(void *special);
    void NuSpecialDrawAt(void *special, void *mtx);
    void NuSpecialSetDrawPos(void *special, void *pos);
    i32 NuSpecialClipTestExtents(void *special, void *mtx);
    float NuSpecialGetOriginRadius(void *special);
}

#endif
