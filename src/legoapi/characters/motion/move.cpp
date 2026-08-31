#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

void MovePlayer_DIRECTIONAL(GameObject_s *object);
void MovePlayer_VEHICLEDIRECTIONAL(GameObject_s *object);
i32 MovePlayer_TWIST(GameObject_s *object);
i32 MovePlayer_CIRCLE(GameObject_s *object);
i32 MovePlayer_GUNSHIPIN(GameObject_s *object);
i32 MovePlayer_POD(GameObject_s *object);
void ApplyGravity(GameObject_s *object, float *gravity, float terminal_velocity, float gravity_scale,
                  float *vertical_velocity);
void GameObjectOrigin(GameObject_s *object);
i32 Grapple_LookAtPos(GameObject_s *object, NUVEC *position);
NUVEC *Technos_TgtPos(TECHNO_s *techno);
void GameCam_UpdateLookRot(GAMECAMERA_s *camera);
u16 SeekRot(u16 current, u16 target, f32 rate);
void SeekVec(NUVEC *result, NUVEC *current, NUVEC *target, f32 rate);
void ShoveSystemCheckGameObject(GameObject_s *object);
void Techno_FindOperator(void *techno, GAMEPAD_s **pad, GameObject_s **operator_object);
bool MovingBackwards(GameObject_s *object);
void GizmoBlowupCheckProximity(WORLDINFO_s *world, GameObject_s *object);
void KeepWeaponOut(GameObject_s *object);
void DropInOutCode(GameObject_s *object);
void Signal_MoveCode(WORLDINFO_s *world, GameObject_s *object);
void TakeHitCode(GameObject_s *object);
void FloatCode(GameObject_s *object);
void SlideCode(GameObject_s *object);
void FlattenCode(GameObject_s *object);
void Hang_MoveCode(GameObject_s *object);
void Ledge_MoveCode(WORLDINFO_s *world, GameObject_s *object);
void LedgeTerrain_MoveCode(GameObject_s *object);
void Climb_MoveCode(GameObject_s *object);
void TightRope_MoveCode(GameObject_s *object, i32 jump_pressed);
void ForcedBackCode(GameObject_s *object);
void Tube_MoveCode(GameObject_s *object, WORLDINFO_s *world);
void PushCode(GameObject_s *object, i32 allow_grab);
void BackFlipCode(GameObject_s *object);
void TakeOverCode(GameObject_s *object, i32 tag_pressed);
void Glide_MoveCode(GameObject_s *object);
void JumpCode(GameObject_s *object, i32 jump_pressed, i32 jump_held, u32 animation_set, i32 action_pressed,
              i32 action_held, i32 special_animation);
void GizPanel_MoveCode(WORLDINFO_s *world, GameObject_s *object, i32 special_pressed);
void HatMachine_MoveCode(WORLDINFO_s *world, GameObject_s *object, i32 special_pressed);
void ZipUp_MoveCode(GameObject_s *object, i32 special_pressed);
void BuildIt_MoveCode(GameObject_s *object);
void Lever_MoveCode(WORLDINFO_s *world, GameObject_s *object);
void ThermalDetonator_MoveCode(GameObject_s *object);
void Teleport_MoveCode(GameObject_s *object, i32 special_pressed);
void ComboRotateCode(GameObject_s *object, i32 action_held);
void WeaponOutCode(GameObject_s *object);
void WeaponInCode(GameObject_s *object);
void WeaponScalingCode(GameObject_s *object);
void HoldCode(GameObject_s *object);
void HeadMovement(GameObject_s *object);
void CloakMovement(GameObject_s *object);
void HairMovement(GameObject_s *object);
extern i32 newgamecam;
extern f32 newgamecamtime;
extern FadeSystem FadeSys;
i32 GetMenuID(void);

extern "C" {
    extern i16 id_BODYGUARD;
    extern i16 id_GRIEVOUS;
}

enum CHARACTER_CONTEXT : i8 {
    CHARACTER_CONTEXT_JUMP = 0,
    CHARACTER_CONTEXT_BUILD_IT = 0x2d,
    CHARACTER_CONTEXT_NONE = -1,
};

static GAMECHARACTERDATA *GetGameCharacterData(GameObject_s *object) {
    if (object == NULL || object->apiobj.character_data == NULL) {
        return NULL;
    }
    return static_cast<GAMECHARACTERDATA *>(object->apiobj.character_data->field11_0x24);
}

