#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

struct GameObject_s;
struct PARTDEBSYS_s;
struct PART_s;

i32 MAXPARTS;
u8 *Part;

extern "C" void AddGameDebris(void *, i32, NUVEC *) {
}

extern "C" void AddVariableShotDebrisEffectTimed5(void *, NUVEC *, void *, NUVEC *, i32, f32, void *, void *, i16,
                                                   u8) {
}

extern "C" void AddVariableShotDebrisEffectTimed1(void *, NUVEC *, i32, f32, i32, i32, void *) {
}

void AddPartDebris(PARTDEBSYS_s *, i32, NUVEC *) {
}

i32 FindGameDebris(void *, char *) {
    return -1;
}

i32 FindPartDebris(PARTDEBSYS_s *, char *) {
    return -1;
}

void KillPart(void *, i32) {
}

extern "C" void *AddPart(void *) {
    return NULL;
}

void PartCollide_3D(PART_s *) {
}

void AddExplosion(NUVEC *, f32, f32, i32, i32, i32) {
}

void AddPickups(i32, i32, i32, i32, NUVEC *, NUVEC *, f32, i32, f32, f32, GameObject_s *, i32, i32, bool) {
}
