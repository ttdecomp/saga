#pragma once

#include "legoapi/gizmo.h"

extern i32 giztorpmachine_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZTORPMACHINE_s {
} GIZTORPMACHINE;

ADDGIZMOTYPE* GizTorpMachine_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
