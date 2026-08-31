#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" void NuTexAnimSetSignals(u32 signals);

void Signal_MoveCode(WORLDINFO_s *, GameObject_s *) {
}

void SetTexAnimSignals() {
    NuTexAnimSetSignals(texanimbits);
}

void Signal_FindNearest(WORLDINFO_s *, nuvec_s *, GameObject_s *, float *) {
}

void Signals_InitTerrain(WORLDINFO_s *) {
}
