#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nucore/nugcutscene.h"

float CalcValue1648(char *, i32, i32, float, ani3_scalemin_s *);

static const u8 KeyStructSizes[16] = {3, 4, 4, 3, 4, 3, 4, 8, 4, 8, 4, 0, 0, 0, 0, 0};

void Animate_POD(GameObject_s *) {
}

void Animate_ATAT(GameObject_s *) {
}

void Animate_JEDI(GameObject_s *) {
}

void AnimatePlayer(GameObject_s *) {
}

void Animate_BEAST(GameObject_s *) {
}

void Animate_BARMAN(GameObject_s *) {
}

void Animate_CANNON(GameObject_s *) {
}

void Animate_WALKER(GameObject_s *) {
}

void Animate_WEIRDO(GameObject_s *) {
}

void Animate_CRITTER(GameObject_s *) {
}

void Animate_DEFAULT(GameObject_s *) {
}

void Animate_VEHICLE(GameObject_s *) {
}

void Animate_DROIDEKA(GameObject_s *) {
}

void Animate_PROTOCOL(GameObject_s *) {
}

void GameAnimSet_Draw(GAMEANIMSET_s &) {
}

void GameAnimSet_Play(GAMEANIMSET_s *, float, i32) {
}

void GameAnimSet_Stop(GAMEANIMSET_s *) {
}

void Animate_ASTROMECH(GameObject_s *) {
}

void Animate_CHARACTER(GameObject_s *) {
}

void Animate_GEONOSIAN(GameObject_s *) {
}

void GameAnimSet_RemoveFromSystemList(GAMEANIMSET_s *);

i32 GameAnimSet_Reset(GAMEANIMSET_s *set) {
    GAMEANIMOBJ_s *object;
    GAMEANIMINSTANCE_s *instance;

    if (set != NULL) {
        object = set->objects;
        if (object != NULL) {
            do {
                instance = object->instance_animation;
                if (instance != NULL) {
                    instance->flags &= 0xfa;
                    const f32 start_frame = object->start_frame;
                    instance->playback_scale = 1.0f;
                    instance->start_frame = start_frame;
                }
                GameAnimSet_RemoveFromSystemList(set);
                object = object->next;
            } while (object != NULL);
        }
    }
    return 1;
}

void Animate_HOVERDROID(GameObject_s *) {
}

void GameAnimSet_Create(variptr_u *, variptr_u *, GAMEANIMOBJPOOL_s *, GAMEANIMSYS_s *) {
}

void Animate_BATTLEDROID(GameObject_s *) {
}

void Animate_SPEEDERBIKE(GameObject_s *) {
}

void GameAnimSet_Playing(GAMEANIMSET_s *) {
}

void GameAnimSet_EvalAnim(GAMEANIMSET_s *) {
}

void GameAnimSet_AddObject(GAMEANIMSET_s *, nuhspecial_s *, float, float, i32) {
}

void GameAnimSet_JumpToEnd(GAMEANIMSET_s *) {
}

void GameAnimSet_SetOffset(GAMEANIMSET_s *, nuvec_s *) {
}

void GameAnimSet_GetAnimPos(GAMEANIMOBJ_s *) {
}

void GameAnimSet_SetAnimPos(GAMEANIMOBJ_s *, float) {
}

void GameAnimSet_SetTFactor(GAMEANIMSET_s *, float) {
}

void Animate_REPUBLICGUNSHIP(GameObject_s *) {
}

void GameAnimSet_JumpToStart(GAMEANIMSET_s *) {
}

void Animate_SUPERBATTLEDROID(GameObject_s *) {
}

void GameAnimSet_RemoveObject(GAMEANIMSET_s *, GAMEANIMOBJ_s *) {
}

void GameAnimSet_ScaleFParam1(GAMEANIMSET_s *, float) {
}

void GameAnimSet_SetRepeating(GAMEANIMSET_s *, i32) {
}

