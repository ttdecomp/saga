#pragma once

#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus

typedef struct SHARD_s {
    char name[0x10];
    char reserved_10[0x44];
} SHARD;

ADDGIZMOTYPE *Shards_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
