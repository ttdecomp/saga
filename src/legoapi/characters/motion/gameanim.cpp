#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nurand.h"

#include <string.h>

float CalcValue1648(char *, i32, i32, float, ani3_scalemin_s *);
void EvalAnim(nuhspecial_s *special, f32 frame, numtx_s *matrix, i32 include_instance_translation);
bool UseFallAnim(GameObject_s *object);
i32 GetDefaultIdle(GameObject_s *object);
static void MoveAnim_Manage(GameObject_s *object, f32 movement_speed, i32 allow_weapon_anims, i32 variant);
static void MoveAnim_Check(GameObject_s *object);
static GAMECHARACTERDATA *GetGameCharacterData(GameObject_s *object);
void UpdateCharacterIdle(GameObject_s *object);
void AutoWeaponOnOff(GameObject_s *object);
void AddFootSteps(GameObject_s *object);

enum CHARACTER_ANIMATION : i16 {
    CHARACTER_ANIMATION_WALK = 0,
    CHARACTER_ANIMATION_IDLE = 1,
    CHARACTER_ANIMATION_RUN = 3,
    CHARACTER_ANIMATION_TIPTOE = 4,
    CHARACTER_ANIMATION_FALL = 5,
    CHARACTER_ANIMATION_WEAPON_IDLE = 11,
    CHARACTER_ANIMATION_ALT_IDLE = 25,
    CHARACTER_ANIMATION_SABER_RUN = 23,
    CHARACTER_ANIMATION_ALT_WEAPON_IDLE = 39,
    CHARACTER_ANIMATION_SABER_TIPTOE = 63,
    CHARACTER_ANIMATION_SABER_WALK = 64,
    CHARACTER_ANIMATION_BACKWARDS = 80,
};

enum CHARACTER_ANIMATION_FLAGS : u32 {
    CHARACTER_ANIMATION_FLAG_SYNCHRONISED = 0x02,
    CHARACTER_ANIMATION_FLAG_REVERSE_WITH_MOVEMENT = 0x80,
};

static bool HasAnimation(const CHARACTERMODEL_s *model, i32 animation) {
    return model != NULL && animation >= 0 && model->model_data_b != NULL && model->model_data_b[animation] != NULL;
}

static bool HasCharacterAnimation(const GameObject_s *object, i32 animation) {
    return object != NULL && HasAnimation(object->apiobj.character_model, animation);
}

static void MoveAnim_Check(GameObject_s *object) {
    if (object == NULL || HasCharacterAnimation(object, object->apiobj.anim_packet.requested_animation)) {
        return;
    }

    // Alternate between the standard walk and run when a model omits one of
    // them, then fall back to its ordinary idle animation.
    const i16 requested = object->apiobj.anim_packet.requested_animation;
    if (requested == CHARACTER_ANIMATION_WALK && HasCharacterAnimation(object, CHARACTER_ANIMATION_RUN)) {
        object->apiobj.anim_packet.requested_animation = CHARACTER_ANIMATION_RUN;
    } else if (requested == CHARACTER_ANIMATION_RUN && HasCharacterAnimation(object, CHARACTER_ANIMATION_WALK)) {
        object->apiobj.anim_packet.requested_animation = CHARACTER_ANIMATION_WALK;
    } else if (HasCharacterAnimation(object, CHARACTER_ANIMATION_IDLE)) {
        object->apiobj.anim_packet.requested_animation = CHARACTER_ANIMATION_IDLE;
    }
}

static void MoveAnim_Manage(GameObject_s *object, f32 movement_speed, i32, i32) {
    GAMECHARACTERDATA *game_character = GetGameCharacterData(object);
    if (object == NULL || game_character == NULL) {
        return;
    }

    const f32 tiptoe_walk_threshold = (game_character->tiptoe_speed + game_character->walk_speed) * 0.5f;
    const f32 walk_run_threshold = (game_character->walk_speed + game_character->run_speed) * 0.5f;

    CHARACTER_ANIMATION animation;
    if (movement_speed <= tiptoe_walk_threshold) {
        animation = CHARACTER_ANIMATION_TIPTOE;
    } else if (movement_speed <= walk_run_threshold) {
        animation = (object->field_0xefd & GAMEOBJECT_MOVEMENT_FLAG_BACKWARDS) != 0 &&
                            HasCharacterAnimation(object, CHARACTER_ANIMATION_BACKWARDS)
                        ? CHARACTER_ANIMATION_BACKWARDS
                        : CHARACTER_ANIMATION_WALK;
    } else {
        animation = CHARACTER_ANIMATION_RUN;
    }
    object->apiobj.anim_packet.requested_animation = animation;
    MoveAnim_Check(object);
}

static CHARACTERANIM_s *GetAnimationInfo(const CHARACTERMODEL_s *model, i32 animation) {
    if (model == NULL || animation < 0 || model->model_data_a == NULL) {
        return NULL;
    }
    return static_cast<CHARACTERANIM_s *>(model->model_data_a[animation]);
}

static GAMECHARACTERDATA *GetGameCharacterData(GameObject_s *object) {
    if (object == NULL || object->apiobj.character_data == NULL) {
        return NULL;
    }
    return static_cast<GAMECHARACTERDATA *>(object->apiobj.character_data->field11_0x24);
}

