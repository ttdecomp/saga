#pragma once

#include "game/gizmo.h"

extern int force_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZFORCE_s {
} GIZFORCE;

ADDGIZMOTYPE* GizForce_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
