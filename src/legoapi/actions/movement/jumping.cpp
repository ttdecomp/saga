#include "decomp.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

enum PLAYER_JUMP_ACTION : i16 {
    PLAYER_JUMP_ACTION_FALL = 5,
    PLAYER_JUMP_ACTION_JUMP = 6,
    PLAYER_JUMP_ACTION_LAND = 7,
    PLAYER_JUMP_ACTION_SECOND_JUMP = 9,
    PLAYER_JUMP_ACTION_LAND_2 = 10,
    PLAYER_JUMP_ACTION_THIRD_JUMP = 14,
    PLAYER_JUMP_ACTION_FALL_LAND = 89,
    PLAYER_JUMP_ACTION_BACKPACK_FALL_LAND = 179,
};

enum PLAYER_JUMP_MOVEMENT_STATE : u8 {
    PLAYER_JUMP_MOVEMENT_BASIC = 0,
    PLAYER_JUMP_MOVEMENT_ORDINARY = 5,
};

enum PLAYER_JUMP_RUNTIME_FLAGS : u8 {
    PLAYER_JUMP_RUNTIME_BUTTON_HELD = 0x10,
};

enum PLAYER_JUMP_INPUT_FLAGS : u8 {
    PLAYER_JUMP_INPUT_BUFFERED = 0x10,
};

enum PLAYER_JUMP_FLAGS : u8 {
    PLAYER_JUMP_FLAG_SPECIAL_TAKEOFF = 0x01,
};

enum PLAYER_JUMP_CONTEXT_FLAGS : u32 {
    PLAYER_JUMP_CONTEXT_ALLOW_START = 0x00001000,
};

enum PLAYER_JUMP_VARIANT_FLAGS : u8 {
    PLAYER_JUMP_VARIANT_BUTTON_RELEASED = 0x10,
    PLAYER_JUMP_VARIANT_SECOND_JUMP = 0x40,
    PLAYER_JUMP_VARIANT_FALLING = 0x80,
    PLAYER_JUMP_VARIANT_START_CLEAR = 0x90,
    PLAYER_JUMP_VARIANT_END_CLEAR = 0x50,
};

enum PLAYER_MOVEMENT_RUNTIME_FLAGS : u8 {
    PLAYER_MOVEMENT_RUNTIME_DISABLE_JUMP_CODE = 0x10,
};

enum PLAYER_JUMP_ANIMATION_FLAGS : u32 {
    PLAYER_JUMP_ANIMATION_ALLOW_DOUBLE_JUMP = 0x0008,
    PLAYER_JUMP_ANIMATION_USE_THIRD_JUMP = 0x0010,
};

static const f32 PLAYER_JUMP_MINIMUM_AIR_TIME = 0.1f;
static const f32 PLAYER_JUMP_REENTRY_DELAY = 0.2f;

void PlayJumpSfx(GameObject_s *object, i32 variant);
void PlayLandSfx(GameObject_s *object, i32 variant, i32 force);

void StartJump(GameObject_s *object, i32 movement_state);

static GAMECHARACTERDATA *Jump_GetCharacterData(GameObject_s *object) {
    if (object == NULL || object->apiobj.character_data == NULL) {
        return NULL;
    }
    return static_cast<GAMECHARACTERDATA *>(object->apiobj.character_data->field11_0x24);
}

static bool Jump_HasAction(const GameObject_s *object, PLAYER_JUMP_ACTION action) {
    return object != NULL && object->apiobj.character_model != NULL &&
           object->apiobj.character_model->model_data_b != NULL &&
           object->apiobj.character_model->model_data_b[action] != NULL;
}

void BigJumpCode(GameObject_s *) {
}

bool UseFallAnim(GameObject_s *object) {
    const CHARACTER_CONTEXT_INFO_s &context = CInfo[object->character_context];
    return (context.flags & CHARACTER_CONTEXT_INFO_FLAG_USE_FALL_ANIMATION) != 0 &&
           Jump_HasAction(object, PLAYER_JUMP_ACTION_FALL);
}

void StartBigJump(GameObject_s *, nuvec_s *, i32, float, float, i32, signed char) {
}

