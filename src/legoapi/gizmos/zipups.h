#pragma once

#include "legoapi/gizmo.h"

extern i32 zipup_gizmotype_id;

#ifdef __cplusplus

typedef struct ZIPUP_s {
} ZIPUP;

ADDGIZMOTYPE *ZipUps_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