static f32 UpdateAnimationTimer(CHARACTERMODEL_s *model, ANIMPACKET_s *packet, i16 animation, f32 time, f32 frame_step,
                                f32 movement_speed, bool report_events, u8 *reversed, bool backwards,
                                f32 backwards_multiplier) {
    CHARACTERANIM_s *animation_info = GetAnimationInfo(model, animation);
    if (!HasAnimation(model, animation) || animation_info == NULL) {
        return time;
    }

    f32 rate = animation_info->playback_rate;
    if (animation_info->movement_speed > 0.0f) {
        rate *= movement_speed / animation_info->movement_speed;
        if (rate >= 0.0f) {
            if (animation_info->movement_rate_cap > 0.0f && rate > animation_info->movement_rate_cap) {
                rate = animation_info->movement_rate_cap;
            }
        } else if (animation_info->movement_rate_cap < 0.0f && rate < animation_info->movement_rate_cap) {
            rate = animation_info->movement_rate_cap;
        }
    }
    if (*reversed != 0) {
        frame_step = -(frame_step * backwards_multiplier);
    }

    const f32 delta = rate * (frame_step / 30.0f);
    time += delta;
    const f32 end_frame = NuAnimEndFrame(model->model_data_b[animation]);
    bool looped = false;

    if (delta >= 0.0f) {
        if (time > end_frame) {
            if ((animation_info->flags & CHARACTER_ANIMATION_FLAG_SYNCHRONISED) == 0) {
                time = end_frame;
                if (report_events) {
                    packet->flags |= ANIMPACKET_FLAG_FINISHED;
                }
            } else if (end_frame > 1.0f) {
                while (time > end_frame) {
                    time -= end_frame - 1.0f;
                }
                if (report_events) {
                    packet->flags |= ANIMPACKET_FLAG_LOOPED;
                }
                looped = true;
            } else {
                time = 1.0f;
            }
        }
    } else {
        if (report_events) {
            packet->flags |= ANIMPACKET_FLAG_PLAYING_REVERSED;
        }
        if (time < 1.0f) {
            if ((animation_info->flags & CHARACTER_ANIMATION_FLAG_SYNCHRONISED) == 0) {
                time = 1.0f;
                if (report_events) {
                    packet->flags |= ANIMPACKET_FLAG_FINISHED;
                }
            } else if (end_frame > 1.0f) {
                while (time < 1.0f) {
                    time += end_frame - 1.0f;
                }
                if (report_events) {
                    packet->flags |= ANIMPACKET_FLAG_LOOPED;
                }
                looped = true;
            } else {
                time = 1.0f;
            }
        }
    }

    if (looped) {
        if (*reversed == 0) {
            if (backwards && (animation_info->flags & CHARACTER_ANIMATION_FLAG_REVERSE_WITH_MOVEMENT) != 0) {
                *reversed = 1;
            }
        } else if (!backwards) {
            *reversed = 0;
        }
    }
    return time;
}

static void StartAnimation(CHARACTERMODEL_s *model, ANIMPACKET_s *packet, i16 animation, bool backwards) {
    packet->animation_index = animation;
    CHARACTERANIM_s *info = GetAnimationInfo(model, animation);
    const bool reverse =
        backwards && info != NULL && (info->flags & CHARACTER_ANIMATION_FLAG_REVERSE_WITH_MOVEMENT) != 0;
    packet->current_reversed = reverse ? 1 : 0;
    packet->current_time = reverse ? NuAnimEndFrame(model->model_data_b[animation]) : 1.0f;
    packet->previous_time = packet->current_time;
    packet->blending = 0;
}

static const u8 KeyStructSizes[16] = {3, 4, 4, 3, 4, 3, 4, 8, 4, 8, 4, 0, 0, 0, 0, 0};

void Animate_POD(GameObject_s *) {
}

void Animate_ATAT(GameObject_s *) {
}

void Animate_JEDI(GameObject_s *object) {
    if (object == NULL || object->apiobj.character_data == NULL) {
        return;
    }

    ANIMPACKET_s &packet = object->apiobj.anim_packet;
    CHARACTERMODEL_s *model = object->apiobj.character_model;

    if ((object->field_0xe23 & GAMEOBJECT_E23_FLAG_FORCE_WEAPON_IDLE) != 0) {
        const bool use_alternate =
            ((object->field_0xe22 & GAMEOBJECT_E22_FLAG_WEAPON_ANIMATION) != 0 || object->field_0xe32 == 1) &&
            HasAnimation(model, CHARACTER_ANIMATION_ALT_WEAPON_IDLE);
        packet.requested_animation =
            use_alternate ? CHARACTER_ANIMATION_ALT_WEAPON_IDLE : CHARACTER_ANIMATION_WEAPON_IDLE;
        UpdateCharacterIdle(object);
        return;
    }

    packet.requested_animation = CHARACTER_ANIMATION_FALL;
    if (object->apiobj.field_0x27d != 0) {
        packet.requested_animation = static_cast<i16>(GetDefaultIdle(object));
    }

    // Context zero owns the airborne jump state.  All other ordinary player
    // contexts go through the common fall/movement selector in the target.
    if (object->build_context != 0) {
        if (UseFallAnim(object)) {
            packet.requested_animation = CHARACTER_ANIMATION_FALL;
        } else if (packet.requested_animation != CHARACTER_ANIMATION_FALL) {
            GAMEPAD_s *pad = object->pad_gamepad;
            if (pad != NULL && (pad->allocated_5a & GAMEPAD_RUNTIME_SUPPRESS_MOVEMENT) == 0 &&
                pad->input_magnitude > 0.0f) {
                MoveAnim_Manage(object, pad->input_magnitude, 1, 0);
            } else if (((object->field_0xe22 & GAMEOBJECT_E22_FLAG_WEAPON_ANIMATION) != 0 ||
                        object->field_0xe32 == 1) &&
                       HasAnimation(model, CHARACTER_ANIMATION_ALT_IDLE)) {
                packet.requested_animation = CHARACTER_ANIMATION_ALT_IDLE;
            }
        }
    }

    MoveAnim_Check(object);
    UpdateCharacterIdle(object);
}

