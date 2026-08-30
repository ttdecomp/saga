#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nutex.h"
#include "legoapi/world/world.h"

void CutScenes_Reset(WORLDINFO_s *);
void ClearLevelProgress(i32, WORLDINFO_s *);
void Cheats_Reset(void);
void ResetScene(nugscn_s *, SCENEPROGRESS_s *);
void GizmoBlowupVisibilityOverrides(WORLDINFO_s *);
void SetTexAnimSignals(void);
void Customiser_SetUpCharacterData(CUSTOMISER *);
void Surfaces_Reset(void);
void ResetStreaks(void);
void Bolts_Reset(void);
void Batarangs_Reset(void);
void Detonators_Reset(void);
void ResetExplosions(void);
void ShoveObjectSysReset(void);
void Panel_Clear(void);
void GameCam_Reset(GAMECAMERA_s *);
void ResetGameMessages(void);
void Tag_ResetTransfers(void);
void Hint_Reset(void);
void Hint_CancelCurrent(void);
void Teleports_Reset(WORLDINFO_s *);
void TrafficAnimSys_Reset(TRAFFICANIMSYS_s *);
void Pulses_Reset(PULSESYS_s *);
void ResetRepeatSfx(void);
void ResetRippleSet(ripple_set_s *);
void Grabber_Reset(WORLDINFO_s *);
void Faders_Reset(WORLDINFO_s *);
void InitGameMode(void);
extern GAMECAMERA_s *GameCam;
extern ripple_set_s *ripples;
extern "C" void ResetParts(void);
extern "C" void NuSound3StopRumble(void);
void NuDisplayListCreate(nudisplayscene_s *, variptr_u *, variptr_u, i32, i32, i32, i32, i32, i32, i32);

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void CatchUpCode(GameObject_s *, float, float, i32) {
}

void RndrTexQuad(float, float, float, float, i32, numtl_s *, i32) {
}

void SuperWeirdo(GameObject_s *) {
}

void bgProcClose() {
}

void BurnoutApply(i32) {
}

void bgprocFreeze() {
}

void AddPartDebris(PARTDEBSYS_s *, i32, nuvec_s *) {
}

void AddSlamDebris(GameObject_s *) {
}

void CloakMovement(GameObject_s *) {
}

void RndrTexQuad3D(VuMtx const &, i32, numtl_s *) {
}

void CheckResetBits() {
    if ((ResetBits & 0x20) != 0 && WORLD->current_level->area_level_index != -1) {
        ClearLevelProgress(WORLD->current_level->area_level_index, WORLD);
    }

    Cheats_Reset();
    if (WORLD->level_progress != NULL) {
        ResetScene(WORLD->current_gscn, reinterpret_cast<SCENEPROGRESS_s *>(WORLD->level_progress));
        WORLD->level_progress->flags |= 1;
    }

    GizmoBlowupVisibilityOverrides(WORLD);
    texanimbits = 0;
    SetTexAnimSignals();
    SetTexAnimSignals();
    CutScenes_Reset(WORLD);

    if (NOSOUND == 0) {
        if ((ResetBits & 0x40) == 0) {
            InitGameMode();
        } else {
            Customiser_SetUpCharacterData(CharacterCustomiser);
        }
    }

    Surfaces_Reset();
    ResetStreaks();
    Bolts_Reset();
    Batarangs_Reset();
    Detonators_Reset();
    ResetParts();
    ResetExplosions();
    ShoveObjectSysReset();
    Panel_Clear();
    GameCam_Reset(GameCam);
    ResetGameMessages();
    Tag_ResetTransfers();
    Hint_Reset();
    Hint_CancelCurrent();
    Teleports_Reset(WORLD);
    TrafficAnimSys_Reset(WORLD->trafficanim_sys);
    Pulses_Reset(WORLD->pulses_sys);
    NuSound3StopRumble();
    ResetRepeatSfx();
    ResetRippleSet(ripples);
    Grabber_Reset(WORLD);
    Faders_Reset(WORLD);
}

void DebrisTimeSlip(i32) {
}

void bgProcAbortAll() {
}

void bgprocIsFrozen() {
}

void bgprocUnFreeze() {
}

void AddSurfaceDebris(GameObject_s *) {
}

