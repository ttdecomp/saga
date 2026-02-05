#pragma once

#include "game/gizmo.h"

extern int portal_gizmotype_id;

#ifdef __cplusplus

typedef struct PORTAL_s {
} PORTAL;

ADDGIZMOTYPE* Portal_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