void AnimatePlayer(GameObject_s *object) {
    if (object == NULL || object->apiobj.character_data == NULL) {
        return;
    }

    ANIMPACKET_s &packet = object->apiobj.anim_packet;
    packet.previous_animation = packet.animation_index;
    object->mini_animation.previous_animation = object->mini_animation.animation_index;

    if ((object->apiobj.field_0x1f4 & APIOBJECT_STATE_FLAG_IGNORE_DOORS) == 0 &&
        object->apiobj.character_data->animate_fn != NULL) {
        object->apiobj.character_data->animate_fn(object);
    }

    CHARACTERMODEL_s *model = object->apiobj.character_model;
    GAMECHARACTERDATA *game_character = GetGameCharacterData(object);
    if (model == NULL || game_character == NULL ||
        (object->apiobj.field_0x287 != 0 && (object->field_0x1018 == 0.0f || object->apiobj.field_0x287 == 1))) {
        return;
    }

    f32 movement_speed = 0.0f;
    if (object->pad_gamepad != NULL) {
        movement_speed = object->pad_gamepad->animation_input_magnitude;
    }
    if ((game_character->flags_090 & GAMECHARACTER_FLAG_ANIMATION_SPEED_FROM_VELOCITY) != 0) {
        const f32 forward_speed = object->apiobj.velocity.x * object->facing_direction.x +
                                  object->apiobj.velocity.z * object->facing_direction.z;
        movement_speed = forward_speed > 0.0f ? forward_speed : 0.0f;
    }

    UpdateAnimPacket(model, &packet, FRAMETIME * 30.0f, movement_speed, FRAMETIME,
                     game_character->backwards_speed_multiplier);
    AutoWeaponOnOff(object);
    AddFootSteps(object);
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

i32 GameAnimSet_Play(GAMEANIMSET_s *set, float speed, i32 evaluate_state) {
    if (set == NULL) {
        return 1;
    }

    if (evaluate_state != 0) {
        GameAnimSet_EvaluateState(set);
    }
    set->flags = static_cast<GAMEANIMSET_FLAGS>(set->flags & ~GAMEANIMSET_FLAG_STOP_REQUESTED);

    if (speed >= 0.0f) {
        if (set->state == GAMEANIMSET_STATE_AT_END) {
            return 1;
        }
    } else if (speed < 0.0f) {
        if (set->state == GAMEANIMSET_STATE_AT_START) {
            return 1;
        }
    }

    for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
        nuinstanim_s *animation = object->instance_animation;
        if (animation == NULL) {
            continue;
        }

        animation->playing = 1;
        animation->waiting = 0;

        f32 direction;
        if (object->end_frame < object->start_frame) {
            direction = -1.0f;
        } else {
            direction = 1.0f;
        }
        f32 current_frame = animation->ltime * direction;
        if (object->start_frame * direction > current_frame) {
            animation->ltime = object->start_frame;
        } else if (current_frame > object->end_frame * direction) {
            animation->ltime = object->end_frame;
        }

        if (animation->fparam1 != 0.0f) {
            animation->tfactor = animation->fparam1 * speed * direction;
        } else {
            animation->tfactor = direction * speed;
        }
    }

    if ((set->flags & GAMEANIMSET_FLAG_IN_SYSTEM_LIST) == 0) {
        GameAnimSet_AddToSystemList(set);
    }
    return 1;
}

i32 GameAnimSet_Stop(GAMEANIMSET_s *set) {
    if (set == NULL) {
        return 1;
    }

    GAMEANIMSET_STATE state = static_cast<GAMEANIMSET_STATE>(set->state & ~GAMEANIMSET_STATE_AT_END);
    if (state != GAMEANIMSET_STATE_ACTIVE_FORWARD) {
        return 1;
    }

    set->flags = static_cast<GAMEANIMSET_FLAGS>(set->flags | GAMEANIMSET_FLAG_STOP_REQUESTED);
    for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation != NULL) {
            object->instance_animation->playing = 0;
        }
    }
    return 1;
}

void Animate_ASTROMECH(GameObject_s *) {
}

void Animate_CHARACTER(GameObject_s *) {
}

void Animate_GEONOSIAN(GameObject_s *) {
}

i32 GameAnimSet_Reset(GAMEANIMSET_s *set) {
    if (set != NULL) {
        for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
            nuinstanim_s *animation = object->instance_animation;
            if (animation != NULL) {
                animation->playing = 0;
                animation->waiting = 0;
                animation->tfactor = 1.0f;
                animation->ltime = object->start_frame;
            }
            GameAnimSet_RemoveFromSystemList(set);
        }
    }
    return 1;
}

void Animate_HOVERDROID(GameObject_s *) {
}

GAMEANIMSET_s *GameAnimSet_Create(variptr_u *buf, variptr_u *buf_end, GAMEANIMOBJPOOL_s *object_pool,
                                  GAMEANIMSYS_s *system) {
    GAMEANIMSET_s *set = NULL;
    if (object_pool != NULL) {
        set = static_cast<GAMEANIMSET_s *>(GameBufferAlloc(buf, buf_end, sizeof(GAMEANIMSET_s)));
        if (set != NULL) {
            set->object_pool = object_pool;
            set->system = system;
            i32 index = system->set_count;
            if (index < gameanimsysprogress.entry_size) {
                system->sets[index] = set;
                system->set_count = index + 1;
            }
        }
    }
    return set;
}

void Animate_BATTLEDROID(GameObject_s *) {
}

void Animate_SPEEDERBIKE(GameObject_s *) {
}

i32 GameAnimSet_Playing(GAMEANIMSET_s *set) {
    if (set != NULL) {
        for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL && object->instance_animation->playing == 0) {
                return 0;
            }
        }
    }
    return 1;
}

void GameAnimSet_EvalAnim(GAMEANIMSET_s *set) {
    if (set != NULL) {
        for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
            EvalAnim2(&object->special, object->instance_animation->ltime);
        }
    }
}

