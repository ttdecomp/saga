#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void InitCables(WORLDINFO_s *) {
}

CABLE_s *CreateCable(GameObject_s *, GameObject_s *, i32) {
    return NULL;
}

void DestroyCable(CABLE_s *) {
}

void ReleaseCable(CABLE_s *, i32) {
}

void UpdateCables() {
}

void CableTargetGameObject(GameObject_s *, nuvec_s *, float) {
}

void CableCode(GameObject_s *, i32, float) {
}
