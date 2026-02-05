#pragma once

#include "game/gizmo.h"

extern int gizspecial_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZSPECIAL_s {
} GIZSPECIAL;

ADDGIZMOTYPE* GizSpecial_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
