#pragma once

#include "legoapi/gizmo.h"

extern i32 blowup_gizmotype_id;

#ifdef __cplusplus

typedef struct BLOWUP_s {
} BLOWUP;

ADDGIZMOTYPE *NewBlowup_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