extern NUMTX NuRndr_DebrisMtx;
extern NUVEC4 NuRndr_DebrisPlane;
extern nunativedebrisdata_s *g_ParticleGroup;
extern void *g_pVBData;
extern u32 g_CurrentVBVertexCount;
extern u32 g_FrameVertexCount;
extern u32 g_VBMaxVertexCount;
extern u32 g_CurrentDebriVBIndex;
extern i32 g_UseSysMemVB;
extern i32 NuDebrisRendererNextBuffer();
extern void NuRndrParticleSetRepeat(NUVEC *position);
void AddParticleGroupToDisplayList(nunativedebrisdata_s *group);

static nunativedebrisdata_s *BeginDebrisPacket(NUMTL *material) {
    VARIPTR *buffer = NuDisplayListGetBuffer();
    nunativedebrisdata_s *packet = static_cast<nunativedebrisdata_s *>(buffer->void_ptr);
    buffer->addr += sizeof(*packet);
    packet->vertex_buffer_index = static_cast<u8>(g_CurrentDebriVBIndex);
    packet->use_system_memory_vb = g_UseSysMemVB;
    packet->first_vertex = static_cast<i32>(g_CurrentVBVertexCount);
    packet->vertex_count = 0;
    packet->material = material;
    AddParticleGroupToDisplayList(packet);
    return packet;
}

void BuildDebrisVerts(PartHeader *header, uv1debdata *chunk_data, NUMTL *material, f32 time, NUMTX *matrix,
                      i32 particle_type, f32, f32, f32, f32 near_clip) {
    const f32 u0 = material->particle_type_tag == -105 ? 0.0f : header->texture_u0;
    const f32 v0 = material->particle_type_tag == -105 ? 0.0f : header->texture_v0;
    const f32 u1 = material->particle_type_tag == -105 ? 1.0f : header->texture_u1;
    const f32 v1 = material->particle_type_tag == -105 ? 1.0f : header->texture_v1;
    dma_particle_chunk_s *chunk = reinterpret_cast<dma_particle_chunk_s *>(chunk_data);
    u32 emitted = 0;

    for (i32 particle_index = 0; particle_index < 32; ++particle_index) {
        const dma_particle_s &particle = chunk->particles[particle_index];
        const f32 age = time - particle.start_time;
        const f32 frame_position = particle.inverse_lifetime * age;
        const u32 frame_index = static_cast<u32>(frame_position);
        if (frame_index >= 63) {
            continue;
        }

        NUVEC position = {
            particle.position.x + particle.momentum.x * age,
            particle.position.y + particle.momentum.y * age + header->gravity * age * age * 0.945f,
            particle.position.z + particle.momentum.z * age,
        };
        NuVecMtxTransform(&position, &position, matrix);
        NuRndr_DebrisMtx.m30 = position.x;
        NuRndr_DebrisMtx.m31 = position.y;
        NuRndr_DebrisMtx.m32 = position.z;
        if (particle_type == 6 || particle_type == 7) {
            NuRndrParticleSetRepeat(&position);
        }

        const f32 plane_distance = NuRndr_DebrisPlane.x * position.x + NuRndr_DebrisPlane.y * position.y +
                                   NuRndr_DebrisPlane.z * position.z + NuRndr_DebrisPlane.w;
        if (plane_distance < near_clip) {
            continue;
        }

        if (g_CurrentVBVertexCount + emitted + 6 > g_VBMaxVertexCount) {
            g_ParticleGroup->vertex_count += static_cast<i32>(emitted);
            g_FrameVertexCount += emitted;
            g_CurrentVBVertexCount += emitted;
            emitted = 0;
            if (NuDebrisRendererNextBuffer() == 0) {
                return;
            }
            g_ParticleGroup = BeginDebrisPacket(material);
        }

        const f32 fraction = frame_position - static_cast<f32>(frame_index);
        const f32 inverse_fraction = 1.0f - fraction;
        const debris_particle_frame_s &first = header->frames[frame_index];
        const debris_particle_frame_s &second = header->frames[frame_index + 1];
        NUVEC corners[4];
        corners[0] = {first.position.x * inverse_fraction + second.position.x * fraction,
                      first.position.y * inverse_fraction + second.position.y * fraction,
                      first.position.z * inverse_fraction + second.position.z * fraction};
        corners[1] = {first.texture_offset.x * inverse_fraction + second.texture_offset.x * fraction,
                      first.texture_offset.y * inverse_fraction + second.texture_offset.y * fraction,
                      first.texture_offset.z * inverse_fraction + second.texture_offset.z * fraction};
        corners[2] = {first.extent.x * inverse_fraction + second.extent.x * fraction,
                      first.extent.y * inverse_fraction + second.extent.y * fraction,
                      first.extent.z * inverse_fraction + second.extent.z * fraction};
        corners[3].x = corners[0].x + corners[2].x - corners[1].x;
        corners[3].y = corners[0].y + corners[2].y - corners[1].y;
        corners[3].z = corners[0].z + corners[2].z - corners[1].z;
        for (i32 corner = 0; corner < 4; ++corner) {
            NuVecMtxTransform(&corners[corner], &corners[corner], &NuRndr_DebrisMtx);
        }

        debris_vertex_s *vertices = static_cast<debris_vertex_s *>(g_pVBData) + g_CurrentVBVertexCount + emitted;
        const u32 colour = first.colour;
        vertices[0] = {corners[0], colour, u0, v1};
        vertices[1] = {corners[1], colour, u1, v1};
        vertices[2] = {corners[2], colour, u1, v0};
        vertices[3] = vertices[0];
        vertices[4] = vertices[2];
        vertices[5] = {corners[3], colour, u0, v0};
        emitted += 6;
    }

    g_ParticleGroup->vertex_count += static_cast<i32>(emitted);
    g_FrameVertexCount += emitted;
    g_CurrentVBVertexCount += emitted;
}

