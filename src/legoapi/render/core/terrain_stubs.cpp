#include "legoapi/world/world_shared.h"
#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/nu3d/numtl.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

extern "C" {
    extern debkeydatatype_s *debkeydata;
    extern debinftype **debtab;
    extern i32 EDPP_MAX_TYPES;
    extern i32 EDPP_SCALE_TYPES;
    extern i32 edpp_types_used;
    extern debinftype *effecttypes;
    extern debscale_s *debscale;
    extern f32 panelglobaltime;
    extern f32 globaltime;
    extern f32 renderpanelglobaltime;
    extern f32 renderglobaltime;
    extern f32 timeincrement;
    extern i32 globalframes;
    extern i32 update_debris_enabled;
    extern u32 debrisseed;
    extern PartHeader **DmaDebTypes;
    extern i32 EDPP_MAX_DMADEBTYPES;
    extern i32 freeDmaDebType;
    extern i32 debris_setup_called;
    extern usize debris_trash_space;
    extern usize debris_trash_size;
    extern i32 debrischunks;
    extern i32 debrischunksglass;
    extern i32 freedebchkptr;
    extern i32 freedebchkptrg;
    extern debris_chunk_control_s *debris_chunk_controls;
    extern debris_chunk_control_s **freechunkcontrols;
    extern i32 freechunkcontrolsptr;
    extern dma_particle_chunk_s **freedebchunks;
    extern dma_particle_chunk_s **freedebchunksglass;
    extern particlechunkrendertype_s *ParticleChunkToRender;
    extern particlechunkrendertype_s *ParticleChunkRenderStack[5];
    extern debris_chunk_control_s *debris_chunk_control_stack[2];
    extern NUMTL *DebMat[10];
    extern i32 debris_suspended;
    extern f32 debris_thinning_level;
    extern i32 forced_debris_thinning;

    void NuRndrParticleGroup(uv1debdata *, PartHeader *, NUMTL *, f32, NUMTX *, i32, f32, f32, f32, f32);
    void NuRndrSetParticleRotation(NUMTX *);

    void DebrisReScale(i32, f32);
    void GenericDebinfoDmaTypeUpdate(debinftype *);
    void DebReAlloc(debkeydatatype_s *, i32);
    extern "C++" void DebrisProcessSpheres(uv1deb *, f32, debinftype *, debkeydatatype_s *, i32);
    extern "C++" {
        void DebrisCleanUpDmaDebTypeTables(void);
        void DebrisProcessAllocation(void);
        void DebrisProcessControlChunks(i32);
        void DebrisProcessGeneration(void);
        void DebrisProcessTriggers(void);
    }
}

// InitGameDebris @0x3ca2d0 (game_deb.cpp).
extern "C" void *InitGameDebris(VARIPTR *cursor, VARIPTR end, i32 count, i32 flags, char **names, char page);
// edppLoadPage @0x36c630 (edtoolsall_plain.cpp) — deferred parts-page loader.
extern "C" i32 edppLoadPage(char *path, i32 flag, i32 flags);
// NuFileExists @nufile (nucore_plain.cpp).
extern "C" i32 NuFileExists(char *name);

extern "C" void noterraininit(void) {
}
extern "C" void TerrainSetCur(void *terrain) {
    (void)terrain;
}
extern "C" void TerrSetPlatScanDist(f32 dist) {
    (void)dist;
}
extern "C" void TerrainPlatformOldUpdate(void) {
}
extern "C" void TerrainPlatformNewUpdate(void) {
}
extern "C" void TerrainSetWallDeflectYScale(f32 scale) {
    (void)scale;
}

void *InitPartDebris(VARIPTR *buf, VARIPTR *buf_end, i32 param1, i32 param2, char **param3, i32 page) {
    (void)buf;
    (void)buf_end;
    (void)param1;
    (void)param2;
    (void)param3;
    (void)page;
    return NULL;
}

// Particles_Load @0x4a2a50.
void Particles_Load(WORLDINFO *world, char **debris_name, i32 count, i32 flags) {
    char path[0x100];

    world->page_pp = -1;
    sprintf(path, "%s.ptl", world->config_file);
    if (NuFileExists(path) != 0) {
        world->page_pp = edppLoadPage(path, 1, (i32)(usize)world->current_gscn);
    }

    world->debris_sys = (APIDEBRISSYS_s *)InitGameDebris(&world->giz_buffer, world->unknown_0108, count, flags,
                                                         debris_name, (char)world->page_pp);
}

