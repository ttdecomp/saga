#pragma once

#include "legoapi/gizmo.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus

typedef struct TUBE_s {
    u8 unknown_00[0x10];
    NUVEC position;
    u8 unknown_1c[0xc];
    f32 top_y;
    f32 radius_squared;
    u8 unknown_30[4];
    u32 flags;
    u8 unknown_38[8];
} TUBE;

struct GameObject_s;
struct WORLDINFO_s;

static_assert(sizeof(TUBE) == 0x40, "TUBE size");

i32 Tube_InCylinder(GameObject_s *object, TUBE *tube, f32 *distance_squared, i32 ignore_height);
TUBE *Tube_InAnyCylinder(WORLDINFO_s *world, GameObject_s *object, i32 ignore_height);
i32 ObjInTube(GameObject_s *object);

ADDGIZMOTYPE *Tubes_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
