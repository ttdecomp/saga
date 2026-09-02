#pragma once

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/legoapi_types.h"

typedef enum portaldoorflags_e {
    PORTALDOOR_TRIGGER_AT_END = 1 << 0,
    PORTALDOOR_ACTIVE = 1 << 1,
    PORTALDOOR_INACTIVE = 1 << 2,
} PORTALDOORFLAGS;

typedef struct PORTALDOOR_s {
    nuhspecial_s special;
    u16 flags;
    u8 portal_id;
    u8 pad_0f;
} PORTALDOOR;

DECOMP_ASSERT(sizeof(PORTALDOOR) == 0x10, "PORTALDOOR size");
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