extern "C" {

    void AITerrInit(void) {
    }

    void AITerrShadow(void) {
    }

    void AITerrShadowOnPlatform(void) {
    }

    void CheckForPlatInst(void) {
    }

    PartHeader *CreateDmaPartEffectList(void *memory, i32 *size) {
        *size = sizeof(PartHeader);
        return reinterpret_cast<PartHeader *>(ALIGN(reinterpret_cast<usize>(memory), 0x10));
    }

    dma_particle_chunk_s *CreateDmaParticleSet(void *memory, i32 *size) {
        dma_particle_chunk_s *chunk = static_cast<dma_particle_chunk_s *>(memory);
        memset(chunk, 0, sizeof(*chunk));
        chunk->command = 0x52;
        for (i32 i = 0; i < 32; ++i) {
            dma_particle_s &particle = chunk->particles[i];
            particle.position.x = 1.0f;
            particle.position.y = 2.0f;
            particle.position.z = 3.0f;
            particle.start_time = -1.0f;
            particle.momentum.x = 4.0f;
            particle.momentum.y = 5.0f;
            particle.momentum.z = 6.0f;
            particle.inverse_lifetime = 128.0f;
        }
        chunk->end_command = 0;
        *size = sizeof(*chunk);
        return chunk;
    }

    dma_particle_chunk_s *CreateDmaParticleSetGlass(void *memory, i32 *size) {
        return CreateDmaParticleSet(memory, size);
    }

    i32 CreateScaledEffect(i32 effect_index, f32 requested_scale) {
        if (effect_index < 1 || EDPP_MAX_TYPES < effect_index || debtab[effect_index] == NULL) {
            return -1;
        }

        debinftype *effect = debtab[effect_index];
        if (effect->scale != 1.0f) {
            effect_index = effect->unscaled_effect_index;
            effect = debtab[effect_index];
            if (effect == NULL) {
                return -1;
            }
        }
        if (requested_scale == 1.0f && effect_index != 0) {
            return effect_index;
        }

        if (requested_scale < 0.01f) {
            requested_scale = 0.01f;
        }

        for (i32 i = 0; i < EDPP_SCALE_TYPES; ++i) {
            debscale_s &entry = debscale[i];
            if (entry.unscaled_effect_index == effect_index && entry.scale == requested_scale &&
                entry.scaled_effect_index != 0) {
                debinftype *scaled = debtab[entry.scaled_effect_index];
                if (scaled != NULL && scaled->unscaled_effect_index == effect_index) {
                    return entry.scaled_effect_index;
                }
                entry.scale = 0.0f;
            }
        }

        i32 closest_index = effect_index;
        f32 closest_distance = fabsf(requested_scale / effect->scale - 1.0f);
        for (i32 i = 1; i < EDPP_MAX_TYPES; ++i) {
            debinftype *candidate = debtab[i];
            if (candidate != NULL && candidate->unscaled_effect_index == effect_index) {
                f32 distance = 1.0f;
                if (requested_scale != 0.0f && candidate->scale != 0.0f) {
                    distance = fabsf(requested_scale / candidate->scale - 1.0f);
                }
                if (distance < closest_distance) {
                    closest_distance = distance;
                    closest_index = i;
                }
            }
        }
        if (closest_index != 0 && closest_distance < 0.1f) {
            return closest_index;
        }

        i32 scaled_index = 1;
        while (scaled_index < EDPP_MAX_TYPES && debtab[scaled_index] != NULL) {
            ++scaled_index;
        }
        if (scaled_index == EDPP_MAX_TYPES) {
            return closest_index;
        }

        debinftype *scaled = &effecttypes[scaled_index];
        debtab[scaled_index] = scaled;
        *scaled = *effect;
        scaled->native_data = NULL;
        for (i32 i = 0; i < 8; ++i) {
            scaled->particle_keys[i] = -1;
        }
        DebrisReScale(scaled_index, requested_scale);
        scaled->scale = requested_scale;
        scaled->last_render_time = scaled->time_group == 4 ? panelglobaltime : globaltime;
        scaled->unscaled_effect_index = effect_index;

        if (strlen(effect->name) < 13) {
            sprintf(scaled->name, "%s%03d", effect->name, scaled_index);
        } else {
            char shortened[13];
            memcpy(shortened, effect->name, 12);
            shortened[12] = '\0';
            sprintf(scaled->name, "%s%03d", shortened, scaled_index);
        }
        ++edpp_types_used;

        for (i32 i = 0; i < EDPP_SCALE_TYPES; ++i) {
            if (debscale[i].scale == 0.0f) {
                debscale[i].unscaled_effect_index = effect_index;
                debscale[i].scaled_effect_index = scaled_index;
                debscale[i].scale = requested_scale;
                break;
            }
        }
        return scaled_index;
    }

    void CreateScaledPARTEffect(void) {
    }

    void CubeImpact(void) {
    }

    void DebFreeAllCreatedEffects(void) {
    }

    void DebFreeAllDMADebTablesInstantly(void) {
    }

    void DebFreeAllPanelEffects(void) {
    }

    void Debris(i32 update_panel_time) {
        if (update_debris_enabled == 0 || debris_suspended != 0) {
            return;
        }

        panelglobaltime += timeincrement;
        renderpanelglobaltime += timeincrement;
        DebrisProcessControlChunks(1);
        if (update_panel_time == 0) {
            globaltime += timeincrement;
            renderglobaltime += timeincrement;
            ++globalframes;
            DebrisCleanUpDmaDebTypeTables();
            DebrisProcessTriggers();
            DebrisProcessAllocation();
            DebrisProcessGeneration();
            DebrisProcessControlChunks(0);
        }
    }

    void DebrisAllCollisionCheckScaleYFlag(void) {
    }

    void DebrisCollisionCheck(void) {
    }

    void DebrisCollisionCheckFlag(void) {
    }

    void DebrisCollisionCheckScaleY(void) {
    }

    void DebrisCollisionCheckScaleYFlag(void) {
    }

    void DebrisDraw(i32, i32 pass) {
        if (debris_suspended != 0 || pass < 0 || pass >= 5) {
            return;
        }

        for (particlechunkrendertype_s *render_chunk = ParticleChunkRenderStack[pass]; render_chunk != NULL;
             render_chunk = render_chunk->next) {
            debinftype *effect = render_chunk->effect;
            if (render_chunk->particle_chunk == NULL || effect == NULL) {
                continue;
            }
            if (effect->native_data == NULL) {
                if (freeDmaDebType == EDPP_MAX_DMADEBTYPES) {
                    continue;
                }
                GenericDebinfoDmaTypeUpdate(effect);
                if (effect->native_data == NULL) {
                    continue;
                }
            }

            NUMTX matrix;
            NUVEC position;
            if (render_chunk->key == NULL) {
                matrix = render_chunk->effect_orientation;
                position = render_chunk->position;
            } else {
                debkeydatatype_s *key = render_chunk->key;
                if (key->field_2f7 == 0) {
                    continue;
                }
                matrix = key->effect_orientation;
                position = key->position;
            }
            NuMtxTranslate(&matrix, &position);

            if (render_chunk->key != NULL && render_chunk->key->field_2fa != 0) {
                NuRndrSetParticleRotation(&render_chunk->key->particle_orientation);
            } else {
                NuRndrSetParticleRotation(NULL);
            }

            const i32 render_mode = effect->particle_type == 7 ? 4 : (effect->use_explicit_clip_box != 0 ? 6 : 0);
            if (effect->particle_type >= 10 || DebMat[effect->particle_type] == NULL) {
                continue;
            }
            const f32 render_time = effect->time_group == 4 ? panelglobaltime : globaltime;
            effect->last_render_time = render_time;
            NuRndrParticleGroup(reinterpret_cast<uv1debdata *>(render_chunk->particle_chunk), effect->native_data,
                                DebMat[effect->particle_type], render_time, &matrix, render_mode, effect->field_140,
                                effect->field_144, effect->clip_extent, effect->field_044);
        }
    }

    void DebrisDrawGlass(void) {
    }

    void DebrisDrawGlassEx(void) {
    }

    void DebrisEmitterMomentum(void) {
    }

    void DebrisEmitterOrientation(i32 handle, i16 z, i16 y, i16 x) {
        if (handle == -1) {
            return;
        }
        debkeydatatype_s &key = debkeydata[handle];
        NUMTX *mtx = &key.emitter_orientation;
        NuMtxSetIdentity(mtx);
        NuMtxRotateZ(mtx, z);
        NuMtxRotateY(mtx, y);
        NuMtxRotateX(mtx, x);
        mtx->m30 = 0.0f;
        mtx->m31 = 0.0f;
        mtx->m32 = 0.0f;
        key.orientation_dirty = 0.0f;
    }

    void DebrisEmitterOrientationMtx(i32 handle, NUMTX *source) {
        if (handle != -1) {
            debkeydatatype_s &key = debkeydata[handle];
            NUMTX *mtx = &key.emitter_orientation;
            memcpy(mtx, source, sizeof(NUMTX));
            mtx->m30 = 0.0f;
            mtx->m31 = 0.0f;
            mtx->m32 = 0.0f;
            key.orientation_dirty = 0.0f;
        }
    }

    void DebrisEmitterPos(i32 handle, f32 x, f32 y, f32 z) {
        if (handle != -1) {
            debkeydatatype_s &key = debkeydata[handle];
            key.orientation_dirty = 0.0f;
            key.position.x = x;
            key.position.y = y;
            key.position.z = z;
        }
    }

    void DebrisFindAllOfType(void) {
    }

    void DebrisGetConeProperties(void) {
    }

    void DebrisGetDuration(void) {
    }

    void DebrisGetName(void) {
    }

    void DebrisGetParticleCount(void) {
    }

    void DebrisGetRingProperties(void) {
    }

    void DebrisGetSeed(void) {
    }

    void DebrisGlassClose(void) {
    }

    void DebrisGlassInit(void) {
    }

    void DebrisGlassParticlesActive(void) {
    }

    void DebrisOrientation(i32 handle, i16 z, i16 y) {
        if (handle != -1) {
            debkeydatatype_s &key = debkeydata[handle];
            NUMTX *mtx = &key.effect_orientation;
            NuMtxSetIdentity(mtx);
            NuMtxRotateZ(mtx, z);
            NuMtxRotateY(mtx, y);
            mtx->m30 = 0.0f;
            mtx->m32 = 0.0f;
            key.orientation_dirty = 0.0f;
        }
    }

    void DebrisOrientationMtx(i32 handle, NUMTX *source) {
        if (handle != -1) {
            debkeydatatype_s &key = debkeydata[handle];
            NUMTX *mtx = &key.effect_orientation;
            memcpy(mtx, source, sizeof(NUMTX));
            mtx->m30 = 0.0f;
            mtx->m31 = 0.0f;
            mtx->m32 = 0.0f;
            key.orientation_dirty = 0.0f;
        }
    }

    void DebrisParticleMomentum(void) {
    }

    void DebrisPopulateInstance(i32 handle, f32 duration) {
        if (duration < 0.0f || handle == -1) {
            return;
        }

        debkeydatatype_s *key = &debkeydata[handle];
        debinftype *effect = debtab[key->effect_index];
        if (effect->frequency == 0) {
            return;
        }
        if (duration == 0.0f) {
            duration = effect->particle_lifetime;
        }

        const f32 now = effect->time_group == 4 ? panelglobaltime : globaltime;
        const f32 end_time = now + timeincrement;
        f32 emission_time = now - duration;
        if (key->previous_allocated_chunk_count == 0) {
            const f32 thinning =
                forced_debris_thinning == 0
                    ? (debris_thinning_level <= effect->thinning ? debris_thinning_level : effect->thinning)
                    : debris_thinning_level;
            DebReAlloc(key, static_cast<i32>(static_cast<f32>(effect->max_particles) / thinning));
        }

        key->field_184 = 1;
        if (key->allocated_chunk_count <= 0) {
            return;
        }
        const f32 thinning =
            forced_debris_thinning == 0
                ? (debris_thinning_level <= effect->thinning ? debris_thinning_level : effect->thinning)
                : debris_thinning_level;
        const f32 emission_interval = 1.0f / (static_cast<f32>(effect->frequency) / thinning);
        key->emission_epoch = emission_time;
        emission_time += emission_interval;
        for (i32 remaining = 999; emission_time < end_time && remaining != 0; --remaining) {
            uv1deb *particle = key->generator(key, effect, emission_time);
            if (effect->process_spheres != 0 && particle != NULL) {
                DebrisProcessSpheres(particle, emission_time, effect, key, 0);
            }
            emission_time = key->emission_epoch + emission_interval;
        }
    }

    void DebrisPosOrientationMtx(i32 handle, NUMTX *source) {
        if (handle != -1) {
            debkeydatatype_s &key = debkeydata[handle];
            NUMTX *mtx = &key.effect_orientation;
            memcpy(mtx, source, sizeof(NUMTX));
            key.position.x = source->m30;
            key.position.y = source->m31;
            key.position.z = source->m32;
            mtx->m30 = 0.0f;
            mtx->m31 = 0.0f;
            mtx->m32 = 0.0f;
            key.orientation_dirty = 0.0f;
        }
    }

    void DebrisPreCheckCollisions(void) {
    }

    void DebrisProcessTimeSlip(void) {
    }

    void DebrisQueryPriority(void) {
    }

    void DebrisReScale(i32 effect_index, f32 scale) {
        if (effect_index < 0 || EDPP_MAX_TYPES <= effect_index || debtab[effect_index] == NULL) {
            return;
        }

        debinftype *effect = debtab[effect_index];
        for (i32 i = 0; i < 8; ++i) {
            effect->width_keys[i].value *= scale;
            effect->height_keys[i].value *= scale;
        }
        effect->field_148 *= scale;
        effect->field_14c *= scale;
        effect->field_048 *= scale;
        effect->field_0a0 *= scale;

        const u8 generator_type = effect->generator_type;
        if ((generator_type & 0xf7) == 0 || generator_type == 9 || generator_type == 10) {
            effect->field_058 *= scale;
            effect->field_05c *= scale;
            effect->field_060 *= scale;
            effect->field_04c *= scale;
            effect->field_050 *= scale;
            effect->field_054 *= scale;
        } else if (generator_type == 6 || generator_type == 7 || generator_type == 11 || generator_type == 12) {
            effect->field_058 *= scale;
            effect->field_04c *= scale;
        }
        effect->field_0b4 *= scale;
        effect->field_0bc *= scale;
        GenericDebinfoDmaTypeUpdate(effect);
    }

    void DebrisReflectionOrientation(i32 handle, i16 x, i16 y, i32 mode, f32 scale) {
        if (handle != -1) {
            debkeydatatype_s &key = debkeydata[handle];
            key.reflection_mode = mode;
            key.reflection_x = x;
            key.reflection_y = y;
            key.reflection_scale = scale;
        }
    }

    void DebrisRegisterCutoffCameraVec(void) {
    }

    void DebrisReserveTrashableSpace(void) {
    }

    void DebrisResetTimers(void) {
    }

    void DebrisSetCutSceneMode(void) {
    }

    void DebrisSetDetailLevels(void) {
    }

    void DebrisSetDrawFlag(void) {
    }

    void DebrisSetFacing(void) {
    }

    void DebrisSetGroupID(void) {
    }

    void DebrisSetPriority(void) {
    }

    void DebrisSetRenderGroup(void) {
    }

    void DebrisSetRoomID(void) {
    }

    void DebrisSetSeed(void) {
    }

    void DebrisSetTimeIncrement(f32 increment) {
        timeincrement = increment;
    }

    void DebrisSetTrigger(i32 handle, i32 first, i32 second, i32 third) {
        if (handle != -1) {
            debkeydatatype_s &key = debkeydata[handle];
            key.trigger_third = third;
            key.trigger_first = first;
            key.trigger_second = second;
        }
    }

    void DebrisSetUserData(void) {
    }

    void DebrisShift(void) {
    }

    void DebrisStartOffsetEx(debkeydatatype_s *key, f32 offset) {
        if (key == NULL) {
            return;
        }
        const i16 effect_index = key->effect_index;
        debinftype *effect = debtab[effect_index];
        f32 now = effect->time_group == 4 ? panelglobaltime : globaltime;
        f32 start;
        f32 period;
        if (effect->emission_period_random == 0.0f && effect->emission_pause_random == 0.0f) {
            const f32 interval = effect->emission_period + effect->emission_pause;
            start = static_cast<f32>(static_cast<i32>(now / interval)) * interval;
            if (effect->generator_type == 7 && effect->emission_pause == 0.0f) {
                const f32 frames = offset * 60.0f;
                key->emitter_rotation_x = static_cast<i16>(static_cast<i32>(effect->field_050 * frames));
                key->emitter_rotation_y = static_cast<i16>(static_cast<i32>(effect->field_054 * frames));
            } else {
                start += offset;
            }
            start += interval;
            key->emission_time = start;
            while (now < start) {
                start -= interval;
            }
        } else {
            start = now;
            key->emission_time = start;
        }
        period = effect->emission_period;
        key->previous_emission_time = -10.0f;
        key->field_1e4 = NuRandFloatSeeded(&debrisseed) * effect->emission_period_random + start + period;
    }

    void DebrisStartOffset(i32 handle, f32 offset) {
        if (handle != -1) {
            DebrisStartOffsetEx(debkeydata + handle, offset);
        }
    }

    void DebrisStatusAlwaysOff(void) {
    }

    void DebrisStatusAlwaysOn(void) {
    }

    void DebrisStatusNormal(void) {
    }

    void DebrisTorusCollisionCheck(void) {
    }

    void DebrisTorusCollisionCheckFlag(void) {
    }

    void DebrisTorusCollisionCheckScaleY(void) {
    }

    void DebrisTorusCollisionCheckScaleYFlag(void) {
    }

    void DebrisTrashableSetup(VARIPTR *buffer) {
        if (debris_setup_called == 0) {
            return;
        }

        usize cursor = debris_trash_space;
        if (cursor == 0) {
            if (buffer == NULL) {
                return;
            }
            buffer->addr = ALIGN(buffer->addr, 0x10);
            cursor = buffer->addr;
        }

        const i32 chunk_count = debrischunks + debrischunksglass;
        memset(debris_chunk_controls, 0, static_cast<usize>(chunk_count) * 2 * sizeof(debris_chunk_control_s));
        memset(freechunkcontrols, 0, static_cast<usize>(chunk_count) * 2 * sizeof(debris_chunk_control_s *));
        memset(freedebchunks, 0, static_cast<usize>(debrischunks) * sizeof(dma_particle_chunk_s *));
        memset(freedebchunksglass, 0, static_cast<usize>(debrischunksglass) * sizeof(dma_particle_chunk_s *));
        memset(ParticleChunkToRender, 0, static_cast<usize>(chunk_count) * sizeof(particlechunkrendertype_s));

        for (i32 i = 0; i < debrischunks; ++i) {
            i32 size;
            freedebchunks[i] = CreateDmaParticleSet(reinterpret_cast<void *>(cursor), &size);
            cursor += static_cast<usize>(size);
        }
        for (i32 i = 0; i < debrischunksglass; ++i) {
            i32 size;
            freedebchunksglass[i] = CreateDmaParticleSetGlass(reinterpret_cast<void *>(cursor), &size);
            cursor += static_cast<usize>(size);
        }

        freedebchkptr = 0;
        freedebchkptrg = 0;
        for (i32 i = 0; i < EDPP_MAX_DMADEBTYPES; ++i) {
            i32 size;
            DmaDebTypes[i] = CreateDmaPartEffectList(reinterpret_cast<void *>(cursor), &size);
            cursor += static_cast<usize>(size);
        }

        if (debris_trash_space == 0) {
            debris_trash_space = buffer->addr;
            debris_trash_size = cursor - buffer->addr;
            buffer->addr = cursor;
        }

        debris_chunk_control_stack[0] = NULL;
        debris_chunk_control_stack[1] = NULL;
        for (i32 i = 0; i < chunk_count * 2; ++i) {
            freechunkcontrols[i] = &debris_chunk_controls[i];
        }
        freechunkcontrolsptr = 0;
        ParticleChunkRenderStack[0] = NULL;
        ParticleChunkRenderStack[1] = NULL;
        ParticleChunkRenderStack[2] = NULL;
        ParticleChunkRenderStack[3] = NULL;
        ParticleChunkRenderStack[4] = NULL;
    }

    void DebrisTypeStatusAlwaysOff(void) {
    }

    void DebrisTypeStatusAlwaysOn(void) {
    }

    void DebrisTypeStatusNormal(void) {
    }

    void DeletePlatinst(void) {
    }

    void DrawHitTerrain(void) {
    }

    void DrawPlatform(void) {
    }

    void FindPlatInst(void) {
    }

    void NewMSituTerrEx(void) {
    }

    void NewPlatInst(void) {
    }

    void NewPlatInstMSitu(void) {
    }

    void NewPlatPickupInst(void) {
    }

    void NewRayCast(void) {
    }

    void NewRayCastEx(void) {
    }

    void NewRayCastGetEmbedDist(void) {
    }

    void NewRayCastGetImpactNormal(void) {
    }

    void NewRayCastGetImpactTerrainType(void) {
    }

    void NewRayCastGetTOFI(void) {
    }

    void NewRayCastHitWallSpline(void) {
    }

    void NewRayCastMask(void) {
    }

    void NewRayCastPlatForm(void) {
    }

    void NewRayCastScaleY(void) {
    }

    void NewRayCastScaleYMask(void) {
    }

    void NewRayCastSet(void) {
    }

    void NewRayCastSetHandel(void) {
    }

    void NewRayCastSetMask(void) {
    }

    void NewRaySetDisablePalt(void) {
    }

    void NewScanHandel(void) {
    }

    void NewScanInit(void) {
    }

    void NewShadow(void) {
    }

    void NewShadowEx(void) {
    }

    void NewShadowHandelEx(void) {
    }

    void NewShadowOnMSitu(void) {
    }

    void NewShadowOnPlatform(void) {
    }

    void NewShapeInit(void) {
    }

    void NewTerrAxisFreedom(void) {
    }

    void NewTerrHitInfo(void) {
    }

    void NewTerrPlatformsOff(void) {
    }

    void NewTerrain(void) {
    }

    void NewTerrainOnAPlatform(void) {
    }

    void NewTerrainScaleY(void) {
    }

    void NewTerrainScaleYMask(void) {
    }

    void PartRayCast(void) {
    }

    void PartTerrInit(void) {
    }

    void PlatImpactInfo(void) {
    }

    void PlatInstBounce(void) {
    }

    void PlatInstCenter(void) {
    }

    void PlatInstGetHit(void) {
    }

    void PlatInstRotate(void) {
    }

    void PlatInstSkinRegister(void) {
    }

    void PlatInstSkinRegisterEx(void) {
    }

    void PlatOnOff(void) {
    }

    void PlatSkinEndReigster(void) {
    }

    void PlatSkinMemReset(void) {
    }

    void PlatSkinMemRigister(void) {
    }

    void PlatformCrush(void) {
    }

    void PlatformRemoveCallback(void) {
    }

    void PlatformUpdateCallback(void) {
    }

    void QuickNewRayCast(void) {
    }

    void SortDebrisRenderStack(void) {
    }

    void TerrDrawImpactPol(void) {
    }

    void TerrErrorString(void) {
    }

    void TerrTempMemory(void) {
    }

    void TerrainAddWallSpline(void) {
    }

    void TerrainFreeId(void) {
    }

    void TerrainGetCur(void) {
    }

    void TerrainGetModelByInst(void) {
    }

    void TerrainInfo(void) {
    }

    void TerrainInfoExtra(void) {
    }

    void TerrainIntensityInfo(void) {
    }

    void TerrainPlatGetMtx(void) {
    }

    void TerrainPlatId(void) {
    }

    void TerrainPolyEdge(void) {
    }

    void TerrainRemoveWallSpline(void) {
    }

    void TerrainScanWallSpline(void) {
    }

    void TerrainSetImpactData(void) {
    }

    void TerrainSetPlatConnectTol(void) {
    }

    void TerrainTrackBack(void) {
    }

    void TerrainTrackFlush(void) {
    }

    void TerrainWallAng(void) {
    }

    void TerrainWallSideSlide(void) {
    }

    void UpdateDebrisRenderStackPriority(void) {
    }

    void terraininit(void) {
    }

    void terrainpickupinit(char *, void **) {
    }

    void UpdatePlatinst(void) {
    }

} // extern "C"
