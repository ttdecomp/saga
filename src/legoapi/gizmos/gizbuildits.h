#pragma once

#include "legoapi/gizmo.h"

struct GameObject_s;

extern int gizbuildit_gizmotype_id;

#ifdef __cplusplus

struct GIZBUILDITPARTTARGET_s {
    u8 unknown_00[0x30];
    NUVEC head_target;
};

struct GIZBUILDITPART_s {
    u8 unknown_00[0x28];
    GIZBUILDITPARTTARGET_s *target;
};

typedef struct GIZBUILDIT_s {
    u8 unknown_00[0x14];
    GIZBUILDITPART_s **parts;
    u8 unknown_18[0x20];
    NUVEC position;
    u8 unknown_44[4];
    f32 previous_step_time;
    f32 step_time;
    u8 unknown_50[0x24];
    u8 part_count;
    u8 unknown_75[2];
    u8 current_part;
} GIZBUILDIT;

#if UINTPTR_MAX != UINT64_MAX
static_assert(offsetof(GIZBUILDIT, parts) == 0x14, "GIZBUILDIT parts offset");
static_assert(offsetof(GIZBUILDIT, position) == 0x38, "GIZBUILDIT position offset");
static_assert(offsetof(GIZBUILDIT, previous_step_time) == 0x48, "GIZBUILDIT previous step time offset");
static_assert(offsetof(GIZBUILDIT, step_time) == 0x4c, "GIZBUILDIT step time offset");
static_assert(offsetof(GIZBUILDIT, part_count) == 0x74, "GIZBUILDIT part count offset");
static_assert(offsetof(GIZBUILDIT, current_part) == 0x77, "GIZBUILDIT current part offset");
#endif

ADDGIZMOTYPE *GizBuildIts_RegisterGizmo(int type_id);
void GizBuildIt_SetHeadTarget(GIZBUILDIT *buildit, GameObject_s *object);
f32 GizBuildItMul(GameObject_s *object);
void GizBuildIt_SetStepTime(GIZBUILDIT *buildit, GameObject_s *object);
void ReleaseBuildIt(GameObject_s *object, i32 complete);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
