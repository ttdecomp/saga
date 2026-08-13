#pragma once

#include "legoapi/gizmo.h"

extern i32 lever_gizmotype_id;

#ifdef __cplusplus

typedef struct LEVER_s {
} LEVER;

ADDGIZMOTYPE *Levers_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
