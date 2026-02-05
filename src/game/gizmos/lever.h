#pragma once

#include "game/gizmo.h"

extern int lever_gizmotype_id;

#ifdef __cplusplus

typedef struct LEVER_s {
} LEVER;

ADDGIZMOTYPE* Levers_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
