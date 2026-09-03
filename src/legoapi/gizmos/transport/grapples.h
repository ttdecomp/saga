#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"

extern i32 grapple_gizmotype_id;

#ifdef __cplusplus

enum GRAPPLE_FLAGS {
    GRAPPLE_FLAG_ACTIVE = 1 << 0,
    GRAPPLE_FLAG_VISIBLE = 1 << 1,
    GRAPPLE_FLAG_DISABLED = 1 << 2,
    GRAPPLE_FLAG_REVERSED = 1 << 3,
};

typedef struct GRAPPLE_s {
    char name[16];           // 0x00
    NUVEC position;          // 0x10
    u8 has_terrain_platform; // 0x1c
    u8 retain_attachment;    // 0x1d
    u8 flags;                // 0x1e
    u8 field_0x1f;
    u16 y_rotation;              // 0x20
    u16 x_rotation;              // 0x22
    NUVEC ground_position;       // 0x24
    NUVEC hook_position;         // 0x30
    NUVEC shadow_probe_position; // 0x3c
    u16 ground_x_rotation;       // 0x48
    u16 ground_z_rotation;       // 0x4a
    f32 field_0x4c;
    f32 rope_length;                      // 0x50
    struct GameObject_s *attached_object; // 0x54
    f32 activation_progress;              // 0x58
    NUVEC rope_points[6];                 // 0x5c
    NUVEC target_rope_points[6];          // 0xa4
    void *display_specials[3];            // 0xec
} GRAPPLE;

DECOMP_ASSERT(sizeof(GRAPPLE) == 0xf8, "GRAPPLE ABI");
DECOMP_ASSERT(offsetof(GRAPPLE, has_terrain_platform) == 0x1c, "GRAPPLE platform-state offset");
DECOMP_ASSERT(offsetof(GRAPPLE, flags) == 0x1e, "GRAPPLE flags offset");
DECOMP_ASSERT(offsetof(GRAPPLE, ground_position) == 0x24, "GRAPPLE ground-position offset");
DECOMP_ASSERT(offsetof(GRAPPLE, hook_position) == 0x30, "GRAPPLE hook-position offset");
DECOMP_ASSERT(offsetof(GRAPPLE, attached_object) == 0x54, "GRAPPLE attached-object offset");
DECOMP_ASSERT(offsetof(GRAPPLE, activation_progress) == 0x58, "GRAPPLE activation progress offset");
DECOMP_ASSERT(offsetof(GRAPPLE, target_rope_points) == 0xa4, "GRAPPLE target-points offset");

ADDGIZMOTYPE *Grapples_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
