#pragma once

#include "game/gizmo.h"

extern int pushblock_gizmotype_id;

#ifdef __cplusplus

typedef struct PUSHBLOCK_s {
} PUSHBLOCK;

ADDGIZMOTYPE* Push_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
