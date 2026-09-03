#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"

extern i32 edGizShadow_gizmotype_id;

#ifdef __cplusplus

enum EDGIZSHADOW_PRESET {
    EDGIZSHADOW_PRESET_CUSTOM = 0,
    EDGIZSHADOW_PRESET_1 = 1,
    EDGIZSHADOW_PRESET_2 = 2,
};

typedef struct EDGIZSHADOW_s {
    NUVEC direction;
    f32 field_0x0c;
    f32 field_0x10;
    f32 field_0x14;
    f32 field_0x18;
    f32 field_0x1c;
    f32 field_0x20;
    f32 field_0x24;
    f32 field_0x28;
    f32 field_0x2c;
    f32 field_0x30;
    EDGIZSHADOW_PRESET preset;
} EDGIZSHADOW;

DECOMP_ASSERT(sizeof(EDGIZSHADOW) == 0x38, "EDGIZSHADOW ABI");

ADDGIZMOTYPE *EdGizShadowMachine_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
