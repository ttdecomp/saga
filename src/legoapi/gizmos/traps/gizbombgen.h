#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 bombgen_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZBOMBGEN_s {
} GIZBOMBGEN;

ADDGIZMOTYPE *GizBombGen_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
