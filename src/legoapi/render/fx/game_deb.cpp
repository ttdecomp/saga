#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nuvec.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

NUMTL *CreateAlphaBlendTexture(VARIPTR *, VARIPTR, char *, i32, i32, i32, i32);
NUMTL *CreateCopyMat(NUMTL *, i32, i32, i32, i32);

uv1deb *GenDebDummy(debkeydatatype_s *, debinftype *, float) {
    return NULL;
}

extern "C" {
    extern u32 debrisseed;
    void GenericDebinfoDmaTypeUpdate(debinftype *);
}

static dma_particle_s *DebrisParticleAt(debkeydatatype_s *key, i16 index, u8 particle_type) {
    const i32 particles_per_chunk = particle_type == 7 ? 12 : 32;
    return &key->particle_chunks[index / particles_per_chunk]->particles[index % particles_per_chunk];
}

uv1deb *GenDebIndex(debkeydatatype_s *key, debinftype *effect, float time) {
    i16 particle_index = key->field_18a;
    if (particle_index < key->particle_count) {
        ++key->field_18a;
    } else {
        particle_index = 0;
        key->field_18a = 1;
    }

    dma_particle_s *particle = DebrisParticleAt(key, particle_index, effect->particle_type);
    particle->start_time = time;
    key->emission_epoch = time;
    particle->inverse_lifetime = 64.0f / effect->particle_lifetime;

    NUVEC displacement;
    NuVecScale(&displacement, &effect->emitter_velocity, time - key->emission_time);

    NUVEC random_position;
    f32 random = NuRandFloatSeeded(&debrisseed);
    random_position.x = (random + random) * effect->field_058 - effect->field_058;
    random = NuRandFloatSeeded(&debrisseed);
    random_position.y = (random + random) * effect->field_05c - effect->field_05c;
    random = NuRandFloatSeeded(&debrisseed);
    random_position.z = (random + random) * effect->field_060 - effect->field_060;
    NuVecMtxTransformVU0(&random_position, &random_position, &key->emitter_orientation);
    NuVecAdd(&particle->position, &random_position, &displacement);

    random = NuRandFloatSeeded(&debrisseed);
    particle->momentum.x = (random + random) * effect->field_04c - effect->field_04c;
    random = NuRandFloatSeeded(&debrisseed);
    particle->momentum.y = (random + random) * effect->field_050 - effect->field_050 + effect->field_048;
    random = NuRandFloatSeeded(&debrisseed);
    particle->momentum.z = (random + random) * effect->field_054 - effect->field_054;
    NuVecMtxTransformVU0(&particle->momentum, &particle->momentum, &key->emitter_orientation);
    if (key->momentum_adjuster != NULL) {
        key->momentum_adjuster(key, effect, reinterpret_cast<uv1deb *>(particle));
    }
    NuVecAdd(&particle->position, &particle->position, &key->emission_position);
    NuVecAdd(&particle->momentum, &particle->momentum, &key->momentum);

    for (i32 trail = 0; trail < static_cast<i8>(effect->trail_count); ++trail) {
        i16 trail_index = key->field_18a;
        if (trail_index < key->particle_count) {
            ++key->field_18a;
        } else {
            trail_index = 0;
            key->field_18a = 1;
        }
        dma_particle_s *trail_particle = DebrisParticleAt(key, trail_index, effect->particle_type);
        *trail_particle = *particle;
        trail_particle->start_time += static_cast<f32>(trail + 1) * effect->trail_time;
    }

    if (effect->native_data == NULL) {
        GenericDebinfoDmaTypeUpdate(effect);
    }
    return reinterpret_cast<uv1deb *>(particle);
}

uv1deb *GenDebIndexRadial(debkeydatatype_s *, debinftype *, float) {
    return NULL;
}

uv1deb *GenDebIndexBounceY(debkeydatatype_s *, debinftype *, float) {
    return NULL;
}

uv1deb *GenDebIndexBounceXZ(debkeydatatype_s *, debinftype *, float) {
    return NULL;
}

uv1deb *GenDebIndexSpheroid(debkeydatatype_s *, debinftype *, float) {
    return NULL;
}

void GenDebMomAdjFromPos(debkeydatatype_s *, debinftype *, uv1deb *) {
}

uv1deb *GenDebIndexRadialStar(debkeydatatype_s *, debinftype *, float) {
    return NULL;
}

uv1deb *GenDebIndexRadialRotor(debkeydatatype_s *, debinftype *, float) {
    return NULL;
}

void GenDebMomAdjFromPosAll(debkeydatatype_s *, debinftype *, uv1deb *) {
}

void GenDebMomAdjFromPosRev(debkeydatatype_s *, debinftype *, uv1deb *) {
}

void GenDebMomAdjFromSplash(debkeydatatype_s *, debinftype *, uv1deb *) {
}

