#pragma once

#include "game/gizmo.h"

extern int gizaimessage_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZAIMESSAGE_s {
} GIZAIMESSAGE;

ADDGIZMOTYPE* GizAIMessage_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
