#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern EXPLOSION Explosion[8];
extern i32 i_explosion;

void AddExplosion(nuvec_s *, float, float, GameObject_s *, i32, i32) {
}

void SetupBlowupSfx(WORLDINFO_s *, specialsfx_s *) {
}

void ResetExplosions() {
    memset(Explosion, 0, sizeof(Explosion));
    i_explosion = 0;
}

void UpdateExplosions() {
}

void UpdateExplosion_Generic(EXPLOSION *) {
}
