#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 gizmopickup_typeid;
extern f32 AreaPickupScale;
extern i32 PickUpFlickerTest;
extern i32 PickUpFlickerFrames;
extern i32 PickupFlickerFrame;

#ifdef __cplusplus

typedef struct GIZMOPICKUP_s GIZMOPICKUP;

ADDGIZMOTYPE *GizmoPickups_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
