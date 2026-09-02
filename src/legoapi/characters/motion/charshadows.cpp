#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nu3d/nurndr.h"

f32 GameShadow(GameObject_s *object, NUVEC *position, f32 probe_height, i32 terrain_mask);
f32 DropInOutScale(GameObject_s *object);
void FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation);
extern "C" i32 EShadowInfo();

namespace {
    constexpr f32 kInvalidShadowHeight = 2000000.0f;
    constexpr i32 kMaxCharacterShadows = 5;
    constexpr i32 kMaxShadowJoints = 16;
} // namespace

void CharShadows_Draw() {
    if (CHARSHADOWS_ON == 0) {
        return;
    }

    GameObject_s *object = Obj;
    for (i32 object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
        const u16 required_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER;
        if ((object->apiobj.field_0x1f8 & required_flags) != required_flags || object->apiobj.field_0x287 != 0) {
            continue;
        }

        if ((object->field_0xe20 & GAMEOBJECT_E20_FLAG_MOVEMENT_DISABLED) != 0 &&
            (CInfo[object->character_context].flags & CHARACTER_CONTEXT_INFO_FLAG_ALLOW_DISABLED_MOVEMENT_SHADOW) ==
                0) {
            continue;
        }
        if ((object->apiobj.character_data->model_flags & CHARACTER_MODEL_FLAG_DISABLE_BLOB_SHADOW) != 0 ||
            object->shadow_opacity <= 0.0f || object->apiobj.model_draw_result == 0 || object->shadow_joint_mask == 0 ||
            (object->field_0xeff & 4) != 0 ||
            (CInfo[object->character_context].flags & CHARACTER_CONTEXT_INFO_FLAG_DISABLE_BLOB_SHADOW) != 0) {
            continue;
        }

        PLAYERCHARACTERCONFIG_s *config = object->apiobj.character_data->player_config;
        i32 alpha = config->blob_shadow_alpha;
        if (alpha == 0xff) {
            alpha = WORLD->current_level->blob_shadow_alpha;
        }
        if (alpha <= 0 || object->shadow_radius <= 0.0f) {
            continue;
        }

        const f32 opacity_scale = object->shadow_opacity * static_cast<f32>(alpha);
        const f32 radius = object->shadow_radius * object->apiobj.field_0xa8 * DropInOutScale(object);
        i32 shadow_index = 0;
        NUMTX *joint_matrix = object->joint_matrices;
        for (i32 joint_index = 0; joint_index < kMaxShadowJoints && shadow_index < kMaxCharacterShadows;
             ++joint_index, ++joint_matrix) {
            if ((object->shadow_joint_mask & (1u << joint_index)) == 0 ||
                object->apiobj.character_model->points_of_interest[joint_index] == NULL) {
                continue;
            }

            CHARACTER_SHADOW_s &shadow = object->character_shadows[shadow_index];
            if (shadow.position.y != kInvalidShadowHeight && shadow.opacity > 0.0f &&
                shadow.position.y <= joint_matrix->m31 + 0.025f) {
                NUVEC position = shadow.position;
                position.y += 0.005f;
                NuRndrAddShadow(&position, radius, static_cast<i32>(shadow.opacity * opacity_scale), shadow.x_rotation,
                                0, shadow.z_rotation);
            }
            ++shadow_index;
        }
    }
}

void CharShadows_Reset(PLAYERPACKET_s *packet) {
    packet->character_shadows[0].position.y = 2000000.0f;
    packet->character_shadows[1].position.y = 2000000.0f;
    packet->character_shadows[2].position.y = 2000000.0f;
    packet->character_shadows[3].position.y = 2000000.0f;
    packet->character_shadows[4].position.y = 2000000.0f;
}

void CharShadows_Update() {
    GameObject_s *object = Obj;
    for (i32 object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
        const u16 required_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER;
        if ((object->apiobj.field_0x1f8 & required_flags) != required_flags || object->apiobj.field_0x287 != 0 ||
            (object->apiobj.character_data->model_flags & CHARACTER_MODEL_FLAG_DISABLE_BLOB_SHADOW) != 0 ||
            object->use_model_origin == 0) {
            continue;
        }

        PLAYERCHARACTERCONFIG_s *config = object->apiobj.character_data->player_config;
        const u16 shadow_joint_mask = config->shadow_joint_mask;
        if (shadow_joint_mask == 0 || object->apiobj.model_draw_result == 0 ||
            (CInfo[object->character_context].flags & CHARACTER_CONTEXT_INFO_FLAG_DISABLE_BLOB_SHADOW) != 0 ||
            object->shadow_opacity <= 0.0f) {
            continue;
        }

        if (g_lowEndLevelBehaviour != 0 && (object->apiobj.flags_low & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) {
            object->shadow_joint_mask = 0;
            continue;
        }

        object->shadow_joint_mask = shadow_joint_mask;
        object->shadow_radius =
            config->shadow_radius < 99.0f ? config->shadow_radius : object->apiobj.character_data->collision_radius;

        i32 shadow_index = 0;
        NUMTX *joint_matrix = object->joint_matrices;
        for (i32 joint_index = 0; joint_index < kMaxShadowJoints && shadow_index < kMaxCharacterShadows;
             ++joint_index, ++joint_matrix) {
            if ((shadow_joint_mask & (1u << joint_index)) == 0 ||
                object->apiobj.character_model->points_of_interest[joint_index] == NULL) {
                continue;
            }

            CHARACTER_SHADOW_s &shadow = object->character_shadows[shadow_index];
            const NUVEC &joint_position = *NUMTX_GET_ROW_VEC(joint_matrix, 3);
            shadow.position.x = joint_position.x;
            shadow.position.z = joint_position.z;

            if ((object->apiobj.field_0x1f8 & APIOBJECT_FLAG_SHADOW_AT_OBJECT_HEIGHT) != 0) {
                shadow.position.y = object->apiobj.field_0x218;
                shadow.x_rotation = 0;
                shadow.z_rotation = 0;
                shadow.opacity = 1.0f;
                ++shadow_index;
                continue;
            }

            NUVEC query_position = joint_position;
            shadow.position.y = GameShadow(object, &query_position, 5.0f, -1);
            if (shadow.position.y == kInvalidShadowHeight) {
                ++shadow_index;
                continue;
            }

            const i32 layer_index = EShadowInfo();
            if (layer_index >= 0 && EShadY != kInvalidShadowHeight && layer_index <= 16 &&
                (TerLayer[layer_index].flags & TERRAIN_LAYER_FLAG_REJECT_CHARACTER_SHADOW) != 0 &&
                (EShadY > shadow.position.y || shadow.position.y > joint_position.y + 0.1f)) {
                shadow.position.y = kInvalidShadowHeight;
                ++shadow_index;
                continue;
            }

            shadow.opacity = ShadNorm.y;
            FindAnglesZX(&ShadNorm, NULL, NULL);
            shadow.x_rotation = temp_xrot;
            shadow.z_rotation = temp_zrot;
            ++shadow_index;
        }

        for (; shadow_index < kMaxCharacterShadows; ++shadow_index) {
            object->character_shadows[shadow_index].position.y = kInvalidShadowHeight;
        }
    }
}
