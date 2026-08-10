#include "legoapi/gameobject.h"

#include "globals.h"
#include "legoapi/qrand.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"

struct AnimationInfo {
    u32 unknown_0;
    u32 flags;
    u8 unknown_8[0x20];
    f32 footstep_frames[4];
};

struct AnimationCharacterData {
    u8 unknown_0[8];
    AnimationInfo **animation_info;
    void **animations;
};

struct AnimationRuntimeData {
    u8 unknown_0[0x50];
    f32 head_horizontal_limit;
    f32 head_vertical_limit;
    f32 head_turn_speed;
    f32 cloak_maximum_angle;
    f32 cloak_minimum_angle;
    u8 unknown_64[0x33];
    u8 flags_97;
    u8 unknown_98[0x76];
    i8 head_joint_index;
    u8 unknown_10f[9];
    i8 head_modifier_joint_index;
    i8 cloak_joint_index;
    i8 hair_joint_index;
};

struct AnimationModifier {
    f32 x_angle;
    f32 y_angle;
    f32 z_angle;
    u8 unknown_0c[0x18];
    i16 maximum_x_angle;
    i16 maximum_y_angle;
    i16 maximum_z_angle;
    i16 minimum_x_angle;
    i16 minimum_y_angle;
    i16 minimum_z_angle;
    i8 joint_index;
    u8 flags;
    u8 unknown_32[2];
};

static_assert(sizeof(AnimationModifier) == 0x34, "AnimationModifier size");
static_assert(offsetof(AnimationModifier, joint_index) == 0x30, "AnimationModifier joint offset");

struct AnimationCharacterInfo {
    u8 unknown_0[10];
    u8 flags_a;
    u8 unknown_b[5];
};

struct ApiCharacterSystem {
    u8 unknown_0[8];
    i32 character_count;
};

void PlayFootStepSfx(GameObject_s *object);
extern i16 LEGOACT_FALL;
extern AnimationCharacterInfo *CInfo;
extern ApiCharacterSystem *apicharsys;
extern "C" void NuVecInvMtxTransform(NUVEC *destination, NUVEC *source, NUMTX *matrix);

static AnimationModifier *GetAnimationModifier(GameObject_s *object) {
    return (AnimationModifier *)((u8 *)object + 0xf48 + object->animation_modifier_count * 0x34);
}

static i16 GetCurrentAnimation(GameObject_s *object) {
    return object->animation_suppressed == 0 ? object->animation_index : object->suppressed_animation_index;
}

static bool CrossedAnimationFrame(f32 previous, f32 current, f32 frame) {
    if (frame < 1.0f) {
        return false;
    }
    if (previous <= current) {
        return previous < frame && frame <= current;
    }
    return frame <= current || previous < frame;
}

static bool AnimationAllowsHeadMovement(GameObject_s *object) {
    AnimationCharacterData *character_data = (AnimationCharacterData *)object->game_character_data;
    i32 animation_index;

    if (object->animation_suppressed == 0) {
        animation_index = object->animation_index;
    } else {
        animation_index = object->suppressed_animation_index;
        if (animation_index >= 0 && animation_index < apicharsys->character_count &&
            character_data->animations[animation_index] != NULL &&
            (character_data->animation_info[animation_index]->flags & 0x40) != 0) {
            return false;
        }
        animation_index = object->fallback_animation_index;
    }

    if (animation_index >= 0 && animation_index < apicharsys->character_count &&
        character_data->animations[animation_index] != NULL) {
        return (character_data->animation_info[animation_index]->flags & 0x40) == 0;
    }
    return true;
}

static f32 ClampHeadAngle(f32 angle, f32 limit) {
    if (angle >= limit) {
        return limit;
    }
    if (angle <= -limit) {
        return -limit;
    }
    return angle;
}

