#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 techno_gizmotype_id;

#ifdef __cplusplus

typedef struct TECHNO_s TECHNO;

ADDGIZMOTYPE *Technos_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
