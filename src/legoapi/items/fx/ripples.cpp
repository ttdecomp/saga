#include "decomp.h"
#include "legoapi/legoapi_types.h"

#include <string.h>
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

void ResetRippleSet(ripple_set_s *set) {
    u16 count = set->count;
    ripple_node_s *nodes = set->nodes;

    memset(nodes, 0, count * sizeof(*nodes));
    memset(&set->reset_state, 0, sizeof(set->reset_state));
    set->current = NULL;
    set->field_0x0c = NULL;
    set->field_0x10 = NULL;
    set->nodes = nodes;
    set->count = count;

    if (nodes != NULL) {
        for (i32 i = 1; i < count - 1; ++i) {
            nodes[i].next = &nodes[i + 1];
            nodes[i].previous = &nodes[i - 1];
        }
        nodes[0].next = &nodes[1];
        nodes[count - 1].next = &nodes[0];
        nodes[count - 1].previous = &nodes[count - 2];
    }

    set->current = nodes;
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
