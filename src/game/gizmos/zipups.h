#pragma once

#include "game/gizmo.h"

extern int zipup_gizmotype_id;

#ifdef __cplusplus

typedef struct ZIPUP_s {
} ZIPUP;

ADDGIZMOTYPE* ZipUps_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
