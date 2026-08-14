#pragma once

#include "legoapi/gizmo.h"

extern i32 gizaimessage_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZAIMESSAGE_s GIZAIMESSAGE;

ADDGIZMOTYPE *GizAIMessage_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
