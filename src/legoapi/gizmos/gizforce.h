#pragma once

#include "legoapi/gameanim.h"
#include "legoapi/gizmo.h"

extern int force_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZFORCE_s {
    u8 unknown_000[0x10];
    NUVEC position;
    NUVEC centre;
    GAMEANIMSET *anim_set;
    u8 unknown_02c[0x1c];
    i32 unknown_048;
    u8 unknown_04c[4];
    i32 unknown_050;
    u8 unknown_054[0x14];
    f32 radius;
    u8 unknown_06c[0xc];
    u8 unknown_078;
    u8 unknown_079[0x2f];
    u8 flags_0a8;
    u8 flags_0a9;
    u8 flags_0aa;
    u8 unknown_0ab[5];
} GIZFORCE;

ADDGIZMOTYPE *GizForce_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