GAMEANIMOBJ_s *GameAnimSet_AddObject(GAMEANIMSET_s *set, nuhspecial_s *special, float start_frame, float end_frame,
                                     i32 append) {
    if (set == NULL || set->object_pool == NULL || set->object_pool->free_objects == NULL || special == NULL ||
        NuSpecialExistsFn(special) == 0) {
        return NULL;
    }

    GAMEANIMOBJPOOL_s *pool = set->object_pool;
    ++set->object_count;
    GAMEANIMOBJ_s *object = pool->free_objects;
    ++pool->active_count;
    pool->free_objects = object->next;

    if (append != 0) {
        object->next = NULL;
        if (set->objects == NULL) {
            set->objects = object;
        } else {
            GAMEANIMOBJ_s *tail = set->objects;
            while (tail->next != NULL) {
                tail = tail->next;
            }
            tail->next = object;
        }
    } else {
        object->next = set->objects;
        set->objects = object;
    }

    const i32 object_index = object - pool->objects;
    object->object_data = static_cast<u8 *>(pool->object_data) + pool->object_data_size * object_index;
    object->special = *special;
    object->instance_animation = NuSpecialGetInstAnim(&object->special);
    if (object->instance_animation == NULL) {
        return object;
    }

    object->animation = object->special.scene->instance_animation_data[object->instance_animation->anim_ix];
    const f32 last_frame = NuSpecialGetAnimEndFrame(&object->special);
    if (last_frame > 0.0f) {
        if (last_frame < end_frame) {
            object->end_frame = last_frame;
        } else {
            object->end_frame = end_frame;
            if (object->end_frame < 1.0f) {
                object->end_frame = 1.0f;
            }
        }
        if (start_frame > last_frame) {
            object->start_frame = last_frame;
        } else {
            object->start_frame = start_frame;
            if (object->start_frame < 1.0f) {
                object->start_frame = 1.0f;
            }
        }
        ++set->animated_object_count;
    } else {
        NuSpecialGetName(&object->special);
    }
    return object;
}

i32 GameAnimSet_JumpToEnd(GAMEANIMSET_s *set) {
    if (set != NULL) {
        for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->playing = 0;
                object->instance_animation->ltime = object->end_frame;
            }
        }
    }
    return 1;
}

void GameAnimSet_SetOffset(GAMEANIMSET_s *set, NUVEC *offset) {
    if (set == NULL) {
        return;
    }

    for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
        if ((object->flags & 4) != 0) {
            continue;
        }
        NUMTX *source = NuSpecialGetMtx(&object->special);
        if (source == NULL) {
            continue;
        }

        NUMTX matrix = *source;
        matrix.m30 += offset->x;
        matrix.m31 += offset->y;
        matrix.m32 += offset->z;
        NuSpecialSetDrawMtx(&object->special, &matrix);
    }
}

f32 GameAnimSet_GetAnimPos(GAMEANIMOBJ_s *object) {
    if (object == NULL || object->instance_animation == NULL || object->animation == NULL) {
        return 0.0f;
    }

    if (object->start_frame == object->end_frame) {
        return 1.0f;
    }

    f32 position =
        (object->instance_animation->ltime - object->start_frame) / (object->end_frame - object->start_frame);
    if (position < 0.0f) {
        return 0.0f;
    }
    if (position > 1.0f) {
        position = 1.0f;
    }
    return position;
}

void GameAnimSet_SetAnimPos(GAMEANIMOBJ_s *object, float position) {
    if (object == NULL || object->instance_animation == NULL || object->animation == NULL) {
        return;
    }

    if (position < 0.0f) {
        position = 0.0f;
    }
    if (position > 1.0f) {
        position = 1.0f;
    }
    object->instance_animation->ltime = (object->end_frame - object->start_frame) * position + object->start_frame;
}

void GameAnimSet_SetTFactor(GAMEANIMSET_s *set, float factor) {
    if (set == NULL) {
        return;
    }

    for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
        nuinstanim_s *animation = object->instance_animation;
        if (animation != NULL) {
            f32 direction = object->end_frame < object->start_frame ? -1.0f : 1.0f;
            if (animation->fparam1 != 0.0f) {
                animation->tfactor = animation->fparam1 * factor * direction;
            } else {
                animation->tfactor = direction * factor;
            }
        }
    }
}

void Animate_REPUBLICGUNSHIP(GameObject_s *) {
}

i32 GameAnimSet_JumpToStart(GAMEANIMSET_s *set) {
    if (set != NULL) {
        for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->playing = 0;
                object->instance_animation->ltime = object->start_frame;
            }
        }
    }
    return 1;
}

void Animate_SUPERBATTLEDROID(GameObject_s *) {
}

void GameAnimSet_RemoveObject(GAMEANIMSET_s *, GAMEANIMOBJ_s *) {
}

void GameAnimSet_ScaleFParam1(GAMEANIMSET_s *set, float scale) {
    if (set != NULL) {
        for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->fparam1 *= scale;
            }
        }
    }
}

i32 GameAnimSet_SetRepeating(GAMEANIMSET_s *set, i32 repeating) {
    if (set != NULL) {
        for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->repeating = repeating & 1;
            }
        }
    }
    return 1;
}

void GameAnimSet_EvaluateState(GAMEANIMSET_s *set) {
    if ((set->flags & GAMEANIMSET_FLAG_IN_SYSTEM_LIST) != 0) {
        return;
    }

    GAMEANIMOBJ_s *object = set->objects;
    i32 all_at_end = 1;
    i32 all_at_start = 1;

    while (object != NULL) {
        nuinstanim_s *animation = object->instance_animation;
        if (animation != NULL) {
            f32 direction = object->end_frame < object->start_frame ? -1.0f : 1.0f;
            f32 current_frame = animation->ltime * direction;
            if (object->end_frame * direction > current_frame) {
                all_at_end = 0;
            }
            if (current_frame > object->start_frame * direction) {
                all_at_start = 0;
            }
        }
        object = object->next;
    }

    set->state = GAMEANIMSET_STATE_AT_START;
    if (all_at_end != 0) {
        set->state = GAMEANIMSET_STATE_AT_END;
    } else if (all_at_start == 0) {
        set->state = GAMEANIMSET_STATE_BETWEEN_ENDPOINTS;
    }
}

void GameAnimSet_GetAveragePos(GAMEANIMSET_s *, nuvec_s *, i32, i32, i32) {
}

