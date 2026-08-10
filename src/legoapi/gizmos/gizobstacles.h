#pragma once

#include "legoapi/gameanim.h"
#include "legoapi/gizmo.h"

extern int obstacle_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZOBSTACLE_s {
    u8 unknown_000[0x34];
    GAMEANIMSET *anim_set;
    u8 unknown_038[0x60];
    u8 flags_098;
    u8 flags_099;
    u8 unknown_09a[2];
    i32 unknown_09c;
    u8 flags_0a0;
    u8 unknown_0a1[3];
} GIZOBSTACLE;

ADDGIZMOTYPE *GizObstacles_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
