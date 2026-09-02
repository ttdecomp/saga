#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 bombgen_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZBOMBGEN_s {
    char name[0x10];
    char reserved_10[0x1c];
} GIZBOMBGEN;

ADDGIZMOTYPE *GizBombGen_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
