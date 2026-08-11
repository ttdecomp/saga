#include "legoapi/bolts.h"
#include "legoapi/gameobject.h"
#include "nu2api/nucore/common.h"

BOLT *objhitobj_bolt;
void (*Punch_HitHoldFn)(GameObject_s *object, GameObject_s *target);
void (*Punch_HitExtraCodeFn)(GameObject_s *object, NUVEC *hit_position);
i32 (*Punch_GetDamageFn)(GameObject_s *object, GameObject_s *target);

i32 ObjHitObj_Flags(GameObject_s *) {
    return 0;
}

i32 CannotKill(GameObject_s *) {
    return 0;
}

i32 NewBlockAction(GameObject_s *) {
    return 0;
}

i32 ObjHitObj(GameObject_s *, GameObject_s *, i32, u16, i32, i32) {
    return 0;
}

void ObjHitShield(GameObject_s *, GameObject_s *, i32, BOLT *) {
}

i32 KillGameObject(GameObject_s *, i32, i32) {
    return 0;
}

void InstantKillParts(GameObject_s *, i32, f32) {
}