void GameAnimSet_EvaluateState(GAMEANIMSET_s *) {
}

void GameAnimSet_GetAveragePos(GAMEANIMSET_s *, nuvec_s *, i32, i32, i32) {
}

void GameAnimSet_GetVisibility(GAMEANIMSET_s *) {
}

void GameAnimSet_JumpToAnimPos(GAMEANIMSET_s *, float) {
}

void GameAnimSet_RemoveSpecial(GAMEANIMSET_s *, nuhspecial_s *) {
}

void GameAnimSet_SetVisibility(GAMEANIMSET_s *, i32) {
}

void GameAnimSet_DrawReflection(GAMEANIMSET_s *, i32, float, numtx_s *) {
}

void GameAnimSet_AddObjectByName(GAMEANIMSET_s *, nugscn_s *, char *, float, float, i32, GIZMOSYS_s *, char *, char *) {
}

void GameAnimSet_AddToSystemList(GAMEANIMSET_s *) {
}

void GameAnimSet_AutoSetReflectY(GAMEANIMSET_s *, nuvec_s *, numtx_s *) {
}

void GameAnimSet_GetCurrentFrame(GAMEANIMSET_s *) {
}

void GameAnimSet_CreateObjectPool(variptr_u *, variptr_u *, i32, i32) {
}

void GameAnimSet_IsAnimationReset(GAMEANIMSET_s *) {
}

void GameAnimSet_RemoveAllObjects(GAMEANIMSET_s *) {
}

void GameAnimSet_GetCentreAndRadius(GAMEANIMSET_s *, nuvec_s *, float *, i32, i32, i32) {
}

void GameAnimSet_GetCompletionRatio(GAMEANIMSET_s *) {
}

void GameAnimSet_RemoveFromSystemList(GAMEANIMSET_s *) {
}

static __used__ i32 LoadAnim(char *, i32, variptr_u *, variptr_u) {
    return 0;
}
static __used__ i32 LoadAnimFromPAK(char *, i32, char *, i32) {
    return 0;
}
static __used__ void NormalizeAnimPath(char *) {
}

