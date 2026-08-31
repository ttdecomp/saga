#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 gizspecial_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZSPECIAL_s GIZSPECIAL;

char *GizSpecial_GetName(GIZSPECIAL *special);
i32 GizmoGetGizmosUsingSpecial(GIZMOSYS *gizmo_sys, void *world, GIZMO **result, i32 result_capacity, char *name);
ADDGIZMOTYPE *GizSpecial_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
