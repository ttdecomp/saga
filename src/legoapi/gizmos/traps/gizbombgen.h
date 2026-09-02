#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"

extern i32 bombgen_gizmotype_id;

#ifdef __cplusplus

struct GAMEANIMSET_s;
struct GameObject_s;

enum GIZBOMBGEN_FLAGS {
    GIZBOMBGEN_FLAG_ACTIVE = 1 << 0,
    GIZBOMBGEN_FLAG_VISIBLE = 1 << 1,
};

enum GIZBOMBGENSYS_FLAGS {
    GIZBOMBGENSYS_FLAG_RESET = 1 << 0,
    GIZBOMBGENSYS_FLAG_KILL_BOMBS_ON_RESET = 1 << 1,
};

typedef struct GIZBOMBGEN_s {
    char name[16];
    NUVEC position;
    GAMEANIMSET_s *anim_set;
    i32 interval;
    GameObject_s *generated_bomb;
    i16 field_0x28;
    u8 flags;
    u8 field_0x2b;
} GIZBOMBGEN;

DECOMP_ASSERT(sizeof(GIZBOMBGEN) == 0x2c, "GIZBOMBGEN ABI");
DECOMP_ASSERT(offsetof(GIZBOMBGEN, generated_bomb) == 0x24, "GIZBOMBGEN generated bomb offset");
DECOMP_ASSERT(offsetof(GIZBOMBGEN, flags) == 0x2a, "GIZBOMBGEN flags offset");

typedef struct GIZBOMBGENSYS_s {
    GIZBOMBGEN *bomb_generators;
    u16 count;
    u16 capacity;
    void *anim_object_pool;
    u8 flags;
    u8 reserved_0x0d[3];
    void *progress;
} GIZBOMBGENSYS;

DECOMP_ASSERT(sizeof(GIZBOMBGENSYS) == 0x14, "GIZBOMBGENSYS ABI");
DECOMP_ASSERT(offsetof(GIZBOMBGENSYS, flags) == 0x0c, "GIZBOMBGENSYS flags offset");
DECOMP_ASSERT(offsetof(GIZBOMBGENSYS, progress) == 0x10, "GIZBOMBGENSYS progress offset");

ADDGIZMOTYPE *GizBombGen_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
