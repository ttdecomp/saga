#pragma once

#include "legoapi/gizmo.h"

struct GameObject_s;
struct WORLDINFO_s;

extern int gizmopickup_typeid;

#ifdef __cplusplus

typedef struct GIZMOPICKUP_s {
    u8 unknown_00[8];
    NUVEC position;
    u8 unknown_14[2];
    u8 coin_type;
    u8 unknown_17[0xf];
    u16 powerup_id;
} GIZMOPICKUP;

static_assert(offsetof(GIZMOPICKUP, position) == 0x8, "GIZMOPICKUP position offset");
static_assert(offsetof(GIZMOPICKUP, coin_type) == 0x16, "GIZMOPICKUP coin type offset");
static_assert(offsetof(GIZMOPICKUP, powerup_id) == 0x26, "GIZMOPICKUP powerup ID offset");

ADDGIZMOTYPE *GizmoPickups_RegisterGizmo(int type_id);
void PowerUp_Update(GameObject_s *object);
void Pup_CollectCoin(WORLDINFO_s *world, GIZMOPICKUP *pickup, i32 pickup_index, GameObject_s *object, i32 player_index);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
