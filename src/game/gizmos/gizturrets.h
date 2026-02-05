#pragma once

#include "game/gizmo.h"

extern int turret_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZTURRET_s {
} GIZTURRET;

ADDGIZMOTYPE* GizTurrets_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