static i32 RotationDistance(u16 current, u16 target) {
    const i32 difference = RotDiff(current, target);
    return difference < 0 ? -difference : difference;
}

static void ShiftPadDirectionHistory(GameObject_s *object, GAMEPAD_s *pad) {
    i32 player_index = -1;
    if (Player[0] == object) {
        player_index = 0;
    } else if (Player[1] == object) {
        player_index = 1;
    }
    if (player_index == -1) {
        return;
    }

    PadOldSpeed2[player_index] = PadOldSpeed[player_index];
    PadOldAngle2[player_index] = PadOldAngle[player_index];
    PadOldSpeed[player_index] = pad->input_magnitude;
    PadOldAngle[player_index] = pad->input_angle;
}

void MoveBlocks(WORLDINFO_s *, pushblock_s *, i32, nuvec_s *) {
}

void MovePlayer(GameObject_s *object) {
    GAMECHARACTERDATA *game_character = GetGameCharacterData(object);
    if (object == NULL || object->pad_gamepad == NULL || game_character == NULL) {
        return;
    }

    GAMEPAD_s *pad = object->pad_gamepad;
    APIOBJECT &api = object->apiobj;

    object->field_0xe22 &= ~GAMEOBJECT_E22_FLAG_INPUT_ANGLE_VALID;
    if (pad->input_magnitude != pad->previous_input_magnitude) {
        pad->allocated_5a |= GAMEPAD_RUNTIME_MAGNITUDE_CHANGED;
    }

    api.field_0x214 = api.field_0x218;
    api.start_position = api.position;
    api.initial_position = {api.pos_x, api.pos_y, api.pos_z};
    api.field_0x27e = api.field_0x27d;
    pad->previous_input_angle = pad->input_angle;
    pad->previous_input_magnitude = pad->input_magnitude;
    object->field_0xefd &= ~GAMEOBJECT_MOVEMENT_FLAG_BACKWARDS;
    object->previous_movement_angle = api.field_0x276;
    object->field_0xf1c = api.model_draw_result == 0 ? object->field_0xf1c + FRAMETIME : 0.0f;

    if ((object->field_0xf03 & 4) != 0) {
        APIObjectVelocities(object);
        GameObjectOrigin(object);
        return;
    }
    if (api.field_0x287 != 0) {
        return;
    }

    pad->allocated_5a &= ~GAMEPAD_RUNTIME_WAGGLED;
    pad->waggle_magnitude = 0.0f;
    object->field_0x107a = -1;
    object->field_0xefd &= ~GAMEOBJECT_MOVEMENT_FLAG_FACE_REVERSED;

    f32 input_x = 0.0f;
    f32 input_z = 0.0f;
    const bool accepts_player_input = (api.field_0x1f8 & APIOBJECT_FLAG_AI_PLAYER_MASK) == APIOBJECT_FLAG_PLAYER_ACTIVE;
    const i32 menu_id = GetMenuID();
    const bool input_blocked =
        FadeSys.fade > 0.0f || newgamecam != 0 ||
        (static_cast<i8>(api.flags_low) < 0 && (MiniCutCam == 2 || (menu_id != 0x0e && menu_id != -1)));
    if (accepts_player_input && !input_blocked) {
        const u32 dpad = pad->buttons_held & (GAMEPAD_DLEFT | GAMEPAD_DRIGHT | GAMEPAD_DUP | GAMEPAD_DDOWN);
        if (dpad != 0) {
            input_x = (dpad & GAMEPAD_DLEFT) != 0 ? -1.0f : (dpad & GAMEPAD_DRIGHT) != 0 ? 1.0f : 0.0f;
            input_z = (dpad & GAMEPAD_DDOWN) != 0 ? -1.0f : (dpad & GAMEPAD_DUP) != 0 ? 1.0f : 0.0f;
            pad->input_state = 1;
        } else if (pad->pad != NULL) {
            static const f32 kAnalogCentre = 127.5f;
            static const f32 kAnalogDeadzoneSquared = 1806.25f;
            const f32 raw_x = static_cast<f32>(pad->pad->analog_left_x) - kAnalogCentre;
            const f32 raw_z = kAnalogCentre - static_cast<f32>(pad->pad->analog_left_y);
            if (raw_x * raw_x + raw_z * raw_z >= kAnalogDeadzoneSquared) {
                input_x = raw_x / kAnalogCentre;
                input_z = raw_z / kAnalogCentre;
                pad->input_state = 2;
            }
        }
    } else {
        pad->input_state = 1;
    }

    ShiftPadDirectionHistory(object, pad);
    const f32 stick_magnitude = NuFsqrt(input_x * input_x + input_z * input_z);
    pad->input_direction_x = input_x;
    pad->input_direction_z = input_z;
    pad->input_magnitude = 0.0f;
    if (stick_magnitude >= 0.2f) {
        if (stick_magnitude < 0.5f && (game_character->flags_090 & 0x08) == 0) {
            pad->input_magnitude = game_character->tiptoe_speed;
        } else if (stick_magnitude < 0.8f) {
            pad->input_magnitude = game_character->walk_speed;
        } else {
            pad->input_magnitude = game_character->run_speed;
        }
        if (pad->input_magnitude > 0.0f) {
            pad->input_angle = NuAtan2D(input_x, input_z);
        }
    }

    const i32 waggle = GamePad_Waggle(pad);
    pad->allocated_5a =
        static_cast<u8>((pad->allocated_5a & ~GAMEPAD_RUNTIME_WAGGLED) | (waggle != 0 ? GAMEPAD_RUNTIME_WAGGLED : 0));
    pad->waggle_magnitude = GamePad_Rotate(object);
    pad->animation_input_magnitude = pad->input_magnitude;

    object->field_0x1086 = 2;
    ShoveSystemCheckGameObject(object);
    if (MovePlayer_TWIST(object) == 0 && MovePlayer_CIRCLE(object) == 0 && MovePlayer_GUNSHIPIN(object) == 0 &&
        MovePlayer_POD(object) == 0) {
        if ((api.character_data->model_flags & 0x2000) == 0) {
            MovePlayer_DIRECTIONAL(object);
        } else {
            MovePlayer_VEHICLEDIRECTIONAL(object);
        }
    }

    api.field_0x1fa |= 1;
    api.previous_velocity = api.velocity;
    APIObjectVelocities(object);
    GameObjectOrigin(object);
}

