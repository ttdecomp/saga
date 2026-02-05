#pragma once

#include "game/gizmo.h"

extern int giztorpmachine_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZTORPMACHINE_s {
} GIZTORPMACHINE;

ADDGIZMOTYPE* GizTorpMachine_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
