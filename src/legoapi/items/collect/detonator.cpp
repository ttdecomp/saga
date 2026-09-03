#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Detonators_Draw() {
}

void Detonators_Reset() {
    memset(Detonator, 0, sizeof(Detonator));
}

void Detonators_Update() {
}

void Detonator_Detonate(DETONATOR_s *) {
}

void Detonator_MoveCode(GameObject_s *) {
}

void Detonator_FindNearest(nuvec_s *, float, GameObject_s *) {
}