void Move_BEAST(GameObject_s *) {
}

void Move_BARMAN(GameObject_s *) {
}

void Move_CANNON(GameObject_s *) {
}

void Move_WALKER(GameObject_s *) {
}

void Move_WEIRDO(GameObject_s *) {
}

void Move_CRITTER(GameObject_s *) {
}

void Move_DEFAULT(GameObject_s *) {
}

void Move_DRAGBOMB(GameObject_s *) {
}

void Move_DROIDEKA(GameObject_s *) {
}

static void PlayerCamPos(GameObject_s *object, NUVEC *camera_position, NUVEC *) {
    CHARACTERDATA *character = object->apiobj.character_data;
    f32 centre_height = (character->field15_0x34 + character->field16_0x38) * character->field17_0x3c * 0.5f;

    // This is the common character path in the original. Special vehicles,
    // grapples and targetable objects select a different focus point below
    // this branch; ordinary hub characters use their world position plus the
    // vertical centre of the configured character bounds.
    *camera_position = object->apiobj.position;
    camera_position->y += centre_height;
}

static void SetGameCameraView(GAMECAMERA_s *camera, const NUVEC &position, const NUVEC &target, bool snap_angles) {
    NUVEC delta;
    NuVecSub(&delta, const_cast<NUVEC *>(&target), const_cast<NUVEC *>(&position));

    const u16 desired_pitch = static_cast<u16>(-NuAtan2D(delta.y, NuFsqrt(delta.x * delta.x + delta.z * delta.z)));
    const u16 desired_yaw = static_cast<u16>(NuAtan2D(delta.x, delta.z));
    camera->desired_pitch = desired_pitch;
    camera->desired_yaw = desired_yaw;
    camera->desired_roll = 0;

    if (snap_angles) {
        camera->pitch = desired_pitch;
        camera->yaw = desired_yaw;
        camera->roll = 0;
    } else {
        camera->pitch = SeekRot(static_cast<u16>(camera->pitch), desired_pitch, camera->angle_seek);
        camera->yaw = SeekRot(static_cast<u16>(camera->yaw), desired_yaw, camera->angle_seek);
        camera->roll = SeekRot(static_cast<u16>(camera->roll), 0, camera->angle_seek);
    }

    camera->pos = position;
    camera->target = target;
    GameCam_UpdateLookRot(camera);

    const NUANG render_pitch = camera->pitch + static_cast<u16>(static_cast<i32>(camera->field_0x214));
    const NUANG render_yaw = camera->yaw + static_cast<u16>(static_cast<i32>(camera->field_0x218));

    NuMtxSetRotationZ(&camera->mtx, camera->roll);
    NuMtxRotateX(&camera->mtx, render_pitch);
    NuMtxRotateY(&camera->mtx, render_yaw);
    NuMtxTranslate(&camera->mtx, const_cast<NUVEC *>(&position));
    camera->render_mtx = camera->mtx;

    NuMtxSetRotationZ(&camera->target_mtx, camera->roll);
    NuMtxRotateX(&camera->target_mtx, camera->pitch);
    NuMtxRotateY(&camera->target_mtx, camera->yaw);
    NuMtxTranslate(&camera->target_mtx, const_cast<NUVEC *>(&position));

    if (pNuCam != NULL) {
        pNuCam->mtx = camera->render_mtx;
        NuCameraSet(pNuCam);
    }
}

