#pragma once

#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus

typedef struct ATTRACTO_s {
    char name[0x10];
    char reserved_10[0x64];
} ATTRACTO;

ADDGIZMOTYPE *Attractos_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
