#pragma once

#include "legoapi/gizmo.h"

extern i32 hatmachine_gizmotype_id;

#ifdef __cplusplus

typedef struct HATMACHINE_s {
} HATMACHINE;

ADDGIZMOTYPE* HatMachine_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
