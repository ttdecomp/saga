#pragma once

#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus

typedef struct SECURITYDOOR_s {
    char unknown_00[0x80];
    char name[0x40];
} SECURITYDOOR;

ADDGIZMOTYPE *SecurityDoors_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