void MoveGameCamera(GAMECAMERA_s *camera) {
    // Original title-camera mode (3): portal_places[2] contains one camera
    // position followed by its look target.  The complete function selects
    // many gameplay camera modes; only this currently reachable mode is
    // transcribed here.
    if (camera == NULL || WORLD == NULL || WORLD->current_level == NULL) {
        return;
    }

    if (WORLD->current_level == TITLES_LDATA) {
        if (WORLD->portal_places == NULL || WORLD->portal_places[2] == NULL ||
            WORLD->portal_places[2]->positions == NULL) {
            return;
        }
        f32 *points = WORLD->portal_places[2]->positions;
        NUVEC position = {points[0], points[1], points[2]};
        NUVEC target = {points[3], points[4], points[5]};
        camera->mode = 3;
        camera->desired_position = position;
        SetGameCameraView(camera, position, target, true);
        camera->previous_mode = camera->mode;
        return;
    }

    // New-game input remains locked while the initial portal-camera pair is
    // available, but never for more than ten seconds.  This is the target's
    // ordinary-camera prologue: without it `newgamecam` remains set forever
    // and MovePlayer correctly rejects every stick/D-pad sample.
    if (newgamecam != 0) {
        constexpr f32 kNewGameCameraTimeout = 10.0f;
        newgamecamtime += FRAMETIME;
        const bool initial_camera_missing =
            WORLD->portal_places == NULL || WORLD->portal_places[6] == NULL || WORLD->portal_places[7] == NULL;
        if (newgamecamtime >= kNewGameCameraTimeout || initial_camera_missing) {
            newgamecam = 0;
        }
    }

    // The original selects its ordinary free-camera mode when no socket
    // system is present and its rail-camera mode otherwise.  Both modes use
    // the same player-focus path below.
    camera->mode = WORLD->sock_sys == NULL ? 0 : 1;
    const bool mode_changed = camera->mode != camera->previous_mode;
    NUVEC player_camera_positions[2];
    NUVEC player_positions[2];
    i32 player_count = 0;
    for (i32 i = 0; i < 2; ++i) {
        if (Player[i] == NULL) {
            continue;
        }
        PlayerCamPos(Player[i], &player_camera_positions[player_count], &camera->pos);
        player_positions[player_count] = Player[i]->apiobj.position;
        ++player_count;
    }
    if (player_count == 0) {
        return;
    }

    NUVEC position = camera->pos;
    NUVEC target = {0.0f, 0.0f, 0.0f};
    for (i32 i = 0; i < player_count; ++i) {
        NuVecAdd(&target, &target, &player_camera_positions[i]);
    }
    NuVecScale(&target, &target, 1.0f / static_cast<f32>(player_count));

    f32 overlap_blend = 0.0f;
    f32 position_seek = camera->position_seek;
    f32 angle_seek = camera->angle_seek;
    f32 terrain_clearance = 0.0f;
    f32 separation_scale = 0.0f;
    SockSysCamera(WORLD->sock_sys, &camera->pos, camera->mode != camera->previous_mode, player_camera_positions,
                  player_positions, player_count, &camera->sock_position, &position, &target, &overlap_blend,
                  &position_seek, &angle_seek, &terrain_clearance, &separation_scale);
    camera->position_seek = position_seek;
    camera->angle_seek = angle_seek;
    camera->desired_position = position;

    if (!mode_changed) {
        NUVEC smoothed_position;
        SeekVec(&smoothed_position, &camera->pos, &position, camera->position_seek);
        position = smoothed_position;
    }

    // On the no-rail path SockSysCamera deliberately retains the previous
    // camera position while still producing the current player focus.  Its
    // return value is not a validity gate; both paths continue into the
    // common matrix update.
    SetGameCameraView(camera, position, target, mode_changed);
    camera->previous_mode = camera->mode;
}

