#pragma once

#include "legoapi/gizmo.h"

extern i32 gizspecial_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZSPECIAL_s {
} GIZSPECIAL;

ADDGIZMOTYPE *GizSpecial_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
