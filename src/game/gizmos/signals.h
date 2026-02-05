#pragma once

#include "game/gizmo.h"

#ifdef __cplusplus

typedef struct SIGNAL_s {
} SIGNAL;

ADDGIZMOTYPE* Signals_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