i32 MovePlayer_POD(GameObject_s *) {
    return 0;
}

void Move_CHARACTER(GameObject_s *object) {
    if (object == NULL) {
        return;
    }
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
}

void Move_GEONOSIAN(GameObject_s *) {
}

void Move_HOVERDROID(GameObject_s *) {
}

void MovePlayerSpline(GameObject_s *) {
}

i32 MovePlayer_TWIST(GameObject_s *) {
    return 0;
}

void MoveSubItemsLeft(i32 *, nuvec_s *, i32) {
}

void Move_SPEEDERBIKE(GameObject_s *) {
}

i32 MovePlayer_CIRCLE(GameObject_s *) {
    return 0;
}

void MoveSubItemsRight(i32 *, nuvec_s *, i32) {
}

void Move_DROIDGENERIC(GameObject_s *) {
}

void MovePlayer_NETWORK(GameObject_s *) {
}

void MovePlayer_ROLLING(GameObject_s *) {
}

void MoveSplinePosition(SPLINEPOS_s *, float) {
}

void MoveBlocksOverBlock(WORLDINFO_s *, pushblock_s *, i32, nuvec_s *) {
}

void MoveInactiveVehicle(GameObject_s *, i32, GameObject_s **) {
}

i32 MovePlayer_GUNSHIPIN(GameObject_s *) {
    return 0;
}

void Move_REPUBLICGUNSHIP(GameObject_s *) {
}

void Move_SUPERBATTLEDROID(GameObject_s *) {
}