void bgprocIsFreezing() {
}

void DebFreeWithoutKey(debkeydatatype_s *) {
}

void DebrisKillPlayers() {
}

void RndrUnfilledCircle(float, float, float, float, float, i32, float, float, numtl_s *) {
}

void DebrisProcessSpheres(uv1deb *, float, debinftype *, debkeydatatype_s *, i32) {
}

void DisplayListPrintItem(nudisplaylistitem_s *, i32, i32, i32 *, i32) {
}

// Debug-capture output helpers consumed by NuDisplayListCaptureSortPriority.
// Transcribed from the original C-linkage symbols:
//   NuHtmlBegin    0x2d5ca0   NuHtmlFlush   0x2d5c30
//   NuHtmlWrite    0x2d5cd0   NuHtmlHeading1 0x2d5d40
static char nudl_html_buf[0xc00]; // original bss buffer @0xb9d750-rel
static char *nudl_html_cursor;    // original @0xb9d720-rel
static char *nudl_html_end;       // original @0xb9d730-rel
static void *nudl_html_file;      // original file-handle pointer

void NuHtmlFlush(i32 force) {
    if (nudl_html_cursor > nudl_html_buf || force) {
        // HOST-ONLY: the original hands the buffer to NuFileWriteString on a
        // debug dump file; that API is not decompiled yet, so emit to stdout.
        fwrite(nudl_html_buf, 1, (usize)(nudl_html_cursor - nudl_html_buf), stdout);
        nudl_html_cursor = nudl_html_buf;
        nudl_html_end = nudl_html_buf + sizeof(nudl_html_buf);
    }
}

extern "C" void NuHtmlBegin(void *file) {
    nudl_html_file = file;
    nudl_html_cursor = nudl_html_buf;
    nudl_html_end = nudl_html_buf + sizeof(nudl_html_buf);
}

extern "C" void NuHtmlWrite(const char *text) {
    if (text == NULL || text[0] == '\0') {
        text = ""; // original substitutes an empty-string constant
    }
    // The original vsprintf's with an empty vararg list, i.e. a plain copy.
    usize len = strlen(text);
    if ((usize)(nudl_html_end - nudl_html_cursor) > len) {
        memcpy(nudl_html_cursor, text, len + 1);
        nudl_html_cursor += len;
    }
    NuHtmlFlush(0);
}

extern "C" void NuHtmlHeading1(const char *fmt, ...) {
    if (fmt == NULL || fmt[0] == '\0') {
        fmt = "";
    }
    NuHtmlWrite("<h1>");
    va_list ap;
    va_start(ap, fmt);
    char tmp[0xc00];
    vsnprintf(tmp, sizeof(tmp), fmt, ap);
    va_end(ap);
    NuHtmlWrite(tmp);
    NuHtmlWrite("</h1>");
}

