#pragma once

#include "legoapi/gizmo.h"

extern i32 teleport_gizmotype_id;

#ifdef __cplusplus

typedef struct TELEPORT_s TELEPORT;

ADDGIZMOTYPE *Teleport_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
