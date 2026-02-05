#pragma once

#include "game/gizmo.h"

extern int grapple_gizmotype_id;

#ifdef __cplusplus

typedef struct GRAPPLE_s {
} GRAPPLE;

ADDGIZMOTYPE* Grapples_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