void MovePlayer_DIRECTIONAL(GameObject_s *object) {
    GAMECHARACTERDATA *game_character = GetGameCharacterData(object);
    if (object == NULL || object->pad_gamepad == NULL || game_character == NULL) {
        return;
    }

    GAMEPAD_s *pad = object->pad_gamepad;
    GameObject_s *operator_object = object;
    Techno_FindOperator(object, &pad, &operator_object);

    GAMECHARACTERDATA *operator_character = GetGameCharacterData(operator_object);
    if (operator_character == NULL || operator_character->run_speed == 0.0f) {
        operator_character = game_character;
        operator_object = object;
    }
    const f32 desired_speed = game_character->run_speed * (pad->input_magnitude / operator_character->run_speed);

    object->field_0xe23 &= ~0x10;
    if (MovingBackwards(object)) {
        object->field_0xefd |= GAMEOBJECT_MOVEMENT_FLAG_BACKWARDS;
    }

    APIOBJECT &api = object->apiobj;
    const u16 input_angle = GameCam != NULL ? GamePad_InputAngle(object, pad) : pad->input_angle;
    object->field_0xe22 |= GAMEOBJECT_E22_FLAG_INPUT_ANGLE_VALID;
    object->current_input_angle = input_angle;

    if (object->delayed_turn_timer > 0.0f) {
        if (pad->input_magnitude > 0.0f) {
            object->delayed_turn_target_angle = input_angle;
        }
        object->delayed_turn_timer -= FRAMETIME;
        if (object->delayed_turn_timer <= 0.0f) {
            api.movement_facing_angle = object->delayed_turn_target_angle;
        }
    } else {
        const i8 context = object->build_context;
        const bool ordinary_context = context == CHARACTER_CONTEXT_NONE || context == 1;
        const bool has_turn_animation =
            api.character_model != NULL && api.character_model->model_data_b != NULL &&
            (api.character_model->model_data_b[12] != NULL || api.character_model->model_data_b[119] != NULL);
        if (ordinary_context && has_turn_animation && pad->input_magnitude > 0.0f &&
            RotationDistance(api.movement_facing_angle, input_angle) >= 0x6aab) {
            object->delayed_turn_timer = 0.2f;
            object->delayed_turn_target_angle = input_angle;
        } else if (pad->input_magnitude > 0.0f && ordinary_context) {
            api.movement_facing_angle = input_angle;
        }
    }

    const i32 turn_speed = static_cast<i32>(game_character->turn_rate * 8.0f * 16384.0f);
    api.facing_angle = TurnRot(api.facing_angle, api.movement_facing_angle, turn_speed, NULL);
    api.field_0x276 = SeekRot(api.field_0x276, api.facing_angle, 10.0f);
    api.movement_facing_angle = api.facing_angle;

    f32 final_speed = (pad->allocated_5a & GAMEPAD_RUNTIME_SUPPRESS_MOVEMENT) != 0 ? 0.0f : desired_speed;
    if ((object->field_0xefd & GAMEOBJECT_MOVEMENT_FLAG_BACKWARDS) != 0) {
        final_speed *= game_character->backwards_speed_multiplier;
    }

    NUVEC forward = {0.0f, 0.0f, final_speed};
    NuVecRotateY(&object->target_velocity, &forward, input_angle);
    object->target_velocity.y = 0.0f;
    if ((object->field_0xefd & GAMEOBJECT_MOVEMENT_FLAG_REVERSE_VELOCITY) != 0) {
        NuVecRotateY(&object->target_velocity, &object->target_velocity, 0x8000);
    }

    api.velocity.x = SeekValF(api.velocity.x, object->target_velocity.x, game_character->velocity_seek_rate);
    api.velocity.z = SeekValF(api.velocity.z, object->target_velocity.z, game_character->velocity_seek_rate);
    object->movement_lean_angle = 0;
    GizmoBlowupCheckProximity(WORLD, object);
}

void MovePlayer_VEHICLEDIRECTIONAL(GameObject_s *) {
}

void Move_POD(GameObject_s *) {
}

void Move_ATAT(GameObject_s *) {
}

void Move_JAWA(GameObject_s *) {
}

void Move_JEDI(GameObject_s *object) {
    if (object == NULL || object->pad_gamepad == NULL) {
        return;
    }

    GAMEPAD_s *pad = object->pad_gamepad;
    const u32 held = pad->buttons_held;
    const u32 pressed = pad->buttons_pressed;
    const i32 jump_pressed = pressed & GAMEPAD_JUMP;
    const i32 jump_held = held & GAMEPAD_JUMP;
    const i32 action_pressed = pressed & GAMEPAD_ACTION;
    const i32 action_held = held & GAMEPAD_ACTION;
    const i32 special_pressed = pressed & GAMEPAD_SPECIAL;

    if (object->id == id_BODYGUARD) {
        KeepWeaponOut(object);
    }
    DropInOutCode(object);
    if ((object->field_0xe20 & GAMEOBJECT_E20_FLAG_MOVEMENT_DISABLED) != 0) {
        return;
    }

    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    if (object->suit != NULL) {
        Signal_MoveCode(WORLD, object);
    }
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    Hang_MoveCode(object);
    Ledge_MoveCode(WORLD, object);
    if (LedgeTerrain_On != 0) {
        LedgeTerrain_MoveCode(object);
    }
    Climb_MoveCode(object);
    TightRope_MoveCode(object, jump_pressed);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    PushCode(object, 1);
    BackFlipCode(object);
    if (object->id != id_GRIEVOUS && object->id != id_BODYGUARD) {
        TakeOverCode(object, pressed & GAMEPAD_TAG);
    }
    Glide_MoveCode(object);

    GAMECHARACTERDATA *game_character = GetGameCharacterData(object);
    u32 jump_animations = 0x1b;
    if (object->id == id_BODYGUARD) {
        jump_animations = 0x10;
    }
    if (game_character != NULL && (game_character->flags_090 & 0x00400000) != 0) {
        jump_animations |= 0x100;
    }
    JumpCode(object, jump_pressed, jump_held, jump_animations, action_pressed, action_held, -1);

    GizPanel_MoveCode(WORLD, object, special_pressed);
    HatMachine_MoveCode(WORLD, object, special_pressed);
    ZipUp_MoveCode(object, special_pressed);
    BuildIt_MoveCode(object);
    Lever_MoveCode(WORLD, object);
    ThermalDetonator_MoveCode(object);
    if (object->apiobj.character_data != NULL && (object->apiobj.character_data->model_flags & 0x00040000) != 0) {
        Teleport_MoveCode(object, special_pressed);
    }
    ComboRotateCode(object, action_held);
    WeaponOutCode(object);
    WeaponInCode(object);
    WeaponScalingCode(object);
    HoldCode(object);
    HeadMovement(object);
    CloakMovement(object);
    HairMovement(object);
}

