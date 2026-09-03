#include "legoapi/legoapi_types.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/characters/core/character.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

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

void DisplayListGenerateTransforms(nudisplayscene_s *) {
}

void bgprocIsFrozen() {
}

void DisplayListCreateGeomItemPS(variptr_u *, void *, numtl_s *) {
}

void DisplayListCreateInstSurfGeomPS(variptr_u *, numtx_s *) {
}

// Flag-sensitive moves from supportall.cpp (-O2): these match at the
// default flag. GetBuffer stays a call and float scheduling matches.
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
void RootFnEx(NUMTX *matrix, void *data, NUVEC *sampled_root, NUVEC *, NUVEC *translation, f32, i32 include_y) {
    APIOBJECT *object = static_cast<APIOBJECT *>(data);

    if (object->previous_animation_root_time > object->anim_packet.current_time) {
        object->previous_animation_root = *sampled_root;
    }

    object->animation_root_delta.x = sampled_root->x - object->previous_animation_root.x;
    object->animation_root_delta.y = include_y ? sampled_root->y - object->previous_animation_root.y : 0.0f;
    object->animation_root_delta.z = sampled_root->z - object->previous_animation_root.z;
    NuVecMtxRotate(&object->animation_root_delta, &object->animation_root_delta, &object->field_0xb8);
    if (!include_y) {
        object->animation_root_delta.y = 0.0f;
    }

    object->previous_animation_root = *sampled_root;
    object->previous_animation_root_time = object->anim_packet.current_time;

    if (object->anim_packet.requested_animation != -1) {
        CHARACTERANIM_s *animation = static_cast<CHARACTERANIM_s *>(
            object->character_model->model_data_a[object->anim_packet.requested_animation]);
        matrix->m30 = translation->x + animation->root_translation.x;
        matrix->m32 = translation->z + animation->root_translation.z;
        if (include_y) {
            matrix->m31 = translation->y + animation->root_translation.y;
        }
    } else {
        matrix->m30 = translation->x;
        if (include_y) {
            matrix->m31 = translation->y;
        }
        matrix->m32 = translation->z;
    }

    if (include_y && object->animation_root_delta.y == 0.0f) {
        object->animation_root_delta.y = 1.0f;
    } else if (object->animation_root_delta.x == 0.0f && object->animation_root_delta.z == 0.0f) {
        object->animation_root_delta.x = 1.0f;
    }
}
