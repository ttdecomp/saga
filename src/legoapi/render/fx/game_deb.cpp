#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void GenDebDummy(debkeydatatype_s *, debinftype *, float) {
}

void GenDebIndex(debkeydatatype_s *, debinftype *, float) {
}

void GenDebIndexRadial(debkeydatatype_s *, debinftype *, float) {
}

void GenDebIndexBounceY(debkeydatatype_s *, debinftype *, float) {
}

void GenDebIndexBounceXZ(debkeydatatype_s *, debinftype *, float) {
}

void GenDebIndexSpheroid(debkeydatatype_s *, debinftype *, float) {
}

void GenDebMomAdjFromPos(debkeydatatype_s *, debinftype *, uv1deb *) {
}

void GenDebIndexRadialStar(debkeydatatype_s *, debinftype *, float) {
}

void GenDebIndexRadialRotor(debkeydatatype_s *, debinftype *, float) {
}

void GenDebMomAdjFromPosAll(debkeydatatype_s *, debinftype *, uv1deb *) {
}

void GenDebMomAdjFromPosRev(debkeydatatype_s *, debinftype *, uv1deb *) {
}

void GenDebMomAdjFromSplash(debkeydatatype_s *, debinftype *, uv1deb *) {
}

void GenDebMomAdjFromAshRock(debkeydatatype_s *, debinftype *, uv1deb *) {
}

void GenDebIndexImprovedRadial(debkeydatatype_s *, debinftype *, float) {
}

void GenDebMomAdjFromPosRevTree(debkeydatatype_s *, debinftype *, uv1deb *) {
}

extern "C" {

    // Debris-effect registry owned by the parts-page loader (edppLoadPage and
    // friends @0x36c630, not transcribed). The original keeps the list and its
    // count in two unnamed .data slots (@0x61516c / @0x61532c) that only the
    // edpp* loader populates. With no page loaded the list is empty and every
    // lookup misses — the state we faithfully model here.
    static void *deb_effects_list = NULL; // original slot @0x61516c
    static i32 deb_effects_count = 0;     // original slot @0x61532c

    void FindGameDebris(void) {
    }

    // LookupDebrisEffectPageIgnore @0x355d30. Walks the debris-effect registry
    // (index 0 reserved, `ignore` skipped), matching names case-insensitively;
    // returns -1 when nothing matches. The per-entry page-byte routing at +0x11
    // belongs to the parts-page loader state and is only reachable once effects
    // are registered.
    i32 LookupDebrisEffectPageIgnore(char *name, i32 page, i32 ignore) {
        (void)page;
        if (name == NULL || deb_effects_count <= 1) {
            return -1;
        }
        for (i32 i = 1; i < deb_effects_count; i++) {
            if (i == ignore) {
                continue;
            }
            char *effect_name = ((char **)deb_effects_list)[i];
            if (effect_name == NULL) {
                continue;
            }
            if (NuStrICmp(name, effect_name) == 0) {
                return i;
            }
        }
        return -1;
    }

    // LookupDebrisEffectPage @0x355ef0: tail call into the Ignore variant.
    i32 LookupDebrisEffectPage(char *name, char page) {
        return LookupDebrisEffectPageIgnore(name, page, 0);
    }

    i32 LookupDebrisEffectPageOnly(char *name, char page) {
        // @0x355f30 walks the same registry but additionally filters entries by
        // their page byte (+0x11) — parts-page loader state, only reachable
        // once effects are registered. Empty registry ⇒ -1 like the original.
        return LookupDebrisEffectPageIgnore(name, page, -1);
    }

    // InitGameDebris @0x3ca2d0. Carves the debris system from the world's
    // particle bump buffer: a 0xc-byte header {flags, count, entries} followed
    // by `count` 0x14-byte entries {i32 effect handle (-1 = none), char[16]
    // name}. The first `flags` entries are seeded from the static debris_name
    // table and looked up in the loaded pages; the rest stay -1.
    void *InitGameDebris(VARIPTR *cursor, VARIPTR *end, i32 count, i32 flags, char **names, char page) {
        (void)end;
        u32 aligned = (cursor->addr + 0xf) & ~0xfu;
        cursor->addr = aligned;
        cursor->addr += 0xc;
        if (aligned == 0) {
            return NULL;
        }

        u32 *sys = (u32 *)aligned;
        memset(sys, 0, 0xc);
        sys[0] = (u32)flags;
        sys[1] = (u32)count;
        sys[2] = (cursor->addr + 0xf) & ~0xfu;
        cursor->addr = sys[2];
        cursor->addr += (u32)count * 0x14;

        memset((void *)sys[2], 0xff, (usize)count * 0x14);
        u32 entries = sys[2];

        // Seed the named entries from the debris_name table.
        for (i32 i = 0; i < (i32)sys[0]; i++) {
            char *entry = (char *)(entries + (u32)i * 0x14);
            NuStrCpy(entry + 4, names[i]);
            *(i32 *)entry = -1;
            *(i32 *)entry = LookupDebrisEffectPage(entry + 4, page);
        }

        // Extend from the loaded parts-page registry (empty until edppLoadPage
        // runs, so this only fills the remaining handles with -1).
        i32 registry_count = deb_effects_count;
        i32 i = (i32)sys[0];
        for (i32 j = 1; i < (i32)sys[1] && j < registry_count; j++, i++) {
            char *entry = (char *)(entries + (u32)i * 0x14);
            NuStrCpy(entry + 4, ((char **)deb_effects_list)[j]);
            *(i32 *)entry = -1;
            *(i32 *)entry = LookupDebrisEffectPageOnly(entry + 4, page);
        }
        // Any entries past both sources keep a -1 handle.
        for (; i < (i32)sys[1]; i++) {
            *(i32 *)(entries + (u32)i * 0x14) = -1;
        }
        return (void *)sys;
    }

    void LookupDebrisEffect(void) {
    }

} // extern "C"
