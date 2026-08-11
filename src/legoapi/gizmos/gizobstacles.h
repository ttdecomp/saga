#pragma once

#include "legoapi/gizmo.h"

extern int obstacle_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZOBSTACLE_s {
    char name[0x10];
    NUVEC position;
    u8 unknown_01c[0x18];
    void *animation_set;
    u8 unknown_038[0x52];
    u16 score;
    u8 unknown_08c[0xc];
    u8 flags;
    u8 reverse_flags;
    u8 unknown_09a[2];
    i32 proximity;
    u8 behavior_flags;
    u8 unknown_0a1[3];
} GIZOBSTACLE;

#if UINTPTR_MAX != UINT64_MAX
static_assert(sizeof(GIZOBSTACLE) == 0xa4, "GIZOBSTACLE size");
static_assert(offsetof(GIZOBSTACLE, animation_set) == 0x34, "GIZOBSTACLE animation set offset");
static_assert(offsetof(GIZOBSTACLE, score) == 0x8a, "GIZOBSTACLE score offset");
static_assert(offsetof(GIZOBSTACLE, flags) == 0x98, "GIZOBSTACLE flags offset");
static_assert(offsetof(GIZOBSTACLE, proximity) == 0x9c, "GIZOBSTACLE proximity offset");
#endif

ADDGIZMOTYPE *GizObstacles_RegisterGizmo(int type_id);
i32 GizObstacles_TotalScore(void *world);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
