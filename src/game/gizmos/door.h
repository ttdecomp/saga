#pragma once

#include "game/gizmo.h"

extern char door_gizmotype_id;

#ifdef __cplusplus

typedef struct DOOR_s {
} DOOR;

ADDGIZMOTYPE* Door_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
