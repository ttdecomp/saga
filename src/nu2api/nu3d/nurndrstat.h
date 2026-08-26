#pragma once

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"

// Per-material render-state cache (original type `nurndrstate_s`, 52 bytes).
// Field offsets follow the Ghidra DB exactly; they are the targets of the
// reset block in NuDisplayListBeforeFrame @0x2a9ea0 (stores at +0x8, +0x14,
// +0x1e, +0x20, +0x24, +0x2c, +0x2e, +0x32).
typedef struct nurndrstate_s {
    i32 global_state_bottom; // 0x00 gs_bot
    i32 global_state_top;    // 0x04 gs_top
    NUMTL *mtl;              // 0x08
    u16 unused;              // 0x0c
    u8 *unknown_10;          // 0x10 mpg scratch pointer
    i32 tex_id;              // 0x14 tid (-1 == none)
    i32 global_state_ptr;    // 0x18 gs_ptr
    u16 texture_slots;       // 0x1c
    i16 konst_id;            // 0x1e (-1 == none)
    i32 global_id;           // 0x20 (-1 == none)
    i16 lights_id;           // 0x24 (-1 == none)
    i16 lights_type;         // 0x26
    i16 lights_specular_id;  // 0x28
    i16 lights_fx_id;        // 0x2a
    i16 camera_id;           // 0x2c (-1 == none)
    i16 fog_id;              // 0x2e (-1 == none)
    i16 vertex_groups_id;    // 0x30 (-1 == none)
    i16 reflection_id;       // 0x32 (-1 == none)
} NURNDRSTATE;

#ifdef __cplusplus
#if !defined(__x86_64__) // the tidy pre-pass parses as 64-bit host; real build is i686
static_assert(sizeof(NURNDRSTATE) == 52, "nurndrstate_s size");
static_assert(offsetof(NURNDRSTATE, mtl) == 0x08, "state.mtl");
static_assert(offsetof(NURNDRSTATE, tex_id) == 0x14, "state.tex_id");
static_assert(offsetof(NURNDRSTATE, konst_id) == 0x1e, "state.konst_id");
static_assert(offsetof(NURNDRSTATE, global_id) == 0x20, "state.global_id");
static_assert(offsetof(NURNDRSTATE, lights_id) == 0x24, "state.lights_id");
static_assert(offsetof(NURNDRSTATE, camera_id) == 0x2c, "state.camera_id");
static_assert(offsetof(NURNDRSTATE, fog_id) == 0x2e, "state.fog_id");
static_assert(offsetof(NURNDRSTATE, reflection_id) == 0x32, "state.reflection_id");
#endif
#endif
