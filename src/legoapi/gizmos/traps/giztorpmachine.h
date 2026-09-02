#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 giztorpmachine_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZTORPMACHINE_s {
    char name[0x10];
    char reserved_10[0x1b];
    u8 flags;
} GIZTORPMACHINE;

typedef struct GIZTORPSYS_s {
    GIZTORPMACHINE *machines;
    i32 count;
} GIZTORPSYS;

ADDGIZMOTYPE *GizTorpMachine_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
