#pragma once

#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus

typedef struct TUBE_s {
    char name[16];
    NUVEC position;
    f32 initial_y_offset;
    f32 travel_speed;
    f32 scale;
    f32 target_y;
    f32 travel_speed_squared;
    u8 reserved_30[4];
    union {
        u8 flags;
        struct {
            u8 active : 1;
            u8 visible : 1;
            u8 configured : 1;
            u8 reverse_active : 1;
            u8 loaded_from_file : 1;
            u8 reserved_flags : 3;
        };
    };
    u8 reserved_35[0xb];
} TUBE;

ADDGIZMOTYPE *Tubes_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
