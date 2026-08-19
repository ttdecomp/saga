#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 force_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZFORCE_s GIZFORCE;

ADDGIZMOTYPE *GizForce_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
