#pragma once

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/world/world.h"

extern i32 portal_gizmotype_id;

typedef struct gizportalprogress_s {
    u32 progress_mask;
} GIZPORTALPROGRESS;

#ifdef __cplusplus

ADDGIZMOTYPE *Portal_RegisterGizmo(i32 type_id);
char *Portal_GetOutputName(GIZMO *gizmo, i32 output_index);
void Portal_Activate(GIZMO *gizmo, i32 active);
void PortalDoors_Reset(WORLDINFO *world_info);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
