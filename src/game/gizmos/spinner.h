#pragma once

#include "game/gizmo.h"

extern int spinner_gizmotype_id;

#ifdef __cplusplus

typedef struct SPINNER_s {
} SPINNER;

ADDGIZMOTYPE* Spinner_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