void StartFallLand(GameObject_s *object, i32 action) {
    if (object == NULL) {
        return;
    }

    PlayLandSfx(object, 0, 0);

    PLAYER_JUMP_ACTION landing_action = PLAYER_JUMP_ACTION_LAND;
    if (action >= 0 && Jump_HasAction(object, static_cast<PLAYER_JUMP_ACTION>(action))) {
        landing_action = static_cast<PLAYER_JUMP_ACTION>(action);
    } else if (Jump_HasAction(object, PLAYER_JUMP_ACTION_FALL_LAND)) {
        landing_action = PLAYER_JUMP_ACTION_FALL_LAND;
    } else if (Jump_HasAction(object, PLAYER_JUMP_ACTION_LAND_2)) {
        landing_action = PLAYER_JUMP_ACTION_LAND_2;
    }

    if (!Jump_HasAction(object, landing_action)) {
        object->context_flags &= ~0x04;
        return;
    }

    object->character_context = CHARACTER_CONTEXT_LAND_JUMP;
    object->context_animation = landing_action;
    ResetAnimPacket(&object->apiobj.anim_packet, -1);
    object->fall_animation_timer = 0.0f;
    object->context_animation_timer = 0.0f;
    object->movement_runtime_flags =
        static_cast<u8>((object->movement_runtime_flags & ~0x08) | ((object->movement_runtime_flags & 0x04) << 1));
}

void StartEndOfJump(GameObject_s *object) {
    if (object == NULL) {
        return;
    }

    object->character_context = CHARACTER_CONTEXT_JUMP;
    object->action_movement_state = PLAYER_JUMP_MOVEMENT_BASIC;
    object->jump_sequence = 2;
    object->jump_flags &= ~PLAYER_JUMP_FLAG_SPECIAL_TAKEOFF;
    object->context_animation = PLAYER_JUMP_ACTION_FALL;
    object->context_variant_flags =
        static_cast<i8>((static_cast<u8>(object->context_variant_flags) | PLAYER_JUMP_VARIANT_FALLING) &
                        ~PLAYER_JUMP_VARIANT_END_CLEAR);
    object->airborne_reset_timer = 0.0f;
}

void StartBallooning(GameObject_s *, i32) {
}

void StartJetPackFall(GameObject_s *, i32) {
}

void MakeJumpReachHeight(GameObject_s *object, float height, i32 force) {
    const f32 remaining_height = height - (object->apiobj.position.y - object->jump_start_height);
    if (remaining_height > 0.0f) {
        const GAMECHARACTERDATA *game_character =
            static_cast<GAMECHARACTERDATA *>(object->apiobj.character_data->field11_0x24);
        const f32 vertical_speed = NuFsqrt(-2.0f * game_character->gravity * remaining_height);
        if (force != 0 || vertical_speed > object->apiobj.velocity.y) {
            object->apiobj.velocity.y = vertical_speed;
        }
    } else if (force != 0) {
        object->apiobj.velocity.y = 0.0f;
    }
}

void SetBallooningHeight(GameObject_s *, float) {
}