void AddChunkToRenderStack(particlechunkrendertype_s *chunk, particlechunkrendertype_s **stack) {
    chunk->previous = NULL;
    chunk->next = NULL;

    particlechunkrendertype_s *current = *stack;
    if (current != NULL) {
        const u16 priority = static_cast<u16>(chunk->render_priority);
        const u16 current_priority = static_cast<u16>(current->render_priority);
        if (priority <= current_priority &&
            (priority != current_priority || current->effect->status < chunk->effect->status)) {
            particlechunkrendertype_s *next = current->next;
            while (next != NULL) {
                const u16 next_priority = static_cast<u16>(next->render_priority);
                if (next_priority <= priority &&
                    (priority != next_priority || chunk->effect->status <= next->effect->status)) {
                    break;
                }
                current = next;
                next = next->next;
            }
            current->next = chunk;
            chunk->previous = current;
            chunk->next = next;
            if (next != NULL) {
                next->previous = chunk;
            }
            return;
        }
        chunk->next = current;
        current->previous = chunk;
    }
    *stack = chunk;
}

void DebrisProcessTriggers() {
}

void ValidateShaderProgram(u32) {
}

void AddChunkControlToStack(debris_chunk_control_s *control, debris_chunk_control_s **stack) {
    debris_chunk_control_s *current = *stack;
    while (current != NULL && current->expiry_time < control->expiry_time) {
        stack = &current->next;
        current = current->next;
    }
    control->next = current;
    *stack = control;
}

extern "C" debkeydatatype_s *debris_keystack;

void AddDebrisEffectToStack(debkeydatatype_s *key) {
    if (key == NULL) {
        return;
    }
    if (debris_keystack != NULL) {
        debris_keystack->next = key;
    }
    key->previous = debris_keystack;
    debris_keystack = key;
}

extern "C" {
    extern debkeydatatype_s *debkeydata;
    extern debinftype **debtab;
    extern debris_chunk_control_s **freechunkcontrols;
    extern i32 freechunkcontrolsptr;
    extern debris_chunk_control_s *debris_chunk_control_stack[2];
    extern particlechunkrendertype_s *ParticleChunkToRender;
    extern particlechunkrendertype_s *ParticleChunkRenderStack[5];
    extern i32 debrischunks;
    extern i32 debrischunksglass;
    extern f32 globaltime;
    extern f32 panelglobaltime;
}
void DebrisGetControlStackLock(void);
void DebrisReleaseControlStackLock(void);
void RemoveChunkFromRenderStack(particlechunkrendertype_s *, particlechunkrendertype_s **);

void DebFreeChunksInstantly(i32 *handle) {
    if (handle == NULL || *handle == -1) {
        return;
    }
    debkeydatatype_s *key = &debkeydata[*handle];
    if (key->effect_index == 0 || key->allocated_chunk_count == 0) {
        return;
    }
    debinftype *effect = debtab[key->effect_index];

    DebrisGetControlStackLock();
    for (i32 i = 0; i < key->allocated_chunk_count; ++i) {
        debris_chunk_control_s *control = freechunkcontrols[freechunkcontrolsptr++];
        control->particle_chunk = key->particle_chunks[i];
        control->active = effect->particle_type == 7 ? 9 : 2;
        control->owner = NULL;
        const bool panel_time = effect->time_group == 4;
        control->expiry_time = panel_time ? panelglobaltime : globaltime;
        AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time ? 1 : 0]);
    }
    DebrisReleaseControlStackLock();

    const i32 render_chunk_count = debrischunks + debrischunksglass;
    for (i32 i = 0; i < render_chunk_count; ++i) {
        particlechunkrendertype_s *render_chunk = &ParticleChunkToRender[i];
        if (render_chunk->particle_chunk == key->particle_chunks[0]) {
            if (key->field_2f6 != 0) {
                RemoveChunkFromRenderStack(render_chunk, &ParticleChunkRenderStack[effect->time_group]);
            }
            render_chunk->particle_chunk = NULL;
            render_chunk->effect = NULL;
            render_chunk->key = NULL;
            break;
        }
    }

    for (i32 i = 0; i < key->allocated_chunk_count; ++i) {
        key->particle_chunks[i] = NULL;
    }
    key->particle_count = 0;
    key->allocated_chunk_count = 0;
    key->previous_particle_count = 0;
    key->previous_allocated_chunk_count = 0;
    key->controlled_chunk_count = 0;
    key->field_18a = 0;
}

