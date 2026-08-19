#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 gizrandom_gizmotype_id;

#ifdef __cplusplus

typedef struct gizrandom_s {
    u8 filler[0x3c];
} GIZRANDOM;

ADDGIZMOTYPE *GizRandom_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
