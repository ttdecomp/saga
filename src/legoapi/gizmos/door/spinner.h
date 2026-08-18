#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 spinner_gizmotype_id;

#ifdef __cplusplus

typedef struct SPINNER_s {
} SPINNER;

ADDGIZMOTYPE *Spinner_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
