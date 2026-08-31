#ifndef NU2API_NU3D_NUSPECIAL_H
#define NU2API_NU3D_NUSPECIAL_H

#include "decomp.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/numtx.h"

struct nuhspecial_s;
struct nuinstanim_s;

enum NULEGACYINSTANCE_FLAGS : u8 {
    NULEGACYINSTANCE_FLAG_VISIBLE = 1 << 0,
    NULEGACYINSTANCE_FLAG_NO_VISIBILITY_TEST = 1 << 3,
};

enum NULEGACYSPECIAL_FLAGS : u32 {
    NULEGACYSPECIAL_FLAG_COLLISION = 1 << 9,
};

enum NUDISPLAYSPECIAL_FLAGS : u32 {
    NUDISPLAYSPECIAL_FLAG_VISIBLE = 1 << 1,
    NUDISPLAYSPECIAL_FLAG_NO_VISIBILITY_TEST = 1 << 7,
    NUDISPLAYSPECIAL_FLAG_COLLISION = 1 << 9,
    NUDISPLAYSPECIAL_FLAG_MATRIX_UPDATED = 1 << 10,
};

// Named-scene-object ("special") query / manipulation API. Declarations live
// here (module nu2api/nu3d) so level and gameplay code includes one header
// instead of hand-declaring these C-linkage helpers.

extern "C" {
    i32 NuSpecialFind(NUGSCN *scene, void **dest, char *name, i32 flags);
    i32 NuSpecialExistsFn(void *special);
    void NuSpecialClear(void *special);
    void NuSpecialGetBounds(void *special, NUVEC *minimum, NUVEC *maximum);
    void NuSpecialGetRadius(void *special, NUVEC *position, f32 *radius);
    f32 NuSpecialGetAnimEndFrame(nuhspecial_s *special);
    nuinstanim_s *NuSpecialGetInstAnim(nuhspecial_s *special);
    NUMTX *NuSpecialGetInstanceMtx(nuhspecial_s *special);
    char *NuSpecialGetName(nuhspecial_s *special);
    void NuSpecialUpdate(nuhspecial_s *special);
    void NuSpecialSetVisibility(void *special, i32 visible);
    void NuSpecialSetDrawMtx(void *special, NUMTX *mtx);
    NUMTX *NuSpecialGetDrawMtx(void *special);
    NUMTX *NuSpecialGetMtx(void *special);
    NUVEC *NuSpecialGetDrawPos(void *special);
    struct nuvec_s *NuSpecialGetPos(void *special);
    i32 NuSpecialDrawAt(void *special, NUMTX *mtx);
    i32 NuSpecialGetVisibilityFn(void *special);
    i32 NuSpecialGetNoVisiTestFn(nuhspecial_s *special);
    void NuSpecialSetNoVisiTest(nuhspecial_s *special, i32 enabled);
    void NuSpecialSetDrawPos(void *special, void *pos);
    i32 NuSpecialClipTestExtents(void *special, void *mtx);
    i32 NuSpecialSetClipping(i32 enabled, i32 state);
    float NuSpecialGetOriginRadius(void *special);
    i32 NuSpecialDrawAtAlpha(void *special, NUMTX *mtx, f32 alpha);
}

#endif
