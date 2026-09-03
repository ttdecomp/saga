#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 force_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZFORCE_s GIZFORCE;

void GizForce_PlayForwards(GIZFORCE_s *force);
void GizForce_PlayBackwards(GIZFORCE_s *force);
void GizForce_SetVisibility(GIZFORCE_s *force, i32 visibility);
i32 GizForce_AnimComplete(GIZFORCE_s *force);
i32 GizForce_Complete(GIZFORCE_s *force);

ADDGIZMOTYPE *GizForce_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
