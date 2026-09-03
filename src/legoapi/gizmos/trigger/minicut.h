#pragma once

#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus

typedef struct MINICUT_s {
    char name[0x10];
    char reserved_10[0x20];
} MINICUT;

ADDGIZMOTYPE *MiniCut_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
