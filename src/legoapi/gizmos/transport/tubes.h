#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus

struct GameObject_s;

enum TUBE_FLAGS {
    TUBE_FLAG_ACTIVE = 1 << 0,
    TUBE_FLAG_VISIBLE = 1 << 1,
    TUBE_FLAG_DIRECTIONAL = 1 << 2,
    TUBE_FLAG_REVERSED = 1 << 3,
    TUBE_FLAG_TOUCH_RADIUS = 1 << 4,
};

typedef struct TUBE_s {
    char name[16];  // 0x00
    NUVEC position; // 0x10
    f32 height;     // 0x1c
    f32 radius;     // 0x20
    f32 field_0x24;
    f32 top;            // 0x28
    f32 radius_squared; // 0x2c
    f32 audio_cooldown; // 0x30
    u8 flags;           // 0x34
    u8 reserved_0x35[3];
    u32 occupied_object_masks[2]; // 0x38, one bit per player/object slot
} TUBE;

DECOMP_ASSERT(sizeof(TUBE) == 0x40, "TUBE ABI");
DECOMP_ASSERT(offsetof(TUBE, flags) == 0x34, "TUBE flags offset");

i32 Tube_InCylinder(GameObject_s *object, TUBE *tube, f32 *horizontal_distance_squared, i32 ignore_height);
i32 Tube_IsObjBitSet(TUBE *tube, i32 object_index);
void Tube_SetObjBit(TUBE *tube, i32 object_index);

ADDGIZMOTYPE *Tubes_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
