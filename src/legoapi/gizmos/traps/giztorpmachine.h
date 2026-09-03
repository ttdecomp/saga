#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"

extern i32 giztorpmachine_gizmotype_id;

#ifdef __cplusplus

enum GIZTORPMACHINE_FLAGS : u8 {
    GIZTORPMACHINE_FLAG_ACTIVE = 1 << 0,
    GIZTORPMACHINE_FLAG_VISIBLE = 1 << 1,
    GIZTORPMACHINE_FLAG_ALTERNATE_MODEL = 1 << 2,
};

typedef struct GIZTORPMACHINE_s {
    char name[16];       // 0x00
    NUVEC position;      // 0x10
    f32 activation_time; // 0x1c
    u16 y_rotation;      // 0x20
    u8 reserved_0x22[9];
    u8 flags; // 0x2b
} GIZTORPMACHINE;

DECOMP_ASSERT(sizeof(GIZTORPMACHINE) == 0x2c, "GIZTORPMACHINE ABI");
DECOMP_ASSERT(offsetof(GIZTORPMACHINE, activation_time) == 0x1c, "GIZTORPMACHINE activation-time offset");
DECOMP_ASSERT(offsetof(GIZTORPMACHINE, flags) == 0x2b, "GIZTORPMACHINE flags offset");

typedef struct GIZTORPMACHINESYS_s {
    GIZTORPMACHINE *machines; // 0x00
    i32 count;                // 0x04
    f32 scale;                // 0x08
} GIZTORPMACHINESYS;

DECOMP_ASSERT(sizeof(GIZTORPMACHINESYS) == 0x0c, "GIZTORPMACHINESYS ABI");

ADDGIZMOTYPE *GizTorpMachine_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