extern "C" {

    void ANI_Ani3ExtractAllNodeCurves(ani3_animheader_s *anim, float frame, float *values, i32 node, char *curve_mask) {
        u32 curve_count = anim->curve_count;
        u32 quarter;
        u32 stride = anim->key_stride;
        float fraction;
        i32 key_offset;

        if (ForcePlayEndFrame == 0 || anim->end_frame == 0) {
            if (anim->key_count == 1) {
                quarter = 0;
                fraction = 0.0f;
                key_offset = 0;
            } else {
                float last_key = static_cast<float>(anim->key_count - 1);
                float key = (frame - static_cast<float>(anim->first_frame)) * last_key /
                            static_cast<float>(anim->frame_count - 1);
                if (key < 0.0f) {
                    key = 0.0f;
                }
                if (last_key <= key) {
                    key = last_key;
                }
                i32 whole_key = static_cast<i32>(key);
                fraction = key - static_cast<float>(whole_key);
                quarter = static_cast<u32>(whole_key) & 3;
                key_offset = (whole_key >> 2) * stride;
            }
        } else {
            float key = static_cast<float>(anim->end_frame + anim->key_count - 4);
            i32 whole_key = static_cast<i32>(key);
            fraction = key - static_cast<float>(whole_key);
            quarter = static_cast<u32>(whole_key) & 3;
            key_offset = (whole_key >> 2) * stride;
        }

        u16 *types = anim->curve_types;
        u8 *force_zero = reinterpret_cast<u8 *>(types + anim->node_count * curve_count);
        ani3_scalemin_s *scale_min = anim->scale_min;
        i16 *constants = anim->constants;
        u8 *keys = anim->keys + key_offset;

        for (i32 n = 0; n < node; ++n) {
            for (u32 curve = 0; curve < curve_count; ++curve) {
                u16 type = *types++;
                if (type < 16) {
                    ++scale_min;
                    keys += KeyStructSizes[type];
                }
            }
            force_zero += curve_count;
        }

        i32 quarter_shift = static_cast<i32>(quarter) * 6;
        i32 next_quarter_shift = (static_cast<i32>(quarter) * 3 + 3) * 2;
        for (u32 curve = 0; curve < curve_count; ++curve, ++values) {
            u16 type = types[curve];
            bool evaluate = curve_mask == NULL || *curve_mask == static_cast<char>(curve);
            if (curve_mask != NULL && evaluate) {
                ++curve_mask;
            }
            if (!evaluate) {
                if (type == 7) {
                    keys += 8;
                    ++scale_min;
                } else if (type == 6) {
                    keys += 4;
                    ++scale_min;
                } else if (type == 8 || type == 10) {
                    keys += 4;
                }
                continue;
            }

            float key_fraction = (force_zero[curve] & 1) != 0 ? 0.0f : fraction;
            if (type == 7) {
                *values = CalcValue1648(reinterpret_cast<char *>(keys), quarter, stride, key_fraction, scale_min);
                keys += 8;
                ++scale_min;
            } else if (type == 8) {
                *values = static_cast<float>(constants[keys[quarter]]);
                keys += 4;
            } else if (type == 10) {
                u32 index = keys[quarter];
                i32 packed = static_cast<i32>(constants[index + 1]) | (static_cast<i32>(constants[index]) << 16);
                *values = static_cast<float>(packed);
                keys += 4;
            } else if (type == 6) {
                u32 first = *reinterpret_cast<u32 *>(keys);
                u32 next = *reinterpret_cast<u32 *>(keys + stride);
                float first_value = static_cast<float>(first & 0xff);
                float next_value = static_cast<float>(next & 0xff);
                u32 tangents = first >> 8;
                float tangent0 = static_cast<float>((tangents >> quarter_shift) & 0x3f) * 0.01587302f;
                float packed_value;
                if (quarter == 3) {
                    float interpolated = (next_value - first_value) * tangent0 + first_value;
                    float tangent1 = static_cast<float>((next >> 8) & 0x3f) * 0.01587302f;
                    float after = static_cast<float>(keys[stride * 2]);
                    packed_value =
                        (((after - next_value) * tangent1 + next_value) - interpolated) * key_fraction + interpolated;
                } else {
                    float tangent1 = static_cast<float>((tangents >> (next_quarter_shift & 0x1f)) & 0x3f) * 0.01587302f;
                    packed_value =
                        (next_value - first_value) * ((tangent1 - tangent0) * key_fraction + tangent0) + first_value;
                }
                *values = packed_value * scale_min->scale + scale_min->minimum;
                keys += 4;
                ++scale_min;
            } else {
                u16 constant = reinterpret_cast<u16 *>(constants)[anim->constant_index + type - 16];
                *values = static_cast<float>(constant) * anim->scale + anim->minimum;
            }
        }
    }

    void ANI_FixUpAddrs(ani3_animheader_s *anim, i32 delta) {
        if (anim->magic != 0x414e4934) {
            return;
        }
        while (true) {
            i32 *words = reinterpret_cast<i32 *>(anim);
            static const u8 pointer_words[] = {10, 9, 12, 11, 13, 14};
            for (u32 i = 0; i < sizeof(pointer_words); ++i) {
                if (words[pointer_words[i]] != 0) {
                    words[pointer_words[i]] += delta;
                }
            }
            u16 next = anim->next_block;
            if (next == 0) {
                break;
            }
            delta += next;
            anim = reinterpret_cast<ani3_animheader_s *>(reinterpret_cast<u8 *>(anim) + next);
        }
    }

    // Original @0x2c17d0.  The non-quaternion ANI4 player decodes the three
    // vector curve groups directly into the animation buffer.
    void ANI_SimpleAni3PlayerV4Joint(ani3_animheader_s *anim, f32 frame, nuanimbuff_s *buffer, i32 first_joint,
                                     i32 joint_count) {
        buffer->use_quaternions = 0;

        i32 end_joint = first_joint + joint_count;
        if (end_joint > anim->node_count) {
            end_joint = anim->node_count;
        }

        for (i32 joint_index = first_joint; joint_index < end_joint; ++joint_index) {
            const u8 flags = anim->node_flags[joint_index];
            buffer->joint_flags[joint_index] = flags;

            f32 curves[9] = {};
            ANI_Ani3ExtractAllNodeCurves(anim, frame, curves, joint_index, NULL);

            nuanimbuffjoint_s &joint = buffer->joints[joint_index];
            if ((flags & 2) != 0) {
                joint.translation = {curves[0], curves[1], curves[2]};
            } else {
                joint.translation = {0.0f, 0.0f, 0.0f};
            }
            joint.translation_w = 0.0f;

            if ((flags & 1) != 0) {
                joint.rotation = {curves[3], curves[4], curves[5]};
            } else {
                joint.rotation = {0.0f, 0.0f, 0.0f};
            }
            joint.rotation_w = 0.0f;

            if ((flags & 8) != 0) {
                joint.scale = {curves[6], curves[7], curves[8]};
            } else {
                joint.scale = {1.0f, 1.0f, 1.0f};
            }
            joint.scale_w = 0.0f;
        }
    }

    void ANI_SimpleAni3PlayerV4Joint_Blend(ani3_animheader_s *anim, f32 frame, nuanimbuff_s *buffer, f32 blend,
                                           i32 first_joint, i32 joint_count, NUVEC *root_translation) {
        nuanimbuffjoint_s target_joints[256];
        u8 target_flags[256];
        nuanimbuff_s target = {
            buffer->joint_count, buffer->max_joints, 0, 0, target_joints, target_flags,
        };
        ANI_SimpleAni3PlayerV4Joint(anim, frame, &target, first_joint, joint_count);

        i32 end_joint = first_joint + joint_count;
        if (end_joint > anim->node_count) {
            end_joint = anim->node_count;
        }
        for (i32 joint_index = first_joint; joint_index < end_joint; ++joint_index) {
            nuanimbuffjoint_s &joint = buffer->joints[joint_index];
            const nuanimbuffjoint_s &target_joint = target.joints[joint_index];
            joint.translation.x += (target_joint.translation.x - joint.translation.x) * blend;
            joint.translation.y += (target_joint.translation.y - joint.translation.y) * blend;
            joint.translation.z += (target_joint.translation.z - joint.translation.z) * blend;
            joint.rotation.x += (target_joint.rotation.x - joint.rotation.x) * blend;
            joint.rotation.y += (target_joint.rotation.y - joint.rotation.y) * blend;
            joint.rotation.z += (target_joint.rotation.z - joint.rotation.z) * blend;
            joint.scale.x += (target_joint.scale.x - joint.scale.x) * blend;
            joint.scale.y += (target_joint.scale.y - joint.scale.y) * blend;
            joint.scale.z += (target_joint.scale.z - joint.scale.z) * blend;
            buffer->joint_flags[joint_index] |= target.joint_flags[joint_index];
        }
        if (root_translation != NULL && end_joint != first_joint) {
            *root_translation = target.joints[first_joint].translation;
        }
    }

    void ANI_SimpleAni3PlayerV4Joint_Blend_EulerQuat(void) {
    }

    void ANI_SimpleAni3PlayerV4Joint_EulerQuat(void) {
    }

    void AddAnimEffects(void) {
    }

    void AnimBlendingFromTo(void) {
    }

    void AnimDuration(void) {
    }

    float AnimEndFrame(void *, i32) {
        return 0.0f;
    }

    void AnimListFrame(void) {
    }

    void AnimListFrameArray(void) {
    }

    void AnimList_NoLoad(void) {
    }

    void AnimList_RequestAnimGroupForCreatures(void) {
    }

    void AnimList_RequestAnimGroups(void) {
    }

    void AnimMiscFlags(void) {
    }

    void AnimPacket_FullToMini(void) {
    }

    void AnimPacket_MiniToFull(void) {
    }

    void AnimPlaying(void) {
    }

    void AnimSpeed(void) {
    }

    void AnimStopFrame(void) {
    }

    void AnimsAvailableToBothCharacters(void) {
    }

    void BlendRootFn(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32) {
    }

    void BlendTimeBetweenAnims(void) {
    }

    void CurrentAnim(void) {
    }

    void EvalModelAnim(void) {
    }

    void FindAnimIX(void) {
    }

    void GetAnimBlendMode(void) {
    }

    void GetAnimTimeRandom(void) {
    }

    void GetInstAnimEndFrame(void) {
    }

    extern i32 AnimBlendMode __asm__("_ZL13AnimBlendMode") __attribute__((visibility("hidden")));

    void ResetAnimPacket(ANIMPACKET_s *packet, i32 animation) {
        if (packet != NULL) {
            packet->field_0x3a = static_cast<i16>(animation);
            packet->field_0x38 = packet->field_0x3a;
            packet->animation_index = packet->field_0x38;
            packet->animation_mode = AnimBlendMode;
            packet->previous_animation_mode = packet->animation_mode;
            packet->current_animation = packet->previous_animation_mode;
            packet->blending = 0;
            packet->field_0x30 = 4;
            packet->frame = -1;
            packet->field_0x3e = 0;
            packet->reset_flags = 0;
            packet->field_0x3d = 0;
        }
    }

#ifdef __EMSCRIPTEN__
    void ResetMiniAnimPacket(void *, i32) {
    }
#else
    void ResetMiniAnimPacket(MINIANIMPACKET_s *packet, i32 animation) {
        if (packet != NULL) {
            packet->requested_animation_id = static_cast<i16>(animation);
            packet->previous_animation_id = packet->requested_animation_id;
            packet->current_animation_id = packet->previous_animation_id;
            packet->animation_mode = AnimBlendMode;
            packet->previous_animation_mode = packet->animation_mode;
            packet->current_animation = packet->previous_animation_mode;
            packet->field_0x19 = 0;
            packet->reset_state = 4;
        }
    }
#endif

    void RootFn(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32) {
    }

    void RootFnY(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32) {
    }

    void SetActionInfo(void *action_info, void *extra_action_data) {
        (void)action_info;
        (void)extra_action_data;
    }

    void SetAnimBlendMode(void) {
    }

    void SetAnimTimeRandom(void) {
    }

    void SetProceduralAnimationFn(void *animbuff) {
        (void)animbuff;
    }

    i32 StateAnimEvaluate(StateAnim *state, u8 *index, u8 *value, f32 frame) {
        u8 next = *index;
        if (next < state->count) {
            bool changed = false;
            do {
                if (frame < state->times[next]) {
                    if (changed) {
                        return 1;
                    }
                    break;
                }
                changed = true;
                *value = state->values[next];
                next = static_cast<u8>(*index + 1);
                *index = next;
            } while (next < state->count);
            if (next >= state->count) {
                return 1;
            }
        }

        i32 changed = 0;
        if (next == 0) {
            return 0;
        }
        do {
            if (state->times[next - 1] <= frame) {
                return changed;
            }
            next--;
            *index = next;
            *value = next == 0 ? state->values[0] : state->values[next - 1];
            changed = 1;
        } while (next != 0);
        return 1;
    }

    bool StateAnimEvaluate2(StateAnim *state, u8 *index, char *value, f32 frame) {
        u32 current = *index;
        u32 count = state->count;
        if (current >= count) {
            current = count - 1;
        }
        char old_value = state->values[current];
        u32 selected;
        if (frame < state->times[current]) {
            selected = current;
            while (selected != 0 && frame < state->times[selected - 1]) {
                --selected;
            }
        } else {
            selected = current;
            while (selected + 1 < count && state->times[selected + 1] <= frame) {
                ++selected;
            }
        }
        char new_value = state->values[selected];
        *value = new_value;
        *index = static_cast<u8>(selected);
        return old_value != new_value;
    }

    StateAnim *StateAnimFixPtrs(StateAnim *state, i32 delta) {
        if (state == NULL) {
            return NULL;
        }
        state = reinterpret_cast<StateAnim *>(reinterpret_cast<u8 *>(state) + delta);
        if (state->times != NULL) {
            state->times = reinterpret_cast<f32 *>(reinterpret_cast<u8 *>(state->times) + delta);
        }
        if (state->values != NULL) {
            state->values += delta;
        }
        return state;
    }

    void UpdateAnimPacket(void) {
    }

    void UpdateMiniAnimPacket(void) {
    }

} // extern "C"