extern "C" {
    extern debkeydatatype_s *debkeydata;
    extern debinftype **debtab;
    extern i16 *freedebkeys;
    extern i32 freedebkeyptr;
    extern dma_particle_chunk_s **freedebchunks;
    extern dma_particle_chunk_s **freedebchunksglass;
    extern i32 freedebchkptr;
    extern i32 freedebchkptrg;
    extern i32 debrischunks;
    extern i32 debrischunksglass;
    extern debris_chunk_control_s **freechunkcontrols;
    extern i32 freechunkcontrolsptr;
    extern debris_chunk_control_s *debris_chunk_control_stack[2];
    extern particlechunkrendertype_s *ParticleChunkToRender;
    extern particlechunkrendertype_s *ParticleChunkRenderStack[5];
    extern f32 globaltime;
    extern f32 panelglobaltime;
    extern f32 timeincrement;
    extern f32 debris_thinning_level;
    extern i32 forced_debris_thinning;
}

extern "C" void DebReAlloc2(debkeydatatype_s *);
extern "C" void DebReAlloc(debkeydatatype_s *, i32);
extern "C" void LinkDmaParticalSets(dma_particle_chunk_s **, i32);
void RemoveChunkFromRenderStack(particlechunkrendertype_s *, particlechunkrendertype_s **);
void DebrisReleaseControlStackLock(void);

void DebrisProcessAllocation() {
    for (debkeydatatype_s *key = debris_keystack; key != NULL; key = key->previous) {
        if (key->previous_particle_count != key->particle_count) {
            DebReAlloc2(key);
        }
    }
}

void DebrisProcessGeneration() {
    for (debkeydatatype_s *key = debris_keystack; key != NULL; key = key->previous) {
        if (key->field_1d8 != 0) {
            continue;
        }

        debinftype *effect = debtab[key->effect_index];
        const f32 now = effect->time_group == 4 ? panelglobaltime : globaltime;
        if (key->field_184 == 0 || key->field_2f4 == 0) {
            continue;
        }
        if (key->previous_allocated_chunk_count == 0) {
            const f32 thinning =
                forced_debris_thinning == 0
                    ? (debris_thinning_level <= effect->thinning ? debris_thinning_level : effect->thinning)
                    : debris_thinning_level;
            DebReAlloc(key, static_cast<i32>(static_cast<f32>(effect->max_particles) / thinning));
            if (key->previous_allocated_chunk_count == 0 || thinning == 0.0f) {
                continue;
            }
        }

        const f32 frequency = static_cast<f32>(effect->frequency);
        if (frequency <= 0.0f) {
            continue;
        }
        const f32 thinning =
            forced_debris_thinning == 0
                ? (debris_thinning_level <= effect->thinning ? debris_thinning_level : effect->thinning)
                : debris_thinning_level;
        const f32 emission_interval = 1.0f / (frequency / thinning);
        f32 emission_time = key->emission_epoch + emission_interval;
        for (i32 emission = 0; emission != 100 && emission_time < now + timeincrement; ++emission) {
            uv1deb *particle = key->generator(key, effect, emission_time);
            if (effect->process_spheres != 0 && particle != NULL && emission == 0) {
                DebrisProcessSpheres(particle, emission_time, effect, key, 0);
            }
            emission_time = key->emission_epoch + emission_interval;
        }
    }
}

void DisplayListRenderBuffer() {
}

void *RndrStateBuildKonstState(nuglobalrndrstate_s *state) {
    VARIPTR *buffer = NuDisplayListGetBuffer();
    f32 *konst = static_cast<f32 *>(buffer->void_ptr);
    f32 *result = konst;

    if (state->const_tint_enabled == 0) {
        konst[0] = 1.0f;
        konst[1] = 1.0f;
        konst[2] = 1.0f;
    } else {
        konst[0] = state->const_tint.r;
        konst[1] = state->const_tint.g;
        konst[2] = state->const_tint.b;
    }
    konst[3] = state->const_alpha_enabled == 0 ? 1.0f : state->const_alpha;
    buffer->addr += sizeof(f32) * 4;
    return result;
}

void DebrisGetControlStackLock() {
}