void AddFootSteps(GameObject_s *object) {
    if (object->animation_suppressed != 0 || object->animation_index == -1) {
        return;
    }

    AnimationCharacterData *character_data = (AnimationCharacterData *)object->game_character_data;
    i32 animation_index = object->animation_index;
    if (character_data->animations[animation_index] == NULL) {
        return;
    }

    AnimationInfo *animation = character_data->animation_info[animation_index];
    if ((animation->flags & 0x100) == 0 || object->animation_frame == object->previous_animation_frame) {
        return;
    }

    bool moving_forward;
    if ((object->animation_flags & 2) == 0) {
        moving_forward = object->previous_animation_frame < object->animation_frame;
    } else {
        moving_forward = object->animation_frame < object->previous_animation_frame;
    }
    if (!moving_forward) {
        return;
    }

    i32 first_frame = (animation->flags & 0x10000) != 0 ? 0 : 2;
    for (i32 i = first_frame; i < 4; i++) {
        if (CrossedAnimationFrame(object->previous_animation_frame, object->animation_frame,
                                  animation->footstep_frames[i])) {
            PlayFootStepSfx(object);
            return;
        }
    }
}

void SetHeadTarget(GameObject_s *object, NUVEC *target, i8 priority, f32 time, f32 minimum_duration,
                   f32 maximum_duration) {
    if (target == NULL) {
        object->head_target = NULL;
        object->head_target_priority = 0;
        object->head_target_time = 0.0f;
        object->head_target_duration = 0.0f;
        return;
    }

    NUVEC *position = target;
    if (object->head_target == NULL || (target != object->head_target && object->head_target_priority <= priority)) {
        object->head_target = target;
        object->head_target_priority = priority;
        f32 random = QRAND_FLOAT();
        object->head_target_duration = maximum_duration * random + (1.0f - random) * minimum_duration;
        object->head_target_time = time + object->head_target_duration;
    } else if (target != object->head_target) {
        position = object->head_target;
    }
    object->head_target_position = *position;
}

void SetObjAsHeadTarget(GameObject_s *object, GameObject_s *target, i8, f32, f32, f32) {
    if (target == NULL || object == NULL || target->character_data == NULL) {
        return;
    }

    AnimationRuntimeData *runtime = (AnimationRuntimeData *)target->character_data->field11_0x24;
    i32 joint_index = runtime->head_joint_index;
    if (joint_index == -1) {
        SetHeadTarget(object, &target->origin, 2, 1.0f, 0.0f, 0.0f);
    } else {
        NUVEC *joint_position = (NUVEC *)&target->player_packet.joint_matrices[joint_index].m30;
        SetHeadTarget(object, joint_position, 2, 1.0f, 0.0f, 0.0f);
    }
}

