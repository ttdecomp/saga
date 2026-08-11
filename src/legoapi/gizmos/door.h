#pragma once

#include "legoapi/gizmo.h"

typedef struct DOOR_s {
    u8 unknown_000[0xf0];
    i16 destination_level_index;
} DOOR;

static_assert(offsetof(DOOR, destination_level_index) == 0xf0, "DOOR destination level index offset");

#ifdef __cplusplus

ADDGIZMOTYPE *Door_RegisterGizmo(int type_id);

#endif