static particlechunkrendertype_s *FindParticleRenderChunk(dma_particle_chunk_s *particle_chunk) {
    const i32 count = debrischunks + debrischunksglass;
    for (i32 i = 0; i < count; ++i) {
        if (ParticleChunkToRender[i].particle_chunk == particle_chunk) {
            return &ParticleChunkToRender[i];
        }
    }
    return NULL;
}

static void ReleaseChunkControl(debris_chunk_control_s *control) {
    --freechunkcontrolsptr;
    freechunkcontrols[freechunkcontrolsptr] = control;
    control->particle_chunk = NULL;
}

void DebrisProcessControlChunks(i32 panel_time) {
    const f32 now = panel_time != 0 ? panelglobaltime : globaltime;
    debris_chunk_control_s **stack = &debris_chunk_control_stack[panel_time];

    DebrisGetControlStackLock();
    while (*stack != NULL && (*stack)->expiry_time <= now) {
        debris_chunk_control_s *control = *stack;
        *stack = control->next;
        control->next = NULL;

        if (control->active == 1) {
            debkeydatatype_s *key = control->owner;
            debinftype *effect = debtab[key->effect_index];
            i32 chunk_index = 0;
            while (chunk_index < key->allocated_chunk_count &&
                   key->particle_chunks[chunk_index] != control->particle_chunk) {
                ++chunk_index;
            }

            const i32 old_chunk_count = key->allocated_chunk_count;
            for (i32 i = chunk_index; i + 1 < old_chunk_count; ++i) {
                key->particle_chunks[i] = key->particle_chunks[i + 1];
            }
            key->particle_chunks[old_chunk_count - 1] = NULL;
            --key->previous_allocated_chunk_count;
            --key->controlled_chunk_count;
            --key->allocated_chunk_count;

            if (key->allocated_chunk_count == 0) {
                key->particle_count = 0;
                key->previous_particle_count = 0;
                for (i32 slot = 0; slot < 8; ++slot) {
                    const i16 key_index = effect->particle_keys[slot];
                    if (key_index != -1 && &debkeydata[key_index] == key) {
                        key->effect_index = 0;
                        key->allocation_index = -1;
                        --freedebkeyptr;
                        freedebkeys[freedebkeyptr] = key_index;
                        effect->particle_keys[slot] = -1;
                    }
                }
                key->particle_chunks[0] = NULL;
            } else {
                const i32 particles_per_chunk = effect->particle_type == 7 ? 12 : 32;
                key->particle_count -= static_cast<i16>(particles_per_chunk);
                key->previous_particle_count -= static_cast<i16>(particles_per_chunk);
                if ((chunk_index + 1) * particles_per_chunk < key->field_18a) {
                    key->field_18a -= static_cast<i16>(particles_per_chunk);
                }
                LinkDmaParticalSets(key->particle_chunks, key->allocated_chunk_count);
            }

            if (chunk_index == 0) {
                particlechunkrendertype_s *render_chunk = FindParticleRenderChunk(control->particle_chunk);
                if (render_chunk != NULL) {
                    render_chunk->particle_chunk = key->particle_chunks[0];
                    if (key->allocated_chunk_count == 0) {
                        RemoveChunkFromRenderStack(render_chunk, &ParticleChunkRenderStack[effect->time_group]);
                        render_chunk->effect = NULL;
                        render_chunk->key = NULL;
                    }
                }
            }

            control->expiry_time += 0.1f;
            control->active = effect->particle_type == 7 ? 9 : 2;
            AddChunkControlToStack(control, stack);
            continue;
        }

        if (control->active == 2 || control->active == 9) {
            LinkDmaParticalSets(&control->particle_chunk, 1);
            control->active = control->active == 9 ? 8 : 3;
            control->expiry_time += 0.1f;
            AddChunkControlToStack(control, stack);
            continue;
        }

        if (control->active == 3) {
            --freedebchkptr;
            freedebchunks[freedebchkptr] = control->particle_chunk;
            for (dma_particle_s &particle : control->particle_chunk->particles) {
                particle.start_time = 0.0f;
                particle.inverse_lifetime = 128.0f;
            }
            ReleaseChunkControl(control);
            continue;
        }

        if (control->active == 8) {
            --freedebchkptrg;
            freedebchunksglass[freedebchkptrg] = control->particle_chunk;
            for (i32 i = 0; i < 12; ++i) {
                control->particle_chunk->particles[i].start_time = 0.0f;
                control->particle_chunk->particles[i].inverse_lifetime = 128.0f;
            }
            ReleaseChunkControl(control);
            continue;
        }

        AddChunkControlToStack(control, stack);
        break;
    }
    DebrisReleaseControlStackLock();
}