void SetAnimFrame(nuhspecial_s *, float) {
}

struct DefaultIdleCharacterData {
    u8 pad[0x116];
    u8 use_standard_idle;
};

i32 GetDefaultIdle(GameObject_s *obj) {
    CHARACTERDATA *character = obj->apiobj.character_data;
    DefaultIdleCharacterData *game_character = static_cast<DefaultIdleCharacterData *>(character->field11_0x24);

    i32 animation = 25;
    i32 table_offset = 100;
    if (game_character->use_standard_idle == 0 && (character->model_flags & 0x80) != 0) {
        animation = 118;
        table_offset = 472;
    }
    if (obj->batarang != NULL && *(reinterpret_cast<u8 *>(obj->batarang) + 0x7d) != 0) {
        return 151;
    }

    u8 *animation_table = reinterpret_cast<u8 *>(obj->apiobj.character_model->model_data_b);
    void *entry = *reinterpret_cast<void **>(animation_table + table_offset);
    if (entry != NULL &&
        (*reinterpret_cast<i32 *>(animation_table + 4) == 0 || (obj->field_0xe22 & 1) != 0 || obj->field_0xe32 == 1)) {
        return animation;
    }
    return 1;
}

void GetAnimDirection(nuinstanim_s *) {
}

void FindTexAnimFromMtl(nugscn_s *, numtl_s *) {
}

