#pragma once

#include "legoapi/gizmo.h"

extern i32 gizbuildit_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZBUILDIT_s GIZBUILDIT;

ADDGIZMOTYPE *GizBuildIts_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