// original 0x2ff660
void DisplayListCreateDynMtlList(variptr_u *buffer, variptr_u buffer_end) {
    NUDLIST_MANAGER *manager = &global_dlist_manager;
    NUDLDLISTSCENE *scene = &manager->dyn_mtl_dlist;

    NuDisplayListCreate(reinterpret_cast<nudisplayscene_s *>(scene), buffer, buffer_end, 0x400, 0x80, 0, 0, 0x80, 0, 0);
    scene->nsort_pris = 0;
    scene->name = const_cast<char *>("Dynamic Material Display Scene");

    NUDISPLAYLISTITEM *material_item = scene->items;
    for (i32 i = 0; i < 0x80; ++i) {
        NUDISPLAYLIST *display_list = scene->dlist_mtls[i];
        display_list->mtl_item = material_item;
        display_list->dyn_geom = material_item + 6;
        display_list->dlist = scene;
        display_list->mtl_id = i;
        material_item += 8;
    }

    manager->nnew_materials = 0;
    manager->ndel_materials = 0;
    manager->new_materials = reinterpret_cast<NUMTL **>(ALIGN(buffer->addr, 0x10));
    manager->del_materials = manager->new_materials + 0x80;
    manager->material_used = reinterpret_cast<u8 *>(manager->new_materials + 0x100);
    manager->mtl_buffers_used = reinterpret_cast<u8 *>(manager->new_materials + 0x120);
    buffer->addr = reinterpret_cast<usize>(manager->new_materials + 0x140);
    memset(manager->material_used, 0, 0x80);
    memset(manager->mtl_buffers_used, 0, 0x80);

    manager->mtlbuff.addr = ALIGN(buffer->addr, 0x10);
    manager->mtlbuffend.addr = manager->mtlbuff.addr + 0x4000;
    *buffer = manager->mtlbuffend;

    NUDISPLAYLIST *list = &manager->dlist_2d;
    list->first->type = 0x8d;
    list->first->id = 1;
    list->first->next = nullptr;
    list->mtl_last = list->first;
    list->state = reinterpret_cast<NURNDRSTATE *>(ALIGN(buffer->addr, 4));
    buffer->addr = reinterpret_cast<usize>(list->state + 1);
    NuDisplayListReset(list);
    scene->flags |= NUDL_SCENE_FLAG_NEEDS_BUILD;
}

void DisplayListCreateGeomItemPS(variptr_u *, void *, numtl_s *) {
}

void DebrisDrawCalculateClipBoxes(debinftype *, debkeydatatype_s *) {
}

void CalculateDistanceToNearestEnd(nuvec_s *, SOCKPOSITION_s *, i32, SOCKSYS *) {
}

void DebrisCleanUpDmaDebTypeTables() {
}

void DebrisReleaseControlStackLock() {
}

void DisplayListGenerateTransforms(nudisplayscene_s *) {
}

void RndrStateBuildReflectionState(nuglobalrndrstate_s *) {
}

void xxxNuDisplayListUpdateSpecial(nuhspecial_s *) {
}

void CalculateDistanceToNearestSide(nuvec_s *, SOCKPOSITION_s *, i32, SOCKSYS *) {
}

void DebrisFreeOldestDmaDebTypeTable() {
}

void DisplayListCreateInstSurfGeomPS(variptr_u *, numtx_s *) {
}

void CalculateDistanceToSpecificSideOrEnd(i32, nuvec_s *, SOCKPOSITION_s *, i32, SOCKSYS *) {
}

void DebrisSingleCollisionCheckScaleYFlag(i32, nuvec_s *, float, float, unsigned char) {
}

void DebrisSingleTorusCollisionCheckScaleYFlag(i32, nuvec_s *, float, float, unsigned char) {
}

void unref(unsigned char *, unsigned char *) {
}

void TBRESET() {
}

void RootFnEx(numtx_s *, void *, nuvec_s *, nuvec_s *, nuvec_s *, float, i32) {
}

void TBOPENFN(char *, i32) {
}

void RndrArrow(float, float, float, i32, i32) {
}

void TBCLOSEFN(char *, i32) {
}
