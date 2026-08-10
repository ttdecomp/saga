#pragma once

#include "legoapi/gizmo.h"

#ifdef __cplusplus

typedef struct LEDGE_s {
    u8 unknown_00[8];
    NUVEC position;
    i16 unknown_14;
    i8 unknown_16;
    u8 unknown_17[2];
    u8 unknown_19;
    u8 unknown_1a[2];
    i16 unknown_1c;
    i16 unknown_1e;
    u8 unknown_20[0x18];
} LEDGE;

ADDGIZMOTYPE *Ledges_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
