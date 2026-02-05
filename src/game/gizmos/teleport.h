#pragma once

#include "game/gizmo.h"

extern int teleport_gizmotype_id;

#ifdef __cplusplus

typedef struct TELEPORT_s {
} TELEPORT;

ADDGIZMOTYPE* Teleport_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
