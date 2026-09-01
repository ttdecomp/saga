#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"

extern i32 gizrandom_gizmotype_id;

#ifdef __cplusplus

typedef struct gizrandom_s {
    char name[16];
    i32 output_count;
    i32 output_chances[8];
    i32 selected_output;
    u8 flags;
    u8 pad_39[3];
} GIZRANDOM;
DECOMP_ASSERT(sizeof(GIZRANDOM) == 0x3c, "GIZRANDOM size");

ADDGIZMOTYPE *GizRandom_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
