#include "decomp.h"
#include "legoapi/legoapi_types.h"

void Grapple_DrawLine(GameObject_s *) {
}

void Grapple_MoveCode(GameObject_s *) {
}

GameObject_s *Grapple_Occupied(GRAPPLE_s *, GameObject_s *, AIPATHCNX_s *) {
    return NULL;
}

i32 Grapple_LookAtPos(GameObject_s *, nuvec_s *) {
    return 0;
}

void Grapple_AddDynamic(void *, i32) {
}

void Grapple_ReachedTop(GameObject_s *) {
}

void Grapple_FindNearest(WORLDINFO_s *, nuvec_s *, GameObject_s *, float *) {
}

void Grapple_SetRotOrder(GameObject_s *) {
}

void Grapple_SetTargetMom(GameObject_s *) {
}

void Grapple_RemoveDynamic(void *) {
}

void Grapple_FindNearestToPos(WORLDINFO_s *, nuvec_s *) {
}

void Grapple_SetPlayerTargetPoint(GameObject_s *, nuvec_s *) {
}

// Static grapple list helpers. Moved from gizmisc_stubs.cpp.

static __used__ void Grapple_FindNearestInList(nuvec_s *, GRAPPLE_s *, int, GameObject_s *, GRAPPLE_s **, float *) {
}
