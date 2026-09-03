#include "legoapi/world/world_shared.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/items/base/apiobject.h"
#include "decomp.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/nu3d/android/nuiosdl_gl.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "globals.h"

#include <string.h>

static f32 EvaluateDebrisCurve(const debris_float_key_s (&keys)[8], f32 time) {
    for (i32 i = 0; i < 7; ++i) {
        if (keys[i].time <= time && time <= keys[i + 1].time) {
            const f32 duration = keys[i + 1].time - keys[i].time;
            if (time == keys[i].time || duration == 0.0f) {
                return keys[i].value;
            }
            const f32 t = (time - keys[i].time) / duration;
            return keys[i].value + (keys[i + 1].value - keys[i].value) * t;
        }
    }
    return 0.0f;
}

static u8 ClampDebrisColour(f32 value) {
    value += value;
    return static_cast<u8>(value > 255.0f ? 255.0f : value);
}

static u32 EvaluateDebrisColour(const debinftype *effect, f32 time) {
    f32 red = 0.0f;
    f32 green = 0.0f;
    f32 blue = 0.0f;
    for (i32 i = 0; i < 7; ++i) {
        const debris_colour_key_s &first = effect->colour_keys[i];
        const debris_colour_key_s &second = effect->colour_keys[i + 1];
        if (first.time <= time && time <= second.time) {
            const f32 duration = second.time - first.time;
            const f32 t = time == first.time || duration == 0.0f ? 0.0f : (time - first.time) / duration;
            red = first.red + (static_cast<i32>(second.red) - first.red) * t;
            green = first.green + (static_cast<i32>(second.green) - first.green) * t;
            blue = first.blue + (static_cast<i32>(second.blue) - first.blue) * t;
            break;
        }
    }
    const u8 alpha = static_cast<u8>(EvaluateDebrisCurve(effect->alpha_keys, time));
    return static_cast<u32>(alpha) << 24 | static_cast<u32>(ClampDebrisColour(red)) |
           static_cast<u32>(ClampDebrisColour(green)) << 8 | static_cast<u32>(ClampDebrisColour(blue)) << 16;
}

struct numtl_s;
typedef struct numtl_s NUMTL;

namespace {
    struct NuVisibilityResult {
        u8 pad_00[0x18];
        i32 instance_count;
        u8 *instance_bits;
    };

    DECOMP_ASSERT(offsetof(NuVisibilityResult, instance_count) == 0x18, "visibility result instance count offset");
    DECOMP_ASSERT(offsetof(NuVisibilityResult, instance_bits) == 0x1c, "visibility result instance bits offset");
} // namespace

