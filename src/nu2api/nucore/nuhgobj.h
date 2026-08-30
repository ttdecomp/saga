#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/numtx.h"
#include "decomp_assert.h"

struct nudldlistscene_s;
struct nugscn_s;
struct ani3_animheader_s;
struct NUJOINTANIM_s;
using NUHGOBJROOTFN = void (*)(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);

struct nuhgobjpoi_s {
    u8 data[0x50];
};

struct nuhgobjjoint_s {
    u8 data_0x00[0x50];
    u8 parent_index;
    u8 data_0x51[0xf];
};

struct nuhgobjjointoverride_s {
    f32 rotation_x;
    f32 rotation_y;
    f32 rotation_z;
    NUVEC translation;
    u8 data_0x18[0x18];
    u8 joint_index;
    u8 data_0x31[3];
};

struct nuhgobjrender_s {
    u8 *visibility;
    void **rigid_specials;
    void *smooth_skin_special;
    void **alternate_rigid_specials;
    void *alternate_smooth_skin_special;
};

struct nuhgobj_s {
    u8 data[0x110];
    nudldlistscene_s *display_list; // 0x110
    u8 data_0x114[0x54];
    i32 joint_count;              // 0x168
    nuhgobjjoint_s *joints;       // 0x16c
    NUMTX *bind_matrices;         // 0x170
    NUMTX *inverse_bind_matrices; // 0x174
    i32 joint_override_map_count; // 0x178
    u8 *joint_override_map;       // 0x17c
    u8 data_0x180[4];
    nuhgobjpoi_s *points_of_interest; // 0x184
    i32 point_of_interest_count;      // 0x188
    u8 *point_of_interest_map;        // 0x18c
    i32 render_count;                 // 0x190
    nuhgobjrender_s *render_parts;    // 0x194
    u8 data_0x198[0x14];
    NUVEC bounds_min; // 0x1ac
    NUVEC bounds_max; // 0x1b8
};

DECOMP_ASSERT(sizeof(nuhgobjpoi_s) == 0x50, "nuhgobjpoi_s size");
DECOMP_ASSERT(sizeof(nuhgobjjoint_s) == 0x60, "nuhgobjjoint_s size");
DECOMP_ASSERT(sizeof(nuhgobjjointoverride_s) == 0x34, "nuhgobjjointoverride_s size");
DECOMP_ASSERT(sizeof(nuhgobjrender_s) == 0x14, "nuhgobjrender_s size");
DECOMP_ASSERT(sizeof(nuhgobj_s) == 0x1c4, "nuhgobj_s size");

// NuHGobj system (module nu2api/nucore, nucore_plain.cpp).

#ifdef __cplusplus
extern "C" {
#endif
    void NuHGobjReversibleCharacters(i32 v);
    void NuHGobjForceShadowsOnCharacters(i32 v);
    nuhgobj_s *NuGHGRead(char *path, VARIPTR *buf, VARIPTR buf_end);
    nuhgobjpoi_s *NuHGobjGetPOI(nuhgobj_s *object, i32 index);
    void NuHGobjEval(nuhgobj_s *object, i32 override_count, nuhgobjjointoverride_s *overrides, NUMTX *matrices);
    void NuHGobjEvalAnim2(nuhgobj_s *object, ani3_animheader_s *animation, f32 time, i32 override_count,
                          NUJOINTANIM_s *overrides, NUMTX *matrices);
    void NuHGobjEvalAnim2Root(nuhgobj_s *object, ani3_animheader_s *animation, f32 time, i32 override_count,
                              NUJOINTANIM_s *overrides, NUMTX *matrices, NUHGOBJROOTFN root_fn, void *root_data);
    void NuHGobjEvalAnim2Root_3(nuhgobj_s *object, ani3_animheader_s *animation, f32 time, i32 override_count,
                                NUJOINTANIM_s *overrides, NUMTX *matrices, NUHGOBJROOTFN root_fn, void *root_data);
    void NuHGobjEvalAnimBlend2(nuhgobj_s *object, ani3_animheader_s *animation_a, f32 time_a,
                               ani3_animheader_s *animation_b, f32 time_b, f32 blend, i32 override_count,
                               NUJOINTANIM_s *overrides, NUMTX *matrices);
    void NuHGobjEvalAnimBlend2Root(nuhgobj_s *object, ani3_animheader_s *animation_a, f32 time_a,
                                   ani3_animheader_s *animation_b, f32 time_b, f32 blend, i32 override_count,
                                   NUJOINTANIM_s *overrides, NUMTX *matrices, NUHGOBJROOTFN root_fn, void *root_data);
    i32 NuHGobjRndrMtxDwa(nuhgobj_s *object, NUMTX *world_matrix, i32 render_count, i16 *render_indices,
                          NUMTX *joint_matrices, void **blend_values, i32 render_flags);
#ifdef __cplusplus
}

void NuHGobjEvalAnimBlend2Root_3(nugscn_s *object, ani3_animheader_s *animation_a, f32 time_a,
                                 ani3_animheader_s *animation_b, f32 time_b, f32 blend, i32 override_count,
                                 NUJOINTANIM_s *overrides, NUMTX *matrices, NUHGOBJROOTFN root_fn, void *root_data);
#endif
