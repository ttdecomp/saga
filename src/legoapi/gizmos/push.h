#pragma once

#include "legoapi/gizmo.h"

extern i32 pushblock_gizmotype_id;

#ifdef __cplusplus

typedef struct PUSHBLOCK_s {
} PUSHBLOCK;

ADDGIZMOTYPE* Push_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
