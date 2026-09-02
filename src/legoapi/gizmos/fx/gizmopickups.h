#pragma once

#include "decomp.h"
#include "legoapi/gizmo/base/gizmo.h"

struct GIZMOPICKUPSYS_s;

typedef void (*GIZMOPICKUPCALLBACK)(void);

struct GIZMOPICKUPTYPE_s {
    const char *name;
    const char *sound_name;
    const char *effect_name;
    u32 field_0x0c;
    u32 field_0x10;
    f32 field_0x14;
    f32 field_0x18;
    f32 scale;
    f32 field_0x20;
    GIZMOPICKUPCALLBACK callback_0x24;
    GIZMOPICKUPCALLBACK callback_0x28;
    i16 debris_type;
    i16 field_0x2e;
    void *field_0x30;
    void *field_0x34;
};
DECOMP_ASSERT(sizeof(GIZMOPICKUPTYPE_s) == 0x38, "GIZMOPICKUPTYPE size");

extern i32 gizmopickup_typeid;
extern GIZMOPICKUPSYS_s GizmoPickupSys_Game;
extern GIZMOPICKUPTYPE_s GizmoPickupType[10];
extern u8 CoinTab[4];

#ifdef __cplusplus

typedef struct GIZMOPICKUP_s GIZMOPICKUP;

ADDGIZMOTYPE *GizmoPickups_RegisterGizmo(i32 type_id);
void GizmoPickups_InitSys(GIZMOPICKUPSYS_s *pickup_sys);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
