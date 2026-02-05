#pragma once

#include "game/gizmo.h"

extern int gizmopickup_typeid;

#ifdef __cplusplus

typedef struct GIZMOPICKUP_s {
} GIZMOPICKUP;

ADDGIZMOTYPE* GizmoPickups_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