GAMEANIMSET_VISIBILITY GameAnimSet_GetVisibility(GAMEANIMSET_s *set) {
    if (set == NULL) {
        return GAMEANIMSET_VISIBILITY_NONE;
    }

    i32 visible_count = 0;
    for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
        if (NuSpecialGetVisibilityFn(&object->special) == 1) {
            ++visible_count;
        }
    }

    if (visible_count == set->object_count) {
        return GAMEANIMSET_VISIBILITY_ALL;
    }
    if (visible_count > 0) {
        return GAMEANIMSET_VISIBILITY_PARTIAL;
    }
    return GAMEANIMSET_VISIBILITY_NONE;
}

void GameAnimSet_JumpToAnimPos(GAMEANIMSET_s *set, float position) {
    if (set == NULL) {
        return;
    }

    for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
        nuinstanim_s *animation = object->instance_animation;
        if (animation == NULL || object->animation == NULL) {
            continue;
        }

        animation->playing = 0;
        f32 frame = (object->end_frame - object->start_frame) * position + object->start_frame;
        f32 direction = object->end_frame < object->start_frame ? -1.0f : 1.0f;
        f32 scaled_frame = frame * direction;
        f32 scaled_end = object->end_frame * direction;
        if (scaled_frame <= scaled_end) {
            animation->ltime = frame;
        } else {
            animation->ltime = object->end_frame;
            scaled_frame = scaled_end;
        }
        if (object->start_frame * direction > scaled_frame) {
            animation->ltime = object->start_frame;
        }
    }
}

void GameAnimSet_RemoveSpecial(GAMEANIMSET_s *, nuhspecial_s *) {
}

void GameAnimSet_SetVisibility(GAMEANIMSET_s *set, i32 visibility) {
    if (set != NULL) {
        for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
            NuSpecialSetVisibility(&object->special, visibility);
        }
    }
}

void GameAnimSet_DrawReflection(GAMEANIMSET_s *, i32, float, numtx_s *) {
}

GAMEANIMOBJ_s *GameAnimSet_AddObjectByName(GAMEANIMSET_s *set, nugscn_s *scene, char *name, float start_frame,
                                           float end_frame, i32 append, GIZMOSYS_s *gizmo_sys, char *prefix,
                                           char *suffix) {
    if (set == NULL) {
        return NULL;
    }

    nuhspecial_s special;
    if (Gizmo_FindNuSpecial(scene, &special, name, 1, gizmo_sys, prefix, suffix) == 0) {
        return NULL;
    }
    return GameAnimSet_AddObject(set, &special, start_frame, end_frame, append);
}

void GameAnimSet_AddToSystemList(GAMEANIMSET_s *set) {
    if (set != NULL && set->system != NULL && (set->flags & GAMEANIMSET_FLAG_IN_SYSTEM_LIST) == 0) {
        NuLinkedListAppend(&set->system->active_sets, &set->links);
        set->flags = static_cast<GAMEANIMSET_FLAGS>(set->flags | GAMEANIMSET_FLAG_IN_SYSTEM_LIST);
    }
}

void GameAnimSet_AutoSetReflectY(GAMEANIMSET_s *, nuvec_s *, numtx_s *) {
}

f32 GameAnimSet_GetCurrentFrame(GAMEANIMSET_s *set) {
    if (set != NULL) {
        for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                return object->instance_animation->ltime;
            }
        }
    }
    return 0.0f;
}

GAMEANIMOBJPOOL_s *GameAnimSet_CreateObjectPool(variptr_u *buf, variptr_u *buf_end, i32 object_data_size,
                                                i32 capacity) {
    GAMEANIMOBJPOOL_s *pool = NULL;
    if (capacity != 0) {
        pool = static_cast<GAMEANIMOBJPOOL_s *>(GameBufferAlloc(buf, buf_end, sizeof(GAMEANIMOBJPOOL_s)));
        if (pool != NULL) {
            pool->capacity = static_cast<u16>(capacity);
            pool->object_data_size = static_cast<u16>(object_data_size);
            pool->objects = static_cast<GAMEANIMOBJ_s *>(
                GameBufferAlloc(buf, buf_end, static_cast<u16>(capacity) * sizeof(GAMEANIMOBJ_s)));
            if (object_data_size != 0) {
                pool->object_data = GameBufferAlloc(
                    buf, buf_end, static_cast<u16>(pool->object_data_size) * static_cast<u16>(pool->capacity));
            }

            for (i32 i = 0; i < pool->capacity; ++i) {
                GAMEANIMOBJ_s *object = &pool->objects[i];
                object->next = pool->free_objects;
                pool->free_objects = object;
            }
        }
    }
    return pool;
}

void GameAnimSet_IsAnimationReset(GAMEANIMSET_s *) {
}

void GameAnimSet_RemoveAllObjects(GAMEANIMSET_s *) {
}

