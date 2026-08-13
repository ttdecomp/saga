#pragma once

#include "legoapi/gizmo.h"

extern i32 turret_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZTURRET_s GIZTURRET;

ADDGIZMOTYPE *GizTurrets_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
