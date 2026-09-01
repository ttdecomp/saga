#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"

extern i32 gizrandom_gizmotype_id;

#ifdef __cplusplus

enum GIZRANDOM_FLAGS : u8 {
    GIZRANDOM_FLAG_ACTIVE = 1 << 0,
};

typedef struct gizrandom_s {
    char name[16];
    i32 output_count;
    i32 output_weights[8];
    i32 selected_output;
    u8 flags;
    u8 reserved_0x39[3];
} GIZRANDOM;

typedef struct GIZRANDOMSYS_s {
    GIZRANDOM *randoms;
    i32 count;
    i32 capacity;
} GIZRANDOMSYS;

DECOMP_ASSERT(sizeof(GIZRANDOM) == 0x3c, "GIZRANDOM ABI");
DECOMP_ASSERT(offsetof(GIZRANDOM, output_count) == 0x10, "GIZRANDOM output count offset");
DECOMP_ASSERT(offsetof(GIZRANDOM, selected_output) == 0x34, "GIZRANDOM selected output offset");
DECOMP_ASSERT(offsetof(GIZRANDOM, flags) == 0x38, "GIZRANDOM flags offset");
DECOMP_ASSERT(sizeof(GIZRANDOMSYS) == 0xc, "GIZRANDOMSYS ABI");

ADDGIZMOTYPE *GizRandom_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
