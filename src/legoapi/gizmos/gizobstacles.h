#pragma once

#include "legoapi/gizmo.h"

extern i32 obstacle_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZOBSTACLE_s {
} GIZOBSTACLE;

ADDGIZMOTYPE *GizObstacles_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