void JumpCode(GameObject_s *object, i32 jump_pressed, i32 jump_held, u32 animation_set, i32, i32, i32) {
    if (object == NULL || (object->movement_runtime_flags & PLAYER_MOVEMENT_RUNTIME_DISABLE_JUMP_CODE) != 0) {
        return;
    }

    if (object->character_context != CHARACTER_CONTEXT_JUMP) {
        const bool has_ground_contact = object->apiobj.field_0x27d != 0 || object->ground_contact_grace_timer > 0.0f;
        const bool context_allows_jump =
            (CInfo[object->character_context].flags & PLAYER_JUMP_CONTEXT_ALLOW_START) != 0;
        if (jump_pressed != 0 && has_ground_contact && context_allows_jump) {
            const PLAYER_JUMP_MOVEMENT_STATE movement_state =
                (animation_set & 0x04) != 0 ? PLAYER_JUMP_MOVEMENT_ORDINARY : PLAYER_JUMP_MOVEMENT_BASIC;
            StartJump(object, movement_state);
        }
    } else {
        if (jump_held == 0) {
            object->field_0xe22 &= ~PLAYER_JUMP_RUNTIME_BUTTON_HELD;
        }

        if (object->apiobj.velocity.y > 0.0f) {
            object->airborne_reset_timer = 0.0f;
        }
        object->context_animation_timer += FRAMETIME;

        GAMECHARACTERDATA *game_character = Jump_GetCharacterData(object);
        const bool buffered_second_jump = (object->jump_input_flags & PLAYER_JUMP_INPUT_BUFFERED) != 0;
        const bool can_start_second_jump = (jump_pressed != 0 || buffered_second_jump) &&
                                           (animation_set & PLAYER_JUMP_ANIMATION_ALLOW_DOUBLE_JUMP) != 0 &&
                                           object->action_movement_state == PLAYER_JUMP_MOVEMENT_BASIC &&
                                           object->jump_sequence <= 1 &&
                                           (object->apiobj.velocity.y > 0.0f || buffered_second_jump);
        if (can_start_second_jump && game_character != NULL) {
            MakeJumpReachHeight(object, game_character->second_jump_height, 0);
            object->jump_sequence++;
            object->context_variant_flags |= PLAYER_JUMP_VARIANT_SECOND_JUMP;
            object->action_movement_state = PLAYER_JUMP_MOVEMENT_BASIC;
            PLAYER_JUMP_ACTION second_jump_action = PLAYER_JUMP_ACTION_JUMP;
            if ((animation_set & PLAYER_JUMP_ANIMATION_USE_THIRD_JUMP) != 0 &&
                Jump_HasAction(object, PLAYER_JUMP_ACTION_THIRD_JUMP)) {
                second_jump_action = PLAYER_JUMP_ACTION_THIRD_JUMP;
            } else if (Jump_HasAction(object, PLAYER_JUMP_ACTION_SECOND_JUMP)) {
                second_jump_action = PLAYER_JUMP_ACTION_SECOND_JUMP;
            }
            object->context_animation = second_jump_action;
            object->context_animation_timer = 0.0f;
            object->field_0xe22 |= PLAYER_JUMP_RUNTIME_BUTTON_HELD;
            PlayJumpSfx(object, 1);
            if ((animation_set & PLAYER_JUMP_ANIMATION_USE_THIRD_JUMP) == 0) {
                object->context_variant_flags |= PLAYER_JUMP_VARIANT_BUTTON_RELEASED;
            }
            return;
        }

        if (object->apiobj.field_0x27d != 0 && object->context_animation_timer >= PLAYER_JUMP_MINIMUM_AIR_TIME) {
            if (object->action_movement_state == PLAYER_JUMP_MOVEMENT_ORDINARY) {
                object->character_context = CHARACTER_CONTEXT_NONE;
                object->jump_reentry_timer = PLAYER_JUMP_REENTRY_DELAY;
                object->jump_chain_timer = 0.0f;
                PlayLandSfx(object, 0, 0);
            } else {
                StartFallLand(object, -1);
            }
        }
        return;
    }

    if (object->character_context == CHARACTER_CONTEXT_LAND_JUMP) {
        object->context_animation_timer -= FRAMETIME;
        if (object->context_animation_timer <= 0.0f) {
            object->character_context = CHARACTER_CONTEXT_NONE;
        }
    }
}

void StartJump(GameObject_s *object, i32 movement_state) {
    GAMECHARACTERDATA *game_character = Jump_GetCharacterData(object);
    if (object == NULL || game_character == NULL) {
        return;
    }

    object->character_context = CHARACTER_CONTEXT_JUMP;
    object->action_movement_state = static_cast<u8>(movement_state);
    object->jump_sequence = 1;
    object->jump_start_height = object->apiobj.position.y;
    object->context_animation_timer = 0.0f;
    object->context_animation = PLAYER_JUMP_ACTION_JUMP;
    object->jump_flags =
        static_cast<u8>((object->jump_flags & ~PLAYER_JUMP_FLAG_SPECIAL_TAKEOFF) |
                        ((movement_state == 3 || movement_state == 4) ? PLAYER_JUMP_FLAG_SPECIAL_TAKEOFF : 0));
    object->context_variant_flags =
        static_cast<i8>(static_cast<u8>(object->context_variant_flags) & ~PLAYER_JUMP_VARIANT_START_CLEAR);
    object->airborne_action_timer = 0.0f;
    if (movement_state != 6 && movement_state != 7) {
        PlayJumpSfx(object, 0);
    }
    object->apiobj.field_0x27d = 0;
    object->field_0x105c = 0;
    object->field_0xe22 |= PLAYER_JUMP_RUNTIME_BUTTON_HELD;
    object->delayed_turn_timer = 0.0f;
    object->airborne_input_timer = 0.0f;
    object->airborne_reset_timer = 0.0f;
    object->apiobj.velocity.y = game_character->jump_speed;
    ResetAnimPacket(&object->apiobj.anim_packet, -1);
    object->apiobj.pitch_angle = 0;
    object->apiobj.roll_angle = 0;
    object->field_0x1086 = 2;
}
