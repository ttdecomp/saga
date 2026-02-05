#pragma once

#include "game/gizmo.h"

extern int hatmachine_gizmotype_id;

#ifdef __cplusplus

typedef struct HATMACHINE_s {
} HATMACHINE;

ADDGIZMOTYPE* HatMachine_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
