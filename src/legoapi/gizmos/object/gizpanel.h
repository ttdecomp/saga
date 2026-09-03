#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 gizpanel_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZPANEL_s GIZPANEL;

ADDGIZMOTYPE *GizPanel_RegisterGizmo(i32 type_id);
void GizPanel_Reset(GIZPANEL *panel);
void GizPanel_GetAbsTargetPos(GIZPANEL *panel, NUVEC *position, i32 target_index);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