void GenDebMomAdjFromAshRock(debkeydatatype_s *, debinftype *, uv1deb *) {
}

uv1deb *GenDebIndexImprovedRadial(debkeydatatype_s *, debinftype *, float) {
    return NULL;
}

void GenDebMomAdjFromPosRevTree(debkeydatatype_s *, debinftype *, uv1deb *) {
}

extern "C" {

    // These are the original registry globals.  `effecttypes` is the
    // contiguous record arena and `debtab` is the separately allocated table
    // of pointers into it.  Slot zero is reserved by the loader.
    debinftype *effecttypes = NULL;
    debinftype **debtab = NULL;
    i32 EDPP_MAX_TYPES = 0;
    i32 EDPP_SCALE_TYPES = 20;
    i32 edpp_types_used = 0;
    i32 DEBPAGE_AREA = -1;
    i32 DEBPAGE_CHARACTER = -1;
    i32 DEBPAGE_GENERAL = -1;
    i32 edpp_page_scene[8] = {};
    i32 edpp_page_on[8] = {};
    i32 edpp_page_used[8] = {};
    u8 edpp_ptls[0xb000] = {};
    debkeydatatype_s *debkeydata = NULL;
    i16 *freedebkeys = NULL;
    i32 freedebkeyptr = 0;
    i32 maxdebkeys = 0;
    debkeydatatype_s *debris_keystack = NULL;
    f32 globaltime = 0.0f;
    f32 panelglobaltime = 0.0f;
    f32 renderglobaltime = 0.0f;
    f32 renderpanelglobaltime = 0.0f;
    f32 timeincrement = 0.0f;
    i32 globalframes = 0;
    i32 update_debris_enabled = 1;
    u32 debrisseed = 0;
    i32 processdeb = 0;
    f32 glyntestha = 0.0f;
    DEBRISMOMENTUMADJUSTER gencodetab[7] = {
        NULL,
        GenDebMomAdjFromPos,
        GenDebMomAdjFromPosRev,
        GenDebMomAdjFromSplash,
        GenDebMomAdjFromAshRock,
        GenDebMomAdjFromPosRevTree,
        GenDebMomAdjFromPosAll,
    };
    DEBRISGENERATOR gensorttab[13] = {
        GenDebIndex,           GenDebDummy,        GenDebDummy,         GenDebDummy,
        GenDebDummy,           GenDebDummy,        GenDebIndexRadial,   GenDebIndexRadialRotor,
        GenDebIndexSpheroid,   GenDebIndexBounceY, GenDebIndexBounceXZ, GenDebIndexImprovedRadial,
        GenDebIndexRadialStar,
    };
    void *perm_debrissys = NULL;
    PartHeader **DmaDebTypes = NULL;
    i32 EDPP_MAX_DMADEBTYPES = 0x40;
    i32 freeDmaDebType = 0;
    i32 debris_setup_called = 0;
    usize debris_trash_space = 0;
    usize debris_trash_size = 0;
    u32 *spherecolldata = NULL;
    u32 *toruscolldata = NULL;
    debscale_s *debscale = NULL;
    i32 debrischunks = 0;
    i32 debrischunksglass = 0;
    i32 freedebchkptr = 0;
    i32 freedebchkptrg = 0;
    debris_chunk_control_s *debris_chunk_controls = NULL;
    debris_chunk_control_s **freechunkcontrols = NULL;
    i32 freechunkcontrolsptr = 0;
    dma_particle_chunk_s **freedebchunks = NULL;
    dma_particle_chunk_s **freedebchunksglass = NULL;
    particlechunkrendertype_s *ParticleChunkToRender = NULL;
    particlechunkrendertype_s *ParticleChunkRenderStack[5] = {};
    debris_chunk_control_s *debris_chunk_control_stack[2] = {};
    NUMTL *DebMat[10] = {};

    dma_particle_chunk_s *CreateDmaParticleSet(void *, i32 *);
    dma_particle_chunk_s *CreateDmaParticleSetGlass(void *, i32 *);
    PartHeader *CreateDmaPartEffectList(void *, i32 *);
    void DebrisTrashableSetup(VARIPTR *);

    void DebrisSetup2(VARIPTR *, VARIPTR, VARIPTR *, VARIPTR, char *, i32, i32, i32, i32);

    void DebrisSetup2(VARIPTR *buffer, VARIPTR buffer_end, VARIPTR *, VARIPTR, char *texture_name, i32 chunk_count,
                      i32 glass_chunk_count, i32 key_count, i32 effect_count) {
        maxdebkeys = key_count;
        EDPP_MAX_TYPES = effect_count;
        debrischunks = chunk_count;
        debrischunksglass = glass_chunk_count;

        const i32 total_chunks = chunk_count + glass_chunk_count;
        buffer->addr = ALIGN(buffer->addr, 0x10);
        debris_chunk_controls = reinterpret_cast<debris_chunk_control_s *>(buffer->addr);
        buffer->addr += static_cast<usize>(total_chunks) * 2 * sizeof(debris_chunk_control_s);
        freechunkcontrols = reinterpret_cast<debris_chunk_control_s **>(buffer->addr);
        buffer->addr += static_cast<usize>(total_chunks) * 2 * sizeof(debris_chunk_control_s *);
        freedebchunks = reinterpret_cast<dma_particle_chunk_s **>(buffer->addr);
        buffer->addr += static_cast<usize>(chunk_count) * sizeof(dma_particle_chunk_s *);
        freedebchunksglass = reinterpret_cast<dma_particle_chunk_s **>(buffer->addr);
        buffer->addr += static_cast<usize>(glass_chunk_count) * sizeof(dma_particle_chunk_s *);
        ParticleChunkToRender = reinterpret_cast<particlechunkrendertype_s *>(buffer->addr);
        buffer->addr += static_cast<usize>(total_chunks) * sizeof(particlechunkrendertype_s);

        buffer->addr = ALIGN(buffer->addr, 0x10);
        debkeydata = reinterpret_cast<debkeydatatype_s *>(buffer->addr);
        buffer->addr += static_cast<usize>(maxdebkeys) * sizeof(debkeydatatype_s);
        freedebkeys = reinterpret_cast<i16 *>(buffer->addr);
        buffer->addr += static_cast<usize>(maxdebkeys) * sizeof(i16);
        effecttypes = reinterpret_cast<debinftype *>(buffer->addr);
        buffer->addr += static_cast<usize>(EDPP_MAX_TYPES) * sizeof(debinftype);
        debtab = reinterpret_cast<debinftype **>(buffer->addr);
        buffer->addr += static_cast<usize>(EDPP_MAX_TYPES) * sizeof(debinftype *);
        DmaDebTypes = reinterpret_cast<PartHeader **>(buffer->addr);
        buffer->addr += static_cast<usize>(EDPP_MAX_DMADEBTYPES) * sizeof(PartHeader *);
        spherecolldata = reinterpret_cast<u32 *>(buffer->addr);
        buffer->addr += static_cast<usize>(maxdebkeys) * 2 * sizeof(u32);
        toruscolldata = reinterpret_cast<u32 *>(buffer->addr);
        buffer->addr += static_cast<usize>(maxdebkeys) * 2 * sizeof(u32);
        debscale = reinterpret_cast<debscale_s *>(buffer->addr);
        buffer->addr += static_cast<usize>(EDPP_SCALE_TYPES) * sizeof(debscale_s);

        memset(freedebkeys, 0, static_cast<usize>(maxdebkeys) * sizeof(i16));
        memset(debkeydata, 0, static_cast<usize>(maxdebkeys) * sizeof(debkeydatatype_s));
        memset(effecttypes, 0, static_cast<usize>(EDPP_MAX_TYPES) * sizeof(debinftype));
        memset(debtab, 0, static_cast<usize>(EDPP_MAX_TYPES) * sizeof(debinftype *));
        memset(debscale, 0, static_cast<usize>(EDPP_SCALE_TYPES) * sizeof(u32));
        memset(debris_chunk_controls, 0, static_cast<usize>(total_chunks) * 2 * sizeof(debris_chunk_control_s));
        memset(freechunkcontrols, 0, static_cast<usize>(total_chunks) * 2 * sizeof(debris_chunk_control_s *));
        memset(ParticleChunkToRender, 0, static_cast<usize>(total_chunks) * sizeof(particlechunkrendertype_s));

        for (i32 i = 0; i < maxdebkeys; ++i) {
            spherecolldata[i * 2] = static_cast<u32>(i);
            spherecolldata[i * 2 + 1] = 3;
            toruscolldata[i * 2] = static_cast<u32>(i);
            toruscolldata[i * 2 + 1] = 3;
        }

        debtab[0] = &effecttypes[0];
        edpp_types_used = 1;

        DebMat[0] = CreateAlphaBlendTexture(buffer, buffer_end, texture_name, 1, 2, 0x100, 0);
        DebMat[0]->attribs.z_mode = 1;
        DebMat[0]->attribs.alpha_test = 1;
        NuMtlUpdate(DebMat[0]);
        DebMat[1] = CreateCopyMat(DebMat[0], 0, 0, 3, 0);
        DebMat[2] = CreateCopyMat(DebMat[0], 1, 1, 1, 1);
        DebMat[3] = CreateCopyMat(DebMat[0], 1, 3, 1, 1);
        DebMat[4] = CreateCopyMat(DebMat[0], 0, 1, 1, 1);
        DebMat[5] = CreateCopyMat(DebMat[0], 0, 2, 1, 1);
        DebMat[6] = CreateCopyMat(DebMat[0], 0, 0, 0, 1);
        for (i32 i = 0; i <= 6; ++i) {
            DebMat[i]->shader_desc.vtx_desc.unknown_2_16 = 1;
            DebMat[i]->tex_id = DebMat[0]->tex_id;
        }

        freedebkeyptr = 0;
        for (i32 i = 0; i < maxdebkeys; ++i) {
            freedebkeys[i] = static_cast<i16>(i);
            debkeydatatype_s *key = &debkeydata[i];
            key->field_184 = 0;
            key->field_18a = 0;
            key->emitter_rotation_x = 0;
            key->emitter_rotation_y = 0;
            key->field_1d4 = 0;
            key->effect_index = 0;
            key->allocation_index = -1;
        }

        debris_setup_called = 1;
        DebrisTrashableSetup(buffer);
    }

    // DebrisSetup @0x34c7d0 is only the argument-shuffling wrapper around the
    // real setup routine.  Keep the call shape visible; DebrisSetup2 owns the
    // arena and all of the ancillary debris tables.
    void DebrisSetup(VARIPTR *p1, VARIPTR p2, char *p3, i32 p4, i32 p5, i32 p6) {
        DebrisSetup2(p1, p2, p1, p2, p3, p4, 0x20, p5, p6);
    }

    i16 FindGameDebris(void *, char *) {
        return -1;
    }

    // LookupDebrisEffectPageIgnore @0x355d30. Search the requested page first,
    // then the two permanent pages, then every other active page.
    i32 LookupDebrisEffectPageIgnore(char *name, i32 page, i32 ignore) {
        if (name == NULL || debtab == NULL) {
            return -1;
        }

        if (static_cast<u32>(page) < 8 && edpp_page_used[page] != 0) {
            for (i32 i = 1; i < EDPP_MAX_TYPES; ++i) {
                debinftype *effect = debtab[i];
                if (i != ignore && effect != NULL && effect->page == static_cast<u8>(page) &&
                    NuStrICmp(effect->name, name) == 0) {
                    return i;
                }
            }
        }

        for (i32 i = 1; i < EDPP_MAX_TYPES; ++i) {
            debinftype *effect = debtab[i];
            if (i != ignore && effect != NULL && effect->page < 2 && edpp_page_used[effect->page] != 0 &&
                NuStrICmp(effect->name, name) == 0) {
                return i;
            }
        }

        for (i32 i = 1; i < EDPP_MAX_TYPES; ++i) {
            debinftype *effect = debtab[i];
            if (i != ignore && effect != NULL && effect->page < 8 && edpp_page_used[effect->page] != 0 &&
                NuStrICmp(effect->name, name) == 0) {
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
        if (name == NULL || debtab == NULL) {
            return -1;
        }

        const u8 requested_page = static_cast<u8>(page);
        if (requested_page < 8 && edpp_page_used[requested_page] != 0) {
            for (i32 i = 1; i < EDPP_MAX_TYPES; ++i) {
                debinftype *effect = debtab[i];
                if (effect != NULL && effect->page == requested_page && NuStrICmp(effect->name, name) == 0) {
                    return i;
                }
            }
        }

        for (i32 i = 1; i < EDPP_MAX_TYPES; ++i) {
            debinftype *effect = debtab[i];
            if (effect != NULL && effect->page < 2 && edpp_page_used[effect->page] != 0 &&
                NuStrICmp(effect->name, name) == 0) {
                return i;
            }
        }
        return -1;
    }

    // InitGameDebris @0x3ca2d0. Carves the debris system from the world's
    // particle bump buffer: a 0xc-byte header {flags, count, entries} followed
    // by `count` 0x14-byte entries {i32 effect handle (-1 = none), char[16]
    // name}. The first `flags` entries are seeded from the static debris_name
    // table and looked up in the loaded pages; the rest stay -1.
    void *InitGameDebris(VARIPTR *cursor, VARIPTR end, i32 count, i32 flags, char **names, char page) {
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

        // The original appends the currently registered page effects after
        // the fixed debris_name set.  effecttypes[0] is reserved, and the
        // pointer table is append-only while pages are loaded.
        i32 i = (i32)sys[0];
        for (i32 j = 1; i < (i32)sys[1] && j < edpp_types_used; j++) {
            debinftype *effect = debtab != NULL ? debtab[j] : NULL;
            if (effect == NULL) {
                break;
            }
            char *entry = (char *)(entries + (u32)i * 0x14);
            NuStrCpy(entry + 4, effect->name);
            *(i32 *)entry = LookupDebrisEffectPageOnly(entry + 4, page);
            i++;
        }

        for (; i < (i32)sys[1]; i++) {
            *(i32 *)(entries + (u32)i * 0x14) = -1;
        }

        return (void *)sys;
    }

    void LookupDebrisEffect(void) {
    }

} // extern "C"
