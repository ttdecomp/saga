#ifndef NU2API_NU3D_NUSPECIAL_H
#define NU2API_NU3D_NUSPECIAL_H

#include "decomp.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/numath/numtx.h"

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

struct NUDISPLAYSPECIAL {
    NUMTX instance_mtx; // 0x00
    NUMTX draw_mtx;     // 0x40
    NUVEC4 bounds_min;  // 0x80
    NUVEC4 bounds_max;  // 0x90
    u8 pad_a0[0x10];
    NUCLIPOBJECT *clip_objects; // 0xb0
    char *name;                 // 0xb4
    u32 flags;                  // 0xb8, NUDISPLAYSPECIAL_FLAGS
    f32 *clip_range;            // 0xbc
    i32 instance_ix;            // 0xc0
    union {
        nuinstanim_s *instance_animation; // 0xc4, animated specials point at a matrix-first record
        NUMTX *draw_mtx_ptr;              // 0xc4, direct draw override used by scene rendering
    };
    u8 pad_c8[8];
};
typedef NUDISPLAYSPECIAL NUDISPLAYSPECIAL_s;

DECOMP_ASSERT(sizeof(NUDISPLAYSPECIAL) == 0xd0, "display special size");
DECOMP_ASSERT(offsetof(NUDISPLAYSPECIAL, draw_mtx) == 0x40, "display special draw matrix offset");
DECOMP_ASSERT(offsetof(NUDISPLAYSPECIAL, flags) == 0xb8, "display special flags offset");

// Named-scene-object ("special") query / manipulation API. Declarations live
// here (module nu2api/nu3d) so level and gameplay code includes one header
// instead of hand-declaring these C-linkage helpers.

extern "C" {
    i32 NuSpecialFind(NUGSCN *scene, nuhspecial_s *dest, char *name, i32 flags);
    i32 NuSpecialFindMulti(NUGSCN *scene, nuhspecial_s *dest, char *name, i32 capacity, i32 flags);
    i32 NuSpecialCompare(nuhspecial_s *first, nuhspecial_s *second);
    i32 NuSpecialExistsFn(void *special);
    void NuSpecialClear(void *special);
    void NuSpecialGetBounds(void *special, NUVEC *minimum, NUVEC *maximum);
    void NuSpecialGetRadius(void *special, NUVEC *position, f32 *radius);
    f32 NuSpecialGetAnimEndFrame(nuhspecial_s *special);
    nuinstanim_s *NuSpecialGetInstAnim(nuhspecial_s *special);
    NUMTX *NuSpecialGetInstanceMtx(nuhspecial_s *special);
    i32 NuSpecialGetInstanceix(nuhspecial_s *special);
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
    void NuSpecialConstTint(i32 enabled, NUVEC *tint);
    float NuSpecialGetOriginRadius(void *special);
    i32 NuSpecialDrawAtAlpha(void *special, NUMTX *mtx, f32 alpha);
}

#endif
