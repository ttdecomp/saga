#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void GameAudio_PlaySfx(i32 sfx, NUVEC *position, i32 flags, i32 volume);

void LoseHelmet(GameObject_s *, i32, i32) {
}

void SetWeaponIn(GameObject_s *) {
}

void FastWeaponIn(GameObject_s *, i32) {
}

void KeepWeaponIn(GameObject_s *) {
}

void SetWeaponOut(GameObject_s *object) {
    const i8 context = object->character_context;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT)) {
        object->character_context = -1;
    }
    object->weapon_scale = 1.0f;
    object->weapon_out_timer = 0.0f;
    object->field_0xe22 |= GAMEOBJECT_E22_FLAG_WEAPON_ANIMATION;
    object->weapon_scale_state = WEAPON_SCALE_IDLE;
}

void SlowWeaponIn(GameObject_s *) {
}

void WeaponInCode(GameObject_s *) {
}

void FastWeaponOut(GameObject_s *object, i32 force_sound) {
    const i8 context = object->character_context;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT)) {
        object->character_context = -1;
    }

    if (force_sound != 0 && object->weapon_scale == 0.0f && object->weapon_scale_state != WEAPON_SCALE_EXTENDING) {
        const i32 current_animation = CurrentAnim(&object->apiobj.anim_packet);
        CHARACTERANIM_s *animation = NULL;
        if (current_animation != -1) {
            animation = static_cast<CHARACTERANIM_s *>(object->apiobj.character_model->model_data_a[current_animation]);
        }

        if (animation == NULL || (animation->flags & CHARACTER_ANIMATION_FLAG_ALLOW_WEAPON_TRANSITION) == 0) {
            const u32 model_flags = object->apiobj.character_data->model_flags;
            if ((model_flags & CHARACTER_MODEL_FLAG_JEDI) != 0) {
                if (object->apiobj.field_0x27c != -1 || WeaponInOut_NoAIJediSfx == 0) {
                    GameAudio_PlaySfx(0x3e, &object->apiobj.collision_position, GameAudio_GetPlrSfxBits(object), 1);
                }
            } else if ((model_flags & CHARACTER_MODEL_FLAG_ALTERNATE_WEAPON) != 0) {
                GameAudio_PlaySfx(0x43, &object->apiobj.collision_position, 0, 1);
            }
        }
    }

    object->weapon_scale_rate = 5.0f;
    object->weapon_out_timer = 0.0f;
    object->weapon_scale_state = WEAPON_SCALE_EXTENDING;
}

void KeepWeaponOut(GameObject_s *object) {
    object->weapon_scale = 1.0f;
    object->field_0xe22 |= GAMEOBJECT_E22_FLAG_WEAPON_ANIMATION;
    object->weapon_scale_state = WEAPON_SCALE_IDLE;
    object->field_0xef8 |= GAMEOBJECT_EF8_FLAG_KEEP_WEAPON_OUT;
}

void ReleaseHearts() {
}

void SlowWeaponOut(GameObject_s *) {
}

void WeaponOutCode(GameObject_s *) {
}

void AutoWeaponOnOff(GameObject_s *) {
}

void RegenerateHearts(GameObject_s *) {
}

void WeaponScalingCode(GameObject_s *object) {
    switch (object->weapon_scale_state) {
        case WEAPON_SCALE_EXTENDING:
            object->weapon_scale = SeekLinearF(object->weapon_scale, 1.0f, object->weapon_scale_rate * FRAMETIME);
            if (object->weapon_scale >= 1.0f) {
                object->weapon_scale_state = WEAPON_SCALE_IDLE;
                object->field_0xe22 |= GAMEOBJECT_E22_FLAG_WEAPON_ANIMATION;
            }
            break;
        case WEAPON_SCALE_RETRACTING:
            object->weapon_scale = SeekLinearF(object->weapon_scale, 0.0f, object->weapon_scale_rate * FRAMETIME);
            if (object->weapon_scale <= 0.0f) {
                object->weapon_scale_state = WEAPON_SCALE_IDLE;
                object->field_0xe22 &= ~GAMEOBJECT_E22_FLAG_WEAPON_ANIMATION;
            }
            break;
        default:
            break;
    }
}

void FindPlayerAndSetWeapon(i32, i32) {
}
