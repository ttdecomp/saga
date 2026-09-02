#pragma once

#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus

typedef struct GUIDELINE_s {
    char name[8];
    NUVEC start_position;
    NUVEC end_position;
    union {
        u8 flags;
        struct {
            u8 active : 1;
            u8 visible : 1;
            u8 reserved_flags : 6;
        };
    };
    u8 reserved_21[3];
} GUIDELINE;

ADDGIZMOTYPE *GuideLines_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
