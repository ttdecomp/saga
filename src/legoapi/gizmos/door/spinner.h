#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 spinner_gizmotype_id;

#ifdef __cplusplus

typedef struct SPINNER_s {
    char unknown_00[0x40];
    char name[0x40];
    char unknown_080[0x284];
} SPINNER;

ADDGIZMOTYPE *Spinner_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
