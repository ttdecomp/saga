#pragma once

#include "game/gizmo.h"

extern int gizpanel_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZPANEL_s {
} GIZPANEL;

ADDGIZMOTYPE* GizPanel_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