void InitTexAnimScripts(char **) {
}

void GizmoFileReadGameAnimSet(GAMEANIMSET_s *, void *, void (*)(GAMEANIMOBJ_s *, unsigned char), unsigned char, char *,
                              char *) {
}

void ANI_SimpleAni3PlayerV4Joint_Quat3(ani3_animheader_s *, float, nuanimbuff_s *, i32, i32) {
}

void ANI_SimpleAni3PlayerV4Joint_Quat3W(ani3_animheader_s *, float, nuanimbuff_s *, i32, i32) {
}

void ANI_SimpleAni3PlayerV4Joint_Blend_Quat3(ani3_animheader_s *, float, nuanimbuff_s *, float, i32, i32, nuvec_s *) {
}

void ANI_SimpleAni3PlayerV4Joint_Blend_Quat3W(ani3_animheader_s *, float, nuanimbuff_s *, float, i32, i32, nuvec_s *) {
}

void EvalAnim(nuhspecial_s *, float, numtx_s *, i32) {
}

void EvalAnim2(nuhspecial_s *, float) {
}

void *GameAnimSys_Create(variptr_u *buf, variptr_u *buf_end) {
    (void)buf;
    (void)buf_end;
    return NULL;
}

void *GameAntnode_CreateSys(WORLDINFO_s *world, variptr_u *buf, variptr_u *buf_end, i32 count) {
    (void)world;
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
