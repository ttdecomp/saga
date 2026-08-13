#pragma once

#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/gizmo.h"

#ifdef __cplusplus

typedef struct LEVELSCRIPTPROCESS_s {
    char name[0x10];
    AISCRIPTPROCESS processor;

    u32 unknown_d8;
} LEVELSCRIPTPROCESS;

typedef struct AI_s {
} AI;

ADDGIZMOTYPE* AI_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
