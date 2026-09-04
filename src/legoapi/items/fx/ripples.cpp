#include "decomp.h"
#include "legoapi/legoapi_types.h"

#include <string.h>
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

static i32 *RE_rippleeffect;

void InitRipples(ripple_set_s **result, variptr_u *buf, variptr_u *, i32 count) {
    ripple_set_s *set = reinterpret_cast<ripple_set_s *>((buf->addr + 3) & ~3u);
    buf->addr = reinterpret_cast<usize>(set + 1);
    *result = set;

    usize node_bytes;
    usize effect_bytes;
    i32 node_count;
    if (count < 1) {
        effect_bytes = 0;
        node_bytes = 0;
        node_count = 0;
    } else if (count < 0x40) {
        node_bytes = static_cast<usize>(count) * sizeof(ripple_node_s);
        effect_bytes = static_cast<usize>(count) * sizeof(*RE_rippleeffect);
        node_count = count;
    } else {
        effect_bytes = 0x100;
        node_bytes = 0x2000;
        node_count = 0x40;
    }

    set->count = static_cast<u16>(node_count);
    set->free_count = 0;

    ripple_node_s *nodes = reinterpret_cast<ripple_node_s *>((buf->addr + 0x7f) & ~0x7fu);
    buf->addr = reinterpret_cast<usize>(nodes) + node_bytes;

    if (nodes != NULL) {
        for (i32 i = 1; i < node_count - 1; ++i) {
            nodes[i].next = &nodes[i + 1];
            nodes[i].previous = &nodes[i - 1];
        }
        nodes[0].next = &nodes[1];
        nodes[0].previous = &nodes[node_count - 1];
        nodes[node_count - 1].next = &nodes[0];
        nodes[node_count - 1].previous = &nodes[node_count - 2];
    }

    RE_rippleeffect = reinterpret_cast<i32 *>((buf->addr + 3) & ~3u);
    buf->addr = reinterpret_cast<usize>(RE_rippleeffect) + effect_bytes;
    (*result)->nodes = nodes;
    (*result)->current = (*result)->nodes;
    (*result)->field_0x0c = NULL;
    (*result)->field_0x10 = NULL;
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
        nodes[0].previous = &nodes[count - 1];
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
