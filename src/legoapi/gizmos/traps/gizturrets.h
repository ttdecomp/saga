#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 turret_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZTURRET_s GIZTURRET;

enum GIZTURRET_FLAGS : u8 {
    GIZTURRET_FLAG_ACTIVE = 1 << 1,
    GIZTURRET_FLAG_VISIBLE = 1 << 2,
    GIZTURRET_FLAG_UPDATE_DISABLED = 1 << 5,
    GIZTURRET_FLAG_FIRED_THIS_FRAME = 1 << 7,
};

enum GIZTURRET_ANIMATION_FLAGS : u8 {
    GIZTURRET_ANIMATION_FLAG_DRAW_REFLECTION = 1 << 4,
};

enum GIZTURRET_RUNTIME_FLAGS : u8 {
    GIZTURRET_RUNTIME_FLAG_ROTATION_SFX_PLAYING = 1 << 3,
};

ADDGIZMOTYPE *GizTurrets_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
