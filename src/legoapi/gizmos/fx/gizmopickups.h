#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 gizmopickup_typeid;

#ifdef __cplusplus

typedef struct GIZMOPICKUP_s GIZMOPICKUP;

ADDGIZMOTYPE *GizmoPickups_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