void HeadMovement(GameObject_s *object) {
    if ((CInfo[object->player_packet.character_state].flags_a & 0x20) != 0) {
        return;
    }

    CHARACTERDATA *character_data = object->character_data;
    AnimationRuntimeData *runtime = (AnimationRuntimeData *)character_data->field11_0x24;
    u8 modifier_index = object->animation_modifier_count;
    if (modifier_index > 2 || runtime->head_modifier_joint_index == -1) {
        return;
    }

    bool animation_allows_movement = AnimationAllowsHeadMovement(object);
    bool has_active_target = object->head_target != NULL && object->head_target_time >= object->head_target_duration &&
                             runtime->head_joint_index != -1 && animation_allows_movement;
    AnimationModifier *modifier = GetAnimationModifier(object);
    f32 target_x = 0.0f;
    f32 target_y = 0.0f;
    f32 target_z = 0.0f;

    if ((runtime->flags_97 & 0x40) == 0 && (((u8 *)character_data)[6] & 2) != 0) {
        if (has_active_target) {
            NUVEC local_target;
            NuVecInvMtxTransform(&local_target, &object->head_target_position,
                                 &object->player_packet.joint_matrices[runtime->head_joint_index]);
            if (local_target.z < 0.0f) {
                target_x = modifier->x_angle - NuAtan2(-local_target.x, -local_target.z);
                target_y = modifier->y_angle - NuAtan2(-local_target.y, -local_target.z);
                target_x = ClampHeadAngle(target_x, runtime->head_horizontal_limit);
                target_y = ClampHeadAngle(target_y, runtime->head_vertical_limit);
            }
        }

        f32 weight = runtime->head_turn_speed * FRAMETIME;
        if (weight > 1.0f) {
            weight = 1.0f;
        }
        f32 previous_weight = 1.0f - weight;
        modifier->x_angle = target_x * weight + modifier->x_angle * previous_weight;
        modifier->y_angle = target_y * weight + modifier->y_angle * previous_weight;
        modifier->z_angle = 0.0f;
        modifier->maximum_x_angle = (i16)(runtime->head_horizontal_limit * 10430.378f);
        modifier->maximum_y_angle = (i16)(runtime->head_vertical_limit * 10430.378f);
        modifier->minimum_x_angle = -modifier->maximum_x_angle;
        modifier->minimum_y_angle = -modifier->maximum_y_angle;
        modifier->joint_index = runtime->head_modifier_joint_index;
        modifier->flags = 0;

        if (NuFabs(modifier->x_angle) > 0.0017453294f || NuFabs(modifier->y_angle) > 0.0017453294f) {
            modifier->flags = 1;
            if (animation_allows_movement) {
                if (modifier->x_angle != 0.0f) {
                    modifier->flags = 9;
                }
                if (modifier->y_angle != 0.0f) {
                    modifier->flags |= 0x10;
                }
            }
        }
    } else {
        object->head_movement_flags &= ~1;
        bool mirrored = (runtime->flags_97 & 0x40) != 0;
        if (has_active_target) {
            object->head_movement_flags |= 1;
            NUVEC local_target;
            NuVecInvMtxTransform(&local_target, &object->head_target_position,
                                 &object->player_packet.joint_matrices[runtime->head_joint_index]);
            if (local_target.z < 0.0f) {
                f32 horizontal_distance = NuFsqrt(local_target.x * local_target.x + local_target.z * local_target.z);
                f32 vertical_angle = NuAtan2(-local_target.y, horizontal_distance);
                f32 horizontal_angle = NuAtan2(-local_target.x, -local_target.z);
                if (mirrored) {
                    target_y = vertical_angle - modifier->y_angle;
                    target_z = -horizontal_angle - modifier->z_angle;
                } else {
                    target_y = modifier->y_angle - vertical_angle;
                    target_z = horizontal_angle + modifier->z_angle;
                }
                target_y = ClampHeadAngle(target_y, runtime->head_vertical_limit);
                if (-runtime->head_horizontal_limit < target_z && target_z < runtime->head_horizontal_limit) {
                    object->head_movement_flags &= ~1;
                }
                target_z = ClampHeadAngle(target_z, runtime->head_horizontal_limit);
            }
        }

        f32 weight = runtime->head_turn_speed * FRAMETIME;
        if (weight > 1.0f) {
            weight = 1.0f;
        }
        f32 previous_weight = 1.0f - weight;
        modifier->x_angle = 0.0f;
        modifier->y_angle = target_y * weight + modifier->y_angle * previous_weight;
        modifier->z_angle = target_z * weight + modifier->z_angle * previous_weight;
        modifier->maximum_y_angle = (i16)(runtime->head_vertical_limit * 10430.378f);
        modifier->maximum_z_angle = (i16)(runtime->head_horizontal_limit * 10430.378f);
        modifier->minimum_y_angle = -modifier->maximum_y_angle;
        modifier->minimum_z_angle = -modifier->maximum_z_angle;
        modifier->joint_index = runtime->head_modifier_joint_index;
        modifier->flags = 0;

        if (NuFabs(modifier->y_angle) > 0.0017453294f || NuFabs(modifier->z_angle) > 0.0017453294f) {
            modifier->flags = 1;
            if (animation_allows_movement) {
                if (modifier->y_angle != 0.0f) {
                    modifier->flags = 0x11;
                }
                if (modifier->z_angle != 0.0f) {
                    modifier->flags |= 0x20;
                }
            }
        }
    }

    if (object->head_target != NULL) {
        object->head_target_time -= FRAMETIME;
        if (object->head_target_time < 0.0f) {
            object->head_target_time = 0.0f;
            object->head_target_duration = 0.0f;
            object->head_target = NULL;
            object->head_target_priority = 0;
        }
    }
    object->animation_modifier_count = modifier_index + 1;
}

