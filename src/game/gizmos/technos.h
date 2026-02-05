#pragma once

#include "game/gizmo.h"

extern int techno_gizmotype_id;

#ifdef __cplusplus

typedef struct TECHNO_s {
} TECHNO;

ADDGIZMOTYPE* Technos_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
