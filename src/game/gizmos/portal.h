#pragma once

#include "game/gizmo.h"

extern int portal_gizmotype_id;

#ifdef __cplusplus

typedef struct nuportal_s {
    u8 filler[0x1a];
    i8 id;
    u8 unknown;
    u32 active;
} NUPORTAL;

ADDGIZMOTYPE* Portal_RegisterGizmo(int type_id);
char *Portal_GetOutputName(GIZMO *gizmo, int output_index);
void Portal_Activate(GIZMO *gizmo, int active);

extern "C" {
#endif

void NuPortalSetActiveDirect(NUPORTAL* portal, int active);

#ifdef __cplusplus
}
#endif
