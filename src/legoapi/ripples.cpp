#include "decomp.h"
#include "legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void InitRipples(ripple_set_s **, variptr_u *, variptr_u *, i32) {
}

void InitRippleMtl(char *, numtl_s **, variptr_u *, variptr_u *) {
}

void AddWaterSplash(GameObject_s *, nuvec_s *) {
}

void BuildRippleMtx(numtx_s *, nuvec_s *, nuvec_s *, u16, u16) {
}

void ResetRippleSet(ripple_set_s *) {
}

void UpdateRippleSet(ripple_set_s *) {
}

void AddSurfaceRipples(GameObject_s *) {
}

void DestroyRippleMtls(WORLDINFO_s *) {
}

void LookupRippleEffectIndex(char *) {
}

void AddRipple(ripple_set_s *, numtx_s *, float, float, float, float, RGBA, RGBA, i32, numtl_s *, nuvec_s *) {
}


void RippleEffects_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}
