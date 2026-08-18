#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 edGizShadow_gizmotype_id;

#ifdef __cplusplus

typedef struct EDGIZSHADOW_s {
} EDGIZSHADOW;

ADDGIZMOTYPE *EdGizShadowMachine_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