i32 GameAnimSet_GetCentreAndRadius(GAMEANIMSET_s *set, NUVEC *centre, f32 *radius, i32 frame_selection,
                                   i32 include_animated, i32 include_static) {
    if (centre == NULL || set == NULL || set->object_count == 0 || set->objects == NULL) {
        return 0;
    }

    NUVEC minimum = {1.0e9f, 1.0e9f, 1.0e9f};
    NUVEC maximum = {-1.0e9f, -1.0e9f, -1.0e9f};
    bool found_object = false;

    for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
        if ((object->flags & 1) != 0) {
            continue;
        }

        NUMTX matrix;
        if (object->instance_animation == NULL) {
            if (include_static == 0) {
                continue;
            }
            NUMTX *draw_matrix = NuSpecialGetDrawMtx(&object->special);
            if (draw_matrix == NULL) {
                continue;
            }
            matrix = *draw_matrix;
        } else {
            if (include_animated == 0) {
                continue;
            }
            f32 frame;
            if (frame_selection == 0) {
                frame = object->start_frame;
            } else if (frame_selection == 1) {
                frame = object->end_frame;
            } else {
                frame = object->instance_animation->ltime;
            }
            EvalAnim(&object->special, frame, &matrix, 1);
        }

        NUVEC object_centre;
        f32 object_radius;
        NuSpecialGetRadius(&object->special, &object_centre, &object_radius);
        object_radius *= 0.75f;
        NuVecMtxTransform(&object_centre, &object_centre, &matrix);

        const NUVEC object_minimum = {
            object_centre.x - object_radius,
            object_centre.y - object_radius,
            object_centre.z - object_radius,
        };
        const NUVEC object_maximum = {
            object_centre.x + object_radius,
            object_centre.y + object_radius,
            object_centre.z + object_radius,
        };
        if (object_minimum.x < minimum.x) {
            minimum.x = object_minimum.x;
        }
        if (object_minimum.y < minimum.y) {
            minimum.y = object_minimum.y;
        }
        if (object_minimum.z < minimum.z) {
            minimum.z = object_minimum.z;
        }
        if (object_maximum.x > maximum.x) {
            maximum.x = object_maximum.x;
        }
        if (object_maximum.y > maximum.y) {
            maximum.y = object_maximum.y;
        }
        if (object_maximum.z > maximum.z) {
            maximum.z = object_maximum.z;
        }
        found_object = true;
    }

    if (!found_object) {
        return 0;
    }

    centre->x = (minimum.x + maximum.x) * 0.5f;
    centre->y = (minimum.y + maximum.y) * 0.5f;
    centre->z = (minimum.z + maximum.z) * 0.5f;
    if (radius != NULL) {
        const f32 half_x = (maximum.x - minimum.x) * 0.5f;
        const f32 half_y = (maximum.y - minimum.y) * 0.5f;
        const f32 half_z = (maximum.z - minimum.z) * 0.5f;
        *radius = NuFsqrt(half_x * half_x + half_y * half_y + half_z * half_z);
    }
    return 1;
}

f32 GameAnimSet_GetCompletionRatio(GAMEANIMSET_s *set) {
    if (set == NULL) {
        return 0.0f;
    }
    for (GAMEANIMOBJ_s *object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation == NULL) {
            continue;
        }
        if (object->start_frame == object->end_frame) {
            return 1.0f;
        }
        f32 ratio =
            (object->instance_animation->ltime - object->start_frame) / (object->end_frame - object->start_frame);
        if (ratio > 1.0f) {
            ratio = 1.0f;
        }
        if (ratio < 0.0f) {
            ratio = 0.0f;
        }
        return ratio;
    }
    return 0.0f;
}

