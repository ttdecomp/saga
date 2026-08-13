#pragma once

#include "legoapi/gizmo.h"

extern i32 plug_gizmotype_id;

#ifdef __cplusplus

typedef struct PLUG_s PLUG;

ADDGIZMOTYPE *Plugs_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
