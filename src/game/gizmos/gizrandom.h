#pragma once

#include "game/gizmo.h"

extern int gizrandom_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZRANDOM_s {
} GIZRANDOM;

ADDGIZMOTYPE* GizRandom_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