void CloakMovement(GameObject_s *object) {
    if (object->animation_modifier_count > 2) {
        return;
    }

    AnimationRuntimeData *runtime = (AnimationRuntimeData *)object->character_data->field11_0x24;
    if (runtime->cloak_joint_index == -1) {
        return;
    }

    AnimationModifier *modifier = GetAnimationModifier(object);
    f32 phase = NuFsqrt((modifier->z_angle - runtime->cloak_minimum_angle) /
                        (runtime->cloak_maximum_angle - runtime->cloak_minimum_angle));
    f32 maximum_weight;
    f32 minimum_weight;
    if (LEGOACT_FALL != -1 && GetCurrentAnimation(object) == LEGOACT_FALL) {
        phase += FRAMETIME + FRAMETIME;
        if (phase <= 1.0f) {
            maximum_weight = phase * phase;
            minimum_weight = 1.0f - maximum_weight;
        } else {
            maximum_weight = 1.0f;
            minimum_weight = 0.0f;
        }
    } else {
        phase -= FRAMETIME + FRAMETIME;
        if (phase < 0.0f) {
            minimum_weight = 1.0f;
            maximum_weight = 0.0f;
        } else {
            maximum_weight = phase * phase;
            minimum_weight = 1.0f - maximum_weight;
        }
    }

    modifier->joint_index = runtime->cloak_joint_index;
    modifier->z_angle = maximum_weight * runtime->cloak_maximum_angle + minimum_weight * runtime->cloak_minimum_angle;
    if (modifier->z_angle == 0.0f) {
        modifier->flags = 0;
    } else {
        modifier->flags = 0x21;
        f32 maximum = runtime->cloak_maximum_angle;
        f32 minimum = runtime->cloak_minimum_angle;
        if (minimum <= maximum) {
            modifier->maximum_z_angle = (i16)(maximum * 10430.378f);
            modifier->minimum_z_angle = (i16)(minimum * 10430.378f);
        } else {
            modifier->maximum_z_angle = (i16)(minimum * 10430.378f);
            modifier->minimum_z_angle = (i16)(maximum * 10430.378f);
        }
    }
    object->animation_modifier_count++;
}

void HairMovement(GameObject_s *object) {
    if (object->animation_modifier_count > 2) {
        return;
    }

    AnimationRuntimeData *runtime = (AnimationRuntimeData *)object->character_data->field11_0x24;
    if (runtime->hair_joint_index == -1) {
        return;
    }

    AnimationModifier *modifier = GetAnimationModifier(object);
    f32 phase = NuFsqrt(modifier->z_angle / -1.8325958f);
    if (LEGOACT_FALL != -1 && GetCurrentAnimation(object) == LEGOACT_FALL) {
        phase += FRAMETIME + FRAMETIME;
        modifier->z_angle = phase <= 1.0f ? phase * phase * -1.8325958f : -1.8325958f;
    } else {
        phase -= FRAMETIME + FRAMETIME;
        modifier->z_angle = phase < 0.0f ? 0.0f : phase * phase * -1.8325958f;
    }

    modifier->joint_index = runtime->hair_joint_index;
    if (modifier->z_angle != 0.0f) {
        modifier->flags = 0x21;
        modifier->maximum_z_angle = 0;
        modifier->minimum_z_angle = (i16)0xb556;
    }
    object->animation_modifier_count++;
}
