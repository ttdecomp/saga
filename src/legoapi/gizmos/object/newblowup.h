#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 blowup_gizmotype_id;

#ifdef __cplusplus

typedef struct BLOWUP_s {
    u8 reserved_00[0xfa];
    char name[0x10];
} BLOWUP;

ADDGIZMOTYPE *NewBlowup_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