void MoveToMarker::BlowUp() {
}

void MoveToMarker::FadeOut() {
}

MoveToMarker::MoveToMarker(MechObjectInterface &) {
}

void MoveToMarker::Process(float) {
}

void MoveToMarker::Render() {
}

static __used__ void JumpAnimCode(GameObject_s *) {
}
static __used__ i32 Jump_UpdateHint(HINT_s *) {
    return 0;
}
static __used__ bool Jump_PreventJump(GameObject_s *) {
    return false;
}
static __used__ void Jump_EndOfLandContext(GameObject_s *) {
}
static __used__ i32 Move_UpdateHint(HINT_s *) {
    return 0;
}
static __used__ i32 LastSafePosExtra(GameObject_s *) {
    return 0;
}

static __used__ bool Lever_UpdateHint(HINT_s *) {
    return false;
}
struct _vuv_s;
static __used__ void MakeWingFormation(_vuv_s *, _vuv_s *, f32, i32) {
}

static __used__ void AtatPart_Stop(PART_s *) {
}

static __used__ void AtatPart_Update(PART_s *) {
}

static __used__ void BigJump_EndOfLand(GameObject_s *) {
}

static __used__ void BigJump_JumpAction_Default(GameObject_s *) {
}

static __used__ void BigJump_LandAction_Default(GameObject_s *) {
}

static __used__ bool AutoJump_UpdateHint(HINT_s *) {
    return {};
}

static __used__ void VehicleStuff_UpdateHint(HINT_s *) {
}

void Slam_Start(GameObject_s *, float) {
}

void StartLunge(GameObject_s *, float, float) {
}

void StartSlide(GameObject_s *, i32) {
}

void CanObjSlide(GameObject_s *, i32) {
}

void CanStepBack(GameObject_s *) {
}

void FlattenCode(GameObject_s *) {
}

void Glide_Start(GameObject_s *) {
}

void JetPackCode(GameObject_s *, i32, i32, i32) {
}

float SeekLinearF(float current, float target, float step) {
    if (current > target) {
        return MAX(target, current - step);
    }
    if (current < target) {
        return MIN(target, current + step);
    }
    return current;
}

void StartLaunch(GameObject_s *) {
}

void ApplyGravity(GameObject_s *object, float *gravity, float terminal_velocity, float gravity_scale,
                  float *vertical_velocity) {
    if (object == NULL || object->move_override != NULL || (object->apiobj.field_0x1f8 & 0x20) != 0) {
        return;
    }

    f32 &velocity = vertical_velocity != NULL ? *vertical_velocity : object->apiobj.velocity.y;
    if ((object->apiobj.field_0x27d & 1) != 0 && vertical_velocity == NULL) {
        velocity = 0.0f;
        return;
    }

    GAMECHARACTERDATA *game_character = GetGameCharacterData(object);
    if (gravity == NULL && game_character == NULL) {
        return;
    }
    const f32 acceleration = gravity != NULL ? *gravity : game_character->gravity;
    velocity += acceleration * FRAMETIME;
    if (terminal_velocity != 0.0f) {
        const f32 terminal_speed = NuFabs(terminal_velocity);
        velocity = MAX(-terminal_speed, MIN(velocity, terminal_speed));
    }

    (void)gravity_scale; // Used by the target's hovering/seek branches, not ordinary gravity.
}

void SetObjTarget(GameObject_s *, GameObject_s *) {
}

void SnapPosTaken(WORLDINFO_s *, pushblock_s *, nuvec_s *, i32) {
}

void StartFlatten(GameObject_s *, GameObject_s *) {
}

