#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"

extern i32 gizspecial_gizmotype_id;

#ifdef __cplusplus

struct GAMEANIMSET_s;

struct GIZSPECIAL_s {
    char name[32];
    GAMEANIMSET_s *animation_set;
    u8 flags;
    u8 pad_25[3];
};
typedef struct GIZSPECIAL_s GIZSPECIAL;

DECOMP_ASSERT(sizeof(GIZSPECIAL_s) == 0x28, "GIZSPECIAL size");

ADDGIZMOTYPE *GizSpecial_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