void GameAnimSet_RemoveFromSystemList(GAMEANIMSET_s *set) {
    if (set != NULL && set->system != NULL) {
        if ((set->flags & GAMEANIMSET_FLAG_IN_SYSTEM_LIST) != 0) {
            NuLinkedListRemove(&set->system->active_sets, &set->links);
            set->flags = static_cast<GAMEANIMSET_FLAGS>(set->flags & ~GAMEANIMSET_FLAG_IN_SYSTEM_LIST);
        }
        GameAnimSet_EvaluateState(set);
    }
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

    void ANI_FixUpAddrs(ani3_animheader_s *anim, isize delta) {
        if (anim->magic != 0x414e4934) {
            return;
        }
        while (true) {
            if (anim->scale_min != NULL) {
                anim->scale_min = reinterpret_cast<ani3_scalemin_s *>(reinterpret_cast<u8 *>(anim->scale_min) + delta);
            }
            if (anim->constants != NULL) {
                anim->constants = reinterpret_cast<i16 *>(reinterpret_cast<u8 *>(anim->constants) + delta);
            }
            if (anim->curve_types != NULL) {
                anim->curve_types = reinterpret_cast<u16 *>(reinterpret_cast<u8 *>(anim->curve_types) + delta);
            }
            if (anim->keys != NULL) {
                anim->keys = reinterpret_cast<u8 *>(anim->keys) + delta;
            }
            if (anim->node_flags != NULL) {
                anim->node_flags = reinterpret_cast<u8 *>(anim->node_flags) + delta;
            }
            if (anim->field_38 != NULL) {
                anim->field_38 = reinterpret_cast<u8 *>(anim->field_38) + delta;
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

    void AnimEndFrame(void) {
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

    i32 CurrentAnim(ANIMPACKET_s *packet) {
        if (packet == NULL) {
            return -1;
        }
        return packet->blending == 0 ? packet->animation_index : packet->blend_animation_b;
    }

    void EvalModelAnim(void) {
    }

    void FindAnimIX(void) {
    }

    void GetAnimTimeRandom(void) {
    }

    void GetInstAnimEndFrame(void) {
    }

    void ResetAnimPacket(ANIMPACKET_s *packet, i16 animation) {
        if (packet == NULL) {
            return;
        }
        packet->requested_animation = animation;
        packet->previous_animation = packet->requested_animation;
        packet->animation_index = packet->previous_animation;
        packet->previous_time = 1.0f;
        packet->blend_target_time = packet->previous_time;
        packet->current_time = packet->blend_target_time;
        packet->blending = 0;
        packet->flags = ANIMPACKET_FLAG_ANIMATION_CHANGED;
        packet->overlay_animation = -1;
        packet->current_reversed = 0;
        packet->blend_source_reversed = 0;
        packet->blend_target_reversed = 0;
    }

    void ResetMiniAnimPacket(void) {
    }

    void RootFn(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32) {
    }

    void RootFnY(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32) {
    }

    void SetActionInfo(void *action_info, void *extra_action_data) {
        (void)action_info;
        (void)extra_action_data;
    }

    void SetAnimTimeRandom(CHARACTERMODEL_s *model, ANIMPACKET_s *packet) {
        if (model == NULL || packet == NULL) {
            return;
        }

        void *animation = model->model_data_b[packet->field_0x3a];
        if (animation != NULL) {
            const f32 random = NuRandFloat();
            const f32 end_frame = NuAnimEndFrame(model->model_data_b[packet->field_0x3a]);
            packet->field_0x00 = 1.0f + random * (end_frame - 1.0f);
        }
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

    StateAnim *StateAnimFixPtrs(StateAnim *state, isize delta) {
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

    void UpdateAnimPacket(CHARACTERMODEL_s *model, ANIMPACKET_s *packet, f32 frame_step, f32 movement_speed,
                          f32 blend_step, f32 backwards_multiplier) {
        if (model == NULL || packet == NULL) {
            return;
        }

        const bool backwards = movement_speed < 0.0f;
        if (backwards) {
            movement_speed = -movement_speed;
        }

        const u8 previous_flags = packet->flags;
        packet->flags = 0;
        packet->previous_time = packet->blending == 0 ? packet->current_time : packet->blend_target_time;
        if (frame_step == 0.0f) {
            packet->flags |= ANIMPACKET_FLAG_ZERO_TIMESTEP;
            return;
        }

        if (packet->overlay_animation == -1) {
            if (packet->blending != 0) {
                packet->blend_elapsed += blend_step;
                if (packet->blend_elapsed >= packet->blend_duration) {
                    packet->blending = 0;
                    packet->animation_index = packet->blend_animation_b;
                    packet->current_time = packet->blend_target_time;
                    packet->current_reversed = packet->blend_target_reversed;
                    packet->flags |= ANIMPACKET_FLAG_BLEND_FINISHED;
                }
            }

            const i16 requested = packet->requested_animation;
            const bool restart = (previous_flags & ANIMPACKET_FLAG_FORCE_RESTART) != 0;
            const bool request_changed = requested != packet->previous_animation || restart;
            if (request_changed && HasAnimation(model, requested)) {
                const i16 current = static_cast<i16>(CurrentAnim(packet));
                if (!HasAnimation(model, current)) {
                    StartAnimation(model, packet, requested, backwards);
                } else if (packet->blending == 0 || packet->blend_animation_b != requested) {
                    i16 source_animation = packet->animation_index;
                    f32 source_time = packet->current_time;
                    u8 source_reversed = packet->current_reversed;
                    if (packet->blending != 0) {
                        const bool use_target = packet->blend_elapsed >= packet->blend_duration * 0.5f;
                        source_animation = use_target ? packet->blend_animation_b : packet->blend_animation_a;
                        source_time = use_target ? packet->blend_target_time : packet->blend_source_time;
                        source_reversed = use_target ? packet->blend_target_reversed : packet->blend_source_reversed;
                    }

                    CHARACTERANIM_s *source_info = GetAnimationInfo(model, source_animation);
                    CHARACTERANIM_s *target_info = GetAnimationInfo(model, requested);
                    if (source_info == NULL || target_info == NULL || source_info->blend_out_time <= blend_step ||
                        target_info->blend_in_time <= blend_step) {
                        StartAnimation(model, packet, requested, backwards);
                    } else {
                        packet->blending = 1;
                        packet->blend_animation_a = source_animation;
                        packet->blend_source_time = source_time;
                        packet->blend_source_reversed = source_reversed;
                        packet->blend_animation_b = requested;

                        const bool animations_synchronise =
                            (source_info->flags & CHARACTER_ANIMATION_FLAG_SYNCHRONISED) != 0 &&
                            (target_info->flags & CHARACTER_ANIMATION_FLAG_SYNCHRONISED) != 0 &&
                            source_info->playback_rate == target_info->playback_rate &&
                            NuAnimEndFrame(model->model_data_b[source_animation]) ==
                                NuAnimEndFrame(model->model_data_b[requested]);
                        if (animations_synchronise) {
                            packet->blend_target_time = packet->blend_source_time;
                            packet->blend_target_reversed =
                                backwards && (target_info->flags & CHARACTER_ANIMATION_FLAG_REVERSE_WITH_MOVEMENT) != 0
                                    ? 1
                                    : 0;
                        } else {
                            packet->blend_target_reversed =
                                backwards && (target_info->flags & CHARACTER_ANIMATION_FLAG_REVERSE_WITH_MOVEMENT) != 0
                                    ? 1
                                    : 0;
                            packet->blend_target_time = packet->blend_target_reversed != 0
                                                            ? NuAnimEndFrame(model->model_data_b[requested])
                                                            : 1.0f;
                        }
                        packet->blend_elapsed = 0.0f;
                        packet->blend_duration = source_info->blend_out_time < target_info->blend_in_time
                                                     ? source_info->blend_out_time
                                                     : target_info->blend_in_time;
                    }
                }
                packet->flags |= ANIMPACKET_FLAG_ANIMATION_CHANGED;
            } else if (packet->blending == 0 && requested == packet->previous_animation) {
                packet->animation_index = requested;
            }

            if (packet->blending == 0) {
                if (!HasAnimation(model, packet->animation_index)) {
                    const i16 requested_animation = packet->requested_animation;
                    ResetAnimPacket(packet, -1);
                    packet->requested_animation = requested_animation;
                    return;
                }
                const f32 step = (previous_flags & ANIMPACKET_FLAG_PAUSED) != 0 ? 0.0f : frame_step;
                packet->current_time = UpdateAnimationTimer(model, packet, packet->animation_index,
                                                            packet->current_time, step, movement_speed, true,
                                                            &packet->current_reversed, backwards, backwards_multiplier);
            } else if (HasAnimation(model, packet->blend_animation_a) &&
                       HasAnimation(model, packet->blend_animation_b)) {
                packet->blend_source_time = UpdateAnimationTimer(
                    model, packet, packet->blend_animation_a, packet->blend_source_time, frame_step, movement_speed,
                    false, &packet->blend_source_reversed, backwards, backwards_multiplier);
                packet->blend_target_time = UpdateAnimationTimer(
                    model, packet, packet->blend_animation_b, packet->blend_target_time, frame_step, movement_speed,
                    true, &packet->blend_target_reversed, backwards, backwards_multiplier);
            }
        } else if (HasAnimation(model, packet->requested_animation) && HasAnimation(model, packet->overlay_animation)) {
            packet->blend_source_time = UpdateAnimationTimer(
                model, packet, packet->requested_animation, packet->blend_source_time, frame_step, movement_speed,
                false, &packet->blend_source_reversed, backwards, backwards_multiplier);
            packet->blend_target_time = UpdateAnimationTimer(
                model, packet, packet->overlay_animation, packet->blend_target_time, frame_step, movement_speed, true,
                &packet->blend_target_reversed, backwards, backwards_multiplier);
        }
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

i32 GizmoFileReadGameAnimSet(GAMEANIMSET_s *set, void *world_ptr,
                             void (*read_object_data)(GAMEANIMOBJ_s *, unsigned char), unsigned char version,
                             char *prefix, char *suffix) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    const unsigned char file_version = static_cast<unsigned char>(EdFileReadChar());
    const unsigned char object_count = static_cast<unsigned char>(EdFileReadChar());
    i32 success = 1;

    for (i32 object_index = 0; object_index < object_count; ++object_index) {
        char object_name[64];
        const i32 name_length = static_cast<signed char>(EdFileReadChar());
        if (name_length != 0) {
            EdFileRead(object_name, name_length);
        }

        const f32 start_frame = EdFileReadFloat();
        const f32 end_frame = EdFileReadFloat();
        u32 flags = 0;
        if (file_version > 1) {
            flags = EdFileReadInt();
        }

        if (name_length == 0) {
            continue;
        }

        if (set->object_pool == NULL || set->object_pool->free_objects == NULL) {
            success = 0;
        }

        GAMEANIMOBJ_s *object = GameAnimSet_AddObjectByName(set, world->current_gscn, object_name, start_frame,
                                                            end_frame, 0, world->gizmo_sys, prefix, suffix);
        GAMEANIMOBJ_s missing_object;
        if (object == NULL) {
            memset(&missing_object, 0, sizeof(missing_object));
            object = &missing_object;
        }

        object->flags = flags;
        if (read_object_data != NULL) {
            read_object_data(object, version);
        }
        if (file_version <= 2) {
            object->flags &= ~2u;
        }
    }

    return success;
}

void ANI_SimpleAni3PlayerV4Joint_Quat3(ani3_animheader_s *, float, nuanimbuff_s *, i32, i32) {
}

void ANI_SimpleAni3PlayerV4Joint_Quat3W(ani3_animheader_s *, float, nuanimbuff_s *, i32, i32) {
}

void ANI_SimpleAni3PlayerV4Joint_Blend_Quat3(ani3_animheader_s *, float, nuanimbuff_s *, float, i32, i32, nuvec_s *) {
}

void ANI_SimpleAni3PlayerV4Joint_Blend_Quat3W(ani3_animheader_s *, float, nuanimbuff_s *, float, i32, i32, nuvec_s *) {
}

void EvalAnim(nuhspecial_s *special, float frame, numtx_s *matrix, i32 include_instance_translation) {
    if (matrix == NULL || special == NULL) {
        return;
    }

    nuinstanim_s *instance_animation = NuSpecialGetInstAnim(special);
    if (instance_animation == NULL) {
        if (include_instance_translation != 0) {
            NUMTX *instance_matrix = NuSpecialGetInstanceMtx(special);
            if (instance_matrix != NULL) {
                memcpy(matrix, instance_matrix, sizeof(NUMTX));
            }
        }
        return;
    }

    NUGSCN *scene = special->scene;
    nuanimdata_s *animation = scene->instance_animation_data[instance_animation->anim_ix];
    if (animation == NULL) {
        return;
    }

    NuAnimData2CalcMatrix(animation, 0, frame, matrix);
    if (include_instance_translation != 0) {
        NUMTX *instance_matrix = NuSpecialGetInstanceMtx(special);
        if (instance_matrix != NULL) {
            NUVEC *translation = NUMTX_GET_ROW_VEC(matrix, 3);
            const NUVEC *instance_translation = NUMTX_GET_ROW_VEC(instance_matrix, 3);
            translation->x += instance_translation->x;
            translation->y += instance_translation->y;
            translation->z += instance_translation->z;
        }
    }
}

void EvalAnim2(nuhspecial_s *special, float frame) {
    nuinstanim_s *instance_animation = NuSpecialGetInstAnim(special);
    if (instance_animation == NULL) {
        return;
    }

    NUGSCN *scene = special->scene;
    nuanimdata_s *animation = scene->instance_animation_data[instance_animation->anim_ix];
    if (animation == NULL || frame == instance_animation->prev_eval_time) {
        return;
    }

    NUMTX *instance_matrix = NuSpecialGetInstanceMtx(special);
    NuAnimData2CalcMatrix(animation, 0, frame, &instance_animation->mtx);

    instance_animation->prev_eval_time = frame;
    const i32 instance_index = instance_animation - scene->instance_animations;
    NUVEC &animation_translation = *NUMTX_GET_ROW_VEC(&instance_animation->mtx, 3);
    NUVEC &instance_translation = *NUMTX_GET_ROW_VEC(instance_matrix, 3);
    NUMTX *evaluated_matrix = &scene->instance_animation_matrices[instance_index];
    memcpy(evaluated_matrix, &instance_animation->mtx, sizeof(NUMTX));

    animation_translation.x += instance_translation.x;
    animation_translation.y += instance_translation.y;
    animation_translation.z += instance_translation.z;
    NuSpecialUpdate(special);
}

GAMEANIMSYS_s *GameAnimSys_Create(variptr_u *buf, variptr_u *buf_end) {
    GAMEANIMSYS_s *system = static_cast<GAMEANIMSYS_s *>(GameBufferAlloc(buf, buf_end, sizeof(GAMEANIMSYS_s)));
    if (system != NULL && gameanimsysprogress.entry_size != 0) {
        system->sets = static_cast<GAMEANIMSET_s **>(
            GameBufferAlloc(buf, buf_end, gameanimsysprogress.entry_size * sizeof(GAMEANIMSET_s *)));
    }
    return system;
}

void *GameAntnode_CreateSys(WORLDINFO_s *world, variptr_u *buf, variptr_u *buf_end, i32 count) {
    (void)world;
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