void UpdateMidPos(GIZMOBLOWUP_s *) {
}

void Hang_MoveCode(GameObject_s *) {
}

void HoldCode_Copy(GameObject_s *) {
}

void SetHeadTarget(GameObject_s *, nuvec_s *, signed char, float, float, float) {
}

void StartBackFlip(GameObject_s *) {
}

void ForcedBackCode(GameObject_s *) {
}

void Glide_MoveCode(GameObject_s *) {
}

void SetObjOnSurface(GameObject_s *, i32) {
}

void TurnCodeCamSafe(GameObject_s *, numtx_s *) {
}

void RotateGameMatrix(numtx_s *matrix, i32 order, u16 x, u16 y, u16 z) {
    switch (order) {
        case 0:
            if (x != 0)
                NuMtxRotateX(matrix, x);
            if (y != 0)
                NuMtxRotateY(matrix, y);
            if (z != 0)
                NuMtxRotateZ(matrix, z);
            break;
        case 1:
            if (y != 0)
                NuMtxRotateY(matrix, y);
            if (x != 0)
                NuMtxRotateX(matrix, x);
            if (z != 0)
                NuMtxRotateZ(matrix, z);
            break;
        case 2:
            if (y != 0)
                NuMtxRotateY(matrix, y);
            if (z != 0)
                NuMtxRotateZ(matrix, z);
            if (x != 0)
                NuMtxRotateX(matrix, x);
            break;
        case 3:
            if (z != 0)
                NuMtxRotateZ(matrix, z);
            if (x != 0)
                NuMtxRotateX(matrix, x);
            if (y != 0)
                NuMtxRotateY(matrix, y);
            break;
    }
}

void Hang_SetTargetMom(GameObject_s *) {
}

void Glide_SetTargetMom(GameObject_s *) {
}

void SetObjAsHeadTarget(GameObject_s *, GameObject_s *, signed char, float, float, float) {
}

void Slide_SetTargetMom(GameObject_s *, u16, float) {
}

void StepBackFromTarget(GameObject_s *) {
}

void ApplyGravity_Network(GameObject_s *) {
}

void VehicleCollisionCode(GameObject_s *) {
}

void VehicleTurnOrLoopOffset(GameObject_s *) {
}

void WallShuffle_SetTargetMom(GameObject_s *, u16) {
}

void SetMoveAndAnimateFunctions(u32, u32, u32, u32, i32, void *, void *, void *) {
}

u16 SeekRot(u16 current, u16 target, f32 rate) {
    const f32 blend = MIN(rate * FRAMETIME, 1.0f);
    i32 difference = static_cast<i32>(target) - static_cast<i32>(current);
    if (difference > 0x8000) {
        difference -= 0x10000;
    } else if (difference < -0x8000) {
        difference += 0x10000;
    }
    return static_cast<u16>(static_cast<f32>(current) + static_cast<f32>(difference) * blend);
}

void SeekVec(NUVEC *result, NUVEC *current, NUVEC *target, f32 rate) {
    const f32 blend = MIN(rate * FRAMETIME, 1.0f);
    result->x = current->x + (target->x - current->x) * blend;
    result->y = current->y + (target->y - current->y) * blend;
    result->z = current->z + (target->z - current->z) * blend;
}

u16 TurnRot(u16 current, u16 target, i32 speed, i32 *difference_out) {
    if (target == current) {
        return target;
    }

    const i32 step = static_cast<i32>(static_cast<f32>(speed) * FRAMETIME);
    const i32 difference = RotDiff(current, target);
    if (difference_out != NULL) {
        *difference_out = difference;
    }
    if (difference > step) {
        return static_cast<u16>(current + step);
    }
    if (difference < -step) {
        return static_cast<u16>(current - step);
    }
    return target;
}

void HoldCode(GameObject_s *) {
}

f32 SeekValF(f32 current, f32 target, f32 rate) {
    const f32 blend = MIN(rate * FRAMETIME, 1.0f);
    return current + (target - current) * blend;
}

void TurnCode(GameObject_s *, i32, GAMEPAD_s *) {
}

void FloatCode(GameObject_s *) {
}

void SlideCode(GameObject_s *) {
}

void StartHold(GameObject_s *) {
}

void StartTurn(GameObject_s *) {
}
