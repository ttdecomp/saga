#pragma once

#include "game/gizmo.h"

extern int edGizShadow_gizmotype_id;

#ifdef __cplusplus

typedef struct EDGIZSHADOW_s {
} EDGIZSHADOW;

ADDGIZMOTYPE* EdGizShadowMachine_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
