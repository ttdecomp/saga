#pragma once

#include "game/gizmo.h"

extern int blowup_gizmotype_id;

#ifdef __cplusplus

typedef struct BLOWUP_s {
} BLOWUP;

ADDGIZMOTYPE* NewBlowup_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
