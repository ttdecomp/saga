#include "legoapi/bolts.h"
#include "legoapi/gizmo.h"

i32 GizmoSys_BoltHit(GIZMOSYS *, void *, BOLT *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *) {
    return 0;
}

i32 GizmoBlowUp_Hit(GameObject_s *, NUVEC *, i32, f32, NUVEC *, NUVEC *, BOLT *, u32, u8 *) {
    return 0;
}

void *GizmoBlowUp_FindFromPlatID(WORLDINFO *, i32) {
    return NULL;
}

i32 GizmoBlowupBlowup(void *, i32, i32, i32, GameObject_s *) {
    return 0;
}
