#ifndef NU2API_NU3D_NUSPECIAL_H
#define NU2API_NU3D_NUSPECIAL_H

#include "decomp.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/numtx.h"

struct NUDISPLAYSPECIAL_s;

struct nuhspecial_s {
    NUGSCN *scene;
    void *special;
    NUDISPLAYSPECIAL_s *display_special;
};

DECOMP_ASSERT(sizeof(nuhspecial_s) == 0xc, "nuhspecial_s size");

// Named-scene-object ("special") query / manipulation API. Declarations live
// here (module nu2api/nu3d) so level and gameplay code includes one header
// instead of hand-declaring these C-linkage helpers.

extern "C" {
    i32 NuSpecialFind(NUGSCN *scene, nuhspecial_s *dest, char *name, i32 flags);
    i32 NuSpecialFindMulti(NUGSCN *scene, nuhspecial_s *dest, char *name, i32 max_results, i32 flags);
    i32 NuSpecialCompare(void *filter, nuhspecial_s *special);
    char *NuSpecialGetName(nuhspecial_s *special);
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
    void *NuSpecialGetInstanceix(void *special);
    i32 NuSpecialDrawAtAlpha(void *special, NUMTX *mtx, f32 alpha);
}

#endif
