#pragma once

#include "game/gizmo.h"
#include "game/world.h"
#include "nu2api.saga/nu3d/nuportal.h"

extern int portal_gizmotype_id;

typedef struct gizportalprogress_s {
    u32 progress_mask;
} GIZPORTALPROGRESS;

#ifdef __cplusplus

ADDGIZMOTYPE *Portal_RegisterGizmo(int type_id);
char *Portal_GetOutputName(GIZMO *gizmo, int output_index);
void Portal_Activate(GIZMO *gizmo, int active);
void PortalDoors_Reset(WORLDINFO *world_info);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
