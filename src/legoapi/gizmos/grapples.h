#pragma once

#include "legoapi/gizmo.h"

extern i32 grapple_gizmotype_id;

#ifdef __cplusplus

typedef struct GRAPPLE_s {
} GRAPPLE;

ADDGIZMOTYPE* Grapples_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