extern "C" {

    extern NUGLOBALRNDRSTATE render_state;
    extern nudisplayscene_s currentScene;

    void *NuVisiEvaluate(NUGSCN *scene, void *visibility_context);
    i32 NuDisplayListRndrSpecial(nuhspecial_s *special, NUMTX *matrix, i32 skinned, NUMTX *skin_matrices,
                                 DEFORMERWEIGHTSARRAY *deformer_weights);

    void AddColourPick(void) {
    }

    void BoundingBoxToLine(void) {
    }

    void BuildCamSpaceClipPlanes(void) {
    }

    void BuildWorldSpaceClipPlanes(void) {
    }

    void ChooseCorrectLOD(void) {
    }

    void *DisplayListCreateFaceonTransformPS(VARIPTR *buffer, NUMTX *transform, NUMTL *, void *faceon_data) {
        buffer->addr = ALIGN(buffer->addr, 4);
        auto *packet = static_cast<NuFaceOnTransformPacket *>(buffer->void_ptr);
        buffer->addr += sizeof(*packet);

        packet->world = *transform;
        const f32 scale_squared = NuVecMagSqr(reinterpret_cast<NUVEC *>(&transform->m00)) +
                                  NuVecMagSqr(reinterpret_cast<NUVEC *>(&transform->m10)) +
                                  NuVecMagSqr(reinterpret_cast<NUVEC *>(&transform->m20));
        packet->magnitude = NuFsqrt(scale_squared / 3.0f);

        NUVEC direction = {};
        if (*static_cast<i32 *>(faceon_data) == 0) {
            NuMtxCalcCheapFaceOn(&packet->face_on, &direction);
        } else {
            direction.x = transform->m10;
            direction.y = transform->m11;
            direction.z = transform->m12;
            NuMtxCalcCheapFaceY_v2(&packet->face_on, &direction);
        }
        return packet;
    }

    void DisplayListCreateFxItemPS(void) {
    }

    void DisplayListCreateFxList(void) {
    }

    void *DisplayListCreateGeomTransformPS(VARIPTR *buffer, NUMTX *transform, NUMTL *mtl, void *next, void *tx) {
        (void)mtl;
        (void)next;
        (void)tx;

        buffer->addr = ALIGN(buffer->addr, 4);
        NUMTX *result = static_cast<NUMTX *>(buffer->void_ptr);
        NuMtxTranspose(result, transform);
        buffer->addr += sizeof(NUMTX);
        return result;
    }

    void DisplayListCreatePS(void) {
    }

    void DisplayListCreateRigidSkin2TransformPS(void) {
    }

    void DisplayListCreateRigidSkinTransformPS(void) {
    }

    // Original 0x29b338.  Build the compact skin-palette packet consumed by
    // NuIOSDLSkinMtxCallback.  Blend-shape deltas are applied to a stream copy
    // of immediate geometry before the ordinary geometry callback runs.
    void *DisplayListCreateSkinTransformPS(VARIPTR *buffer, NUMTX *skin_matrices,
                                           DEFORMERWEIGHTSARRAY *deformer_weights, NUDISPLAYLISTGEOM *geometry,
                                           NUDISPLAYLISTGEOM **render_geometry) {
        if (deformer_weights != NULL && geometry->immediate != 0) {
            buffer->addr = ALIGN(buffer->addr, 0x20);
            NUDISPLAYLISTGEOM *copy = static_cast<NUDISPLAYLISTGEOM *>(buffer->void_ptr);
            *render_geometry = copy;
            buffer->addr += sizeof(*copy);
            *copy = *geometry;

            buffer->addr = ALIGN(buffer->addr, 0x20);
            copy->dynamic_vertex_data = buffer->void_ptr;
            const usize vertex_data_size = static_cast<usize>(geometry->vertex_stride) * geometry->vertex_count;
            buffer->addr += vertex_data_size;

            u8 *source = reinterpret_cast<u8 *>(static_cast<usize>(geometry->vertex_buffer)) +
                         static_cast<usize>(geometry->vertex_stride) * geometry->base_vertex;
            u8 *destination = static_cast<u8 *>(copy->dynamic_vertex_data);
            memmove(destination, source, vertex_data_size);

            for (i32 vertex = 0; vertex < geometry->vertex_count; ++vertex) {
                NUVEC *position = reinterpret_cast<NUVEC *>(destination);
                for (i32 deformer = 0; deformer < deformer_weights->count; ++deformer) {
                    NUVEC *offsets = geometry->deformer_vertex_offsets[deformer];
                    const f32 weight = deformer_weights->weights[deformer];
                    if (offsets != NULL && weight != 0.0f) {
                        position->x += offsets[vertex].x * weight;
                        position->y += offsets[vertex].y * weight;
                        position->z += offsets[vertex].z * weight;
                    }
                }
                destination += geometry->vertex_stride;
                source += geometry->vertex_stride;
            }
        } else {
            geometry->dynamic_vertex_data = NULL;
            *render_geometry = geometry;
        }

        i32 *matrix_count = static_cast<i32 *>(buffer->void_ptr);
        void *packet = matrix_count;
        *matrix_count = 0;
        buffer->addr += sizeof(*matrix_count);
        for (i32 i = 0; i < 8 && geometry->joint_indices[i] != 0xff; ++i) {
            NUMTX *packet_matrix = static_cast<NUMTX *>(buffer->void_ptr);
            *packet_matrix = skin_matrices[geometry->joint_indices[i]];
            buffer->addr += sizeof(NUMTX);
            ++*matrix_count;
        }
        return packet;
    }

    void DisplayListDebugPS(void) {
    }

    void DisplayListDestroyFxItemPS(void) {
    }

    void DisplayListPrintItemPS(void) {
    }

    // DisplayListSetAlphaPS is fully transcribed in nu3d/nudlist.cpp (original 0x29b8c0 / 0x29b77e).
    void DisplayListSetFxItemParamPS(void) {
    }

    void DisplayListSetShadowCasterFlagPS(NUDISPLAYLISTITEM *, NUDISPLAYLISTITEM *, i32) {
    }

    // DisplayListSwapBuffersPS is fully transcribed in nu3d/nudlist.cpp (original 0x29b8c0 / 0x29b77e).
    void DisplayListUpdateRenderStateShadow(void) {
    }

    void DisplayListUpdateSpecialTransformPS(nuhspecial_s *, NUMTX *) {
    }

    void DisplaySceneRndrSpecials(NUDLDLISTSCENE *scene, i32, void *visibility_context) {
        NuVisibilityResult *visibility =
            static_cast<NuVisibilityResult *>(NuVisiEvaluate(scene->gscene, &visibility_context));
        const bool portal_filter = visibility != NULL && portal_special_objects != 0 &&
                                   visibility->instance_count != 0 && portals_enabled != 0;
        const bool shadow_pass = currentScene.unknown_3c != NULL;

        if ((scene->instance_visibility_enabled & NUDL_SCENE_INSTANCE_VISIBILITY_ENABLED) != 0 ||
            noscenespecials != 0) {
            return;
        }

        NUGSCN temporary_scene;
        NUGSCN *gscene = scene->gscene;
        if (gscene == NULL) {
            temporary_scene.display_list = scene;
            gscene = &temporary_scene;
        }

        nuhspecial_s special_handle;
        special_handle.scene = gscene;
        for (i32 index = 0; index < scene->nspecials; ++index) {
            NUDISPLAYSPECIAL *special = &static_cast<NUDISPLAYSPECIAL *>(scene->specials)[index];
            if ((special->flags & NUDISPLAYSPECIAL_FLAG_VISIBLE) == 0) {
                continue;
            }

            const i32 instance_index = special->instance_ix;
            if (shadow_pass &&
                (static_cast<u8>(scene->visibility_flags[instance_index]) & NUDL_INSTANCE_FLAG_CASTS_SHADOW) == 0) {
                continue;
            }
            if (portal_filter &&
                (static_cast<u8>(scene->visibility_flags[instance_index]) & NUDL_INSTANCE_FLAG_NO_VISIBILITY_TEST) ==
                    0 &&
                (visibility->instance_bits[instance_index >> 3] & (1U << (instance_index & 7))) == 0) {
                continue;
            }

            special_handle.display_special = special;
            NUMTX *draw_matrix = special->draw_mtx_ptr;
            if (draw_matrix == NULL || draw_matrix == reinterpret_cast<NUMTX *>(-1)) {
                draw_matrix = &special->draw_mtx;
            }
            NuDisplayListRndrSpecial(&special_handle, draw_matrix, 0, NULL, NULL);
        }
    }

    void FmvTimePS(void) {
    }

    void GenericDebinfoDmaTypeUpdate(debinftype *effect) {
        extern PartHeader **DmaDebTypes;
        extern i32 EDPP_MAX_DMADEBTYPES;
        extern i32 freeDmaDebType;

        if (effect == NULL) {
            return;
        }
        if (effect->native_data == NULL) {
            if (freeDmaDebType >= EDPP_MAX_DMADEBTYPES) {
                return;
            }
            effect->native_data = DmaDebTypes[freeDmaDebType++];
        }

        PartHeader *header = effect->native_data;
        header->gravity = effect->field_0a0;
        header->texture_u0 = static_cast<f32>(static_cast<i32>(effect->texture_u0) & 0x1ff) / 255.0f;
        header->texture_v0 = static_cast<f32>(static_cast<i32>(effect->texture_v0) & 0x1ff) / 255.0f;
        header->texture_u1 = static_cast<f32>(static_cast<i32>(effect->texture_u1) & 0x1ff) / 255.0f;
        header->texture_v1 = static_cast<f32>(static_cast<i32>(effect->texture_v1) & 0x1ff) / 255.0f;

        for (i32 frame_index = 0; frame_index < 64; ++frame_index) {
            const f32 time = static_cast<f32>(frame_index) / 64.0f;
            const f32 width = EvaluateDebrisCurve(effect->width_keys, time);
            const f32 height = EvaluateDebrisCurve(effect->height_keys, time);
            const f32 rotation = EvaluateDebrisCurve(effect->rotation_keys, time);
            const f32 sine = NU_SIN_LUT(rotation);
            const f32 cosine = NU_COS_LUT(rotation);
            const f32 wave_x = effect->field_0b4 * NU_SIN_LUT(effect->field_0b0 * time * 65536.0f);
            const f32 wave_y = effect->field_0bc * NU_SIN_LUT(effect->field_0b8 * time * 65536.0f);

            debris_particle_frame_s &frame = header->frames[frame_index];
            frame.position.x = (-cosine * (width * 0.25f) - sine * (height * 0.25f) + wave_x) / 2048.0f;
            frame.position.y = (sine * (width * 0.25f) - cosine * (height * 0.25f) + wave_y) / 2048.0f;
            frame.position.z = 0.0f;
            frame.texture_offset.x = (cosine * (width * 0.25f) - sine * (height * 0.25f) + wave_x) / 2048.0f;
            frame.texture_offset.y = (-sine * (width * 0.25f) - cosine * (height * 0.25f) + wave_y) / 2048.0f;
            frame.texture_offset.z = 0.0f;
            frame.extent.x = (cosine * (width * 0.25f) + sine * (height * 0.25f) + wave_x) / 2048.0f;
            frame.extent.y = (-sine * (width * 0.25f) + cosine * (height * 0.25f) + wave_y) / 2048.0f;
            frame.extent.z = 0.0f;
            frame.colour = EvaluateDebrisColour(effect, time);
        }
    }

    void Initialise_PS(NUGSCN *scene) {
        scene->instance_visibility_flags = PortalVisiFlags;
    }

    void LinkDmaParticalSets(dma_particle_chunk_s **chunks, i32 count) {
        dma_particle_chunk_s *chunk = chunks[count - 1];
        chunk->command = 0x52;
        chunk->next = NULL;
        for (i32 i = count - 2; i >= 0; --i) {
            chunk = chunks[i];
            chunk->command = 0x4e;
            chunk->next = chunks[i + 1];
        }
    }

    void LocaledbitsDrawCircleXY(void) {
    }

    void LocaledbitsDrawSolidCircleXY(void) {
    }

    void LocaledbitsDrawSolidEllipseXY(void) {
    }

    // Original @0x3cd56e. Build the hierarchy render-part list selected by
    // the caller's bit mask.
    i32 MakeLayerList_Index(CHARACTERMODEL_s *model, i16 *layers, u32 mask) {
        if (model == NULL) {
            return 0;
        }

        i32 count = 0;
        u32 layer_bit = 1;
        for (i32 layer = 0; layer <= 31 && layer < model->hierarchy->render_count; ++layer) {
            if ((mask & layer_bit) != 0) {
                *layers++ = static_cast<i16>(layer);
                ++count;
            }
            layer_bit <<= 1;
        }
        return count;
    }

    void PerspectMidPoint(NUVEC *result, NUVEC *first, NUVEC *second, NUVEC *camera_position) {
        f32 first_distance = NuVecDist(camera_position, first, NULL);
        f32 second_distance = NuVecDist(camera_position, second, NULL);
        f32 ratio = first_distance / (first_distance + second_distance);
        result->x = first->x + (second->x - first->x) * ratio;
        result->y = first->y + (second->y - first->y) * ratio;
        result->z = first->z + (second->z - first->z) * ratio;
    }

    void RndrCircleXZ(void) {
    }

    void RndrMaskScreen(void) {
    }

    void RndrOSphere(void) {
    }

    void RndrOSquare(void) {
    }

    void RndrStateBuildFogState(void) {
    }

    void RndrStateBuildLightState(void) {
    }

    void RndrStateBuildVertexGroupsStates(void) {
    }

    void RndrStateBuildVertexOffsetsStates(void) {
    }

    static void RndrStateClear(NUGLOBALRNDRSTATE *state) {
        state->state.mtl = NULL;
        state->state.tex_id = -1;
        state->state.global_id = -1;
        state->state.lights_id = -1;
        state->state.camera_id = -1;
        state->state.fog_id = -1;
        state->state.konst_id = -1;
        state->state.reflection_id = -1;
    }

    void RndrStateCopyGlobalState(NUGLOBALRNDRSTATE *state) {
        memcpy(state, &render_state, sizeof(*state));
        state->fog_state = NULL;
        state->camera_state = NULL;
        state->light_state = NULL;
        state->konst_state = NULL;
    }

    void RndrStateResetGlobalState(NUGLOBALRNDRSTATE *state) {
        state->const_alpha_enabled = 0;
        state->const_tint_enabled = 0;
        state->fog_state = NULL;
        state->light_state = NULL;
        state->camera_state = NULL;
        state->konst_state = NULL;
        state->reflection_state = NULL;
        render_state.state.global_id = 0;
        render_state.state.lights_id = 0;
        render_state.state.camera_id = 0;
        render_state.state.fog_id = 0;
        render_state.state.konst_id = 0;
        RndrStateClear(&render_state);
    }

    void RndrStateResetSharedGlobalState(void) {
        RndrStateResetGlobalState(&render_state);
    }

    void RndrStateSetConstAlphaTint(i32 alpha_enabled, i32 tint_enabled, f32 alpha, const NUCOLOUR3 *tint, NUMTL *mtl) {
        render_state.const_alpha_mtl = mtl;
        render_state.const_alpha_enabled = alpha_enabled;
        render_state.const_alpha = alpha;
        if (tint != NULL && tint_enabled != 0) {
            render_state.const_tint = *tint;
        }
        render_state.const_tint_enabled = tint_enabled;
        render_state.konst_state = NULL;
        render_state.state.global_id++;
        render_state.state.konst_id++;
    }

    void RndrStateSetReflection(void) {
    }

    void RndrStateUpdate(void *, NUMTL *, NUDISPLAYLISTITEM *) {
    }

    void RndrStateUpdateFx(void *, NUDISPLAYLISTITEM *) {
    }

    void SetAiRndrCullDistance(void) {
    }

    void SetAllInstancesVisible(void) {
        memset(PortalVisiFlags, 0xff, sizeof(PortalVisiFlags));
    }

    void ShadowDir(void) {
    }

    void ShadowInfo(void) {
    }

    void ShadowIntensityInfo(void) {
    }

    void ShadowRoofInfo(void) {
    }

    void SphereDraw(void) {
    }

    void SphereDrawEx(void) {
    }

    void clipRoomAgainstFrustrum(void) {
    }

    void clipTestBox(void) {
    }

    void clipTestSphere(void) {
    }

    void glDeleteVertexArraysOESC(void) {
    }

    void glGenVertexArraysOESC(void) {
    }

} // extern "C"
