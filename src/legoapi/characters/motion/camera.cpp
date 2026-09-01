#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

void Minicam_InitSystem(void);
void GameCam_ResetLookRot(GAMECAMERA_s *camera);
extern i32 newgamecam;
i32 GetMenuID(void);
i32 OnOrInsidePlane(NUVEC *point, NUVEC *plane_point, NUVEC *plane_normal, NUVEC *corrected_point, f32 normal_offset,
                    f32 *distance_out);
void Surface_Deflect(NUVEC *normal, NUVEC *movement, NUVEC *result, i32 mode);
void SpecialMove_Cancel(GameObject_s *object);

void GameCam_Blend(GAMECAMERA_s *camera, f32 duration, f32 curve, i32 mode) {
    if (camera == NULL) {
        camera = GameCam;
    }
    if (duration <= 0.0f || camera->mode == -1) {
        return;
    }

    camera->blend_start_pitch = camera->desired_pitch;
    camera->blend_start_yaw = camera->desired_yaw;
    camera->blend_start_roll = camera->desired_roll;
    camera->blend_mode = mode < 1 ? 1 : 2;
    camera->previous_camera_mode = camera->previous_mode;

    camera->blend_start_position = camera->desired_position;
    camera->blend_end_position = camera->desired_position;
    camera->blend_start_target = camera->target;
    camera->blend_end_target = camera->target;

    camera->reset_blend = 1;
    camera->blend_time = 0.0f;
    camera->blend_duration = duration;
    camera->blend_curve = curve;
}

void GameCam_Reset(GAMECAMERA_s *camera) {
    if (camera == NULL) {
        camera = GameCam;
    }

    camera->sock_position.location.sock = -1;
    camera->sock_position.location.segment = -1;
    camera->previous_mode = -1;
    camera->field_0x1c8 = 0.0f;
    ObstacleCamSpl = NULL;
    camera->field_0x1d0 = 0.0f;
    MiniCutCam = 0;
    camera->field_0x1d4 = 0.0f;
    camera->mode = -1;
    camera->field_0x1d8 = 0.0f;
    camera->field_0x1dc = 1.0f;
    camera->blend_duration = 0.0f;
    camera->blend_time = 0.0f;
    camera->position_seek = static_cast<f32>(static_cast<u8>(WORLD->current_level->cam_pos_seek));
    camera->field_0x1b8 = 0.0f;
    camera->field_0x1ec = 0.0f;
    camera->field_0x1e8 = 0.0f;
    camera->field_0x1f4 = 0.0f;
    camera->field_0x1f0 = 0.0f;
    camera->angle_seek = static_cast<f32>(static_cast<u8>(WORLD->current_level->cam_angle_seek));
    GameCam_ResetLookRot(camera);
    Minicam_InitSystem();
}

void GameCam_Judder(GAMECAMERA_s *, float, i32, nuvec_s *) {
}

void GameCam_HitRoll() {
}

void GameCam_NewShake(GAMECAMERA_s *camera, float amount, float speed, float duration) {
    if (camera == NULL) {
        camera = GameCam;
    }
    camera->field_0x1d4 = amount;
    camera->field_0x1d8 = speed;
    camera->field_0x1dc = duration;
}

void GameCam_HitJudder() {
}

void GameCam_UpdateShake(GAMECAMERA_s *, float) {
}

void GameCam_ResetLookRot(GAMECAMERA_s *camera) {
    if (camera == NULL) {
        camera = GameCam;
    }

    camera->field_0x214 = 0.0f;
    camera->field_0x218 = 0.0f;
    camera->field_0x20c = 0.0f;
    camera->field_0x210 = 0.0f;
    camera->field_0x204 = 0.0f;
    camera->field_0x208 = 0.0f;
}

static bool GameCam_AddPlayerLookRot(GAMECAMERA_s *camera, GameObject_s *object) {
    if (object == NULL || static_cast<i8>(object->apiobj.flags_low) >= 0 || object->pad_gamepad == NULL ||
        object->pad_gamepad->pad == NULL) {
        return false;
    }

    i32 look_source = 1;
    if (GameCam_ObjLookingWithLeftStick != NULL) {
        look_source = GameCam_ObjLookingWithLeftStick(object);
    }
    if (look_source != 1 && look_source != 2) {
        return false;
    }

    GAMEPAD_s *gamepad = object->pad_gamepad;
    constexpr f32 kLookPitch = 1820.0f;
    constexpr f32 kLookYaw = 2730.0f;

    if (gamepad->input_mode == 1) {
        const u32 horizontal = gamepad->buttons_held & (GAMEPAD_DLEFT | GAMEPAD_DRIGHT);
        const u32 vertical = gamepad->buttons_held & (GAMEPAD_DUP | GAMEPAD_DDOWN);
        if (horizontal == GAMEPAD_DLEFT) {
            camera->field_0x208 -= kLookYaw;
        } else if (horizontal == GAMEPAD_DRIGHT) {
            camera->field_0x208 += kLookYaw;
        }
        if (vertical == GAMEPAD_DUP) {
            camera->field_0x204 -= kLookPitch;
        } else if (vertical == GAMEPAD_DDOWN) {
            camera->field_0x204 += kLookPitch;
        }
        return true;
    }

    nupad_s *pad = gamepad->pad;
    const f32 analog_x = static_cast<f32>(look_source == 2 ? pad->analog_left_x : pad->analog_right_x);
    const f32 analog_y = static_cast<f32>(look_source == 2 ? pad->analog_left_y : pad->analog_right_y);
    constexpr f32 kAnalogCentre = 127.5f;
    constexpr f32 kAnalogScale = 1.0f / kAnalogCentre;
    camera->field_0x204 += (analog_y - kAnalogCentre) * kAnalogScale * kLookPitch;
    camera->field_0x208 += (analog_x - kAnalogCentre) * kAnalogScale * kLookYaw;
    return true;
}

void GameCam_UpdateLookRot(GAMECAMERA_s *camera) {
    if (camera == NULL) {
        camera = GameCam;
    }

    camera->field_0x204 = 0.0f;
    camera->field_0x208 = 0.0f;

    i32 contributing_players = 0;
    if (MiniCutCam == 0) {
        for (i32 i = 0; i < 2; ++i) {
            if (GameCam_AddPlayerLookRot(camera, Player[i])) {
                ++contributing_players;
            }
        }
    }
    if (contributing_players > 1) {
        const f32 inverse_count = 1.0f / static_cast<f32>(contributing_players);
        camera->field_0x204 *= inverse_count;
        camera->field_0x208 *= inverse_count;
    }

    camera->field_0x20c = SeekLinearF(camera->field_0x20c, camera->field_0x204, FRAMETIME * 2.0f);
    camera->field_0x210 = SeekLinearF(camera->field_0x210, camera->field_0x208, FRAMETIME * 2.0f);
    camera->field_0x214 = SeekValF(camera->field_0x214, camera->field_0x20c, 3.0f);
    camera->field_0x218 = SeekValF(camera->field_0x218, camera->field_0x210, 3.0f);
}

void GameCameraMakeMiniCut(nugspline_s *, float, float, float, float, i32, i32) {
}

void GameCameraMakeMiniCut2(nuvec_s *, nuvec_s *, i32, float, float, float, float, i32, i32, i32) {
}

void GameCameraMakeMiniCut3(u32, float, i32, i32, i32, void *, i32, nuvec_s *, float, float, float, float, float, float,
                            float, i32, nugspline_s *, char, char) {
}

u16 GameCam_GetAdjustedYRot(GAMECAMERA_s *camera) {
    if (camera == NULL) {
        camera = GameCam;
    }
    return static_cast<u16>(camera->input_yaw + static_cast<i32>(camera->field_0x218));
}

static __used__ variptr_u buildFrustrum(nuvec_s *, nuvec_s *, i16) {
    return {};
}

extern "C" {

    void CameraEmitterDistance(void) {
    }

} // extern "C"

void ViewCamDraw() {
}

void KeepOnScreen(GameObject_s *object) {
    const f32 previous_keep_time = object->field_0xda8;
    object->field_0xda8 = 0.0f;

    // Ordinary on-foot Cantina path.  Vehicle, gunship and special-character
    // branches in the target deliberately use different plane responses.
    if (newgamecam != 0 || (object->field_0xf03 & 0x10) != 0 || static_cast<i8>(object->apiobj.flags_low) >= 0 ||
        object->apiobj.field_0x287 != 0 || (object->field_0xefe & 4) == 0 || MiniCutCam != 0 || GetMenuID() != -1 ||
        VehicleArea != 0) {
        return;
    }

    switch (object->character_id_0x7a5) {
        case 0x0f:
        case 0x1f:
        case 0x2b:
        case 0x46:
        case 0x47:
        case 0x51:
            return;
        default:
            break;
    }

    // In an ordinary area only the two controllable player slots are kept in
    // the camera frustum.  Other active objects reach KeepOnScreen through the
    // same update loop but are not constrained by the target.
    if (object != Player[0] && object != Player[1]) {
        return;
    }

    // A vehicle paired with this ordinary player owns the camera constraint.
    GameObject_s *other_player = object == Player[0] ? Player[1] : Player[0];
    if (other_player != NULL && static_cast<i8>(other_player->apiobj.flags_low) < 0 &&
        other_player->character_id_0x7a5 == 0x2b) {
        return;
    }

    CHARACTERDATA *character = object->apiobj.character_data;
    if (character == NULL || character->field11_0x24 == NULL) {
        return;
    }
    const f32 push_distance = static_cast<GAMECHARACTERDATA_s *>(character->field11_0x24)->movement_speed;
    NUVEC constrained_movement = object->apiobj.velocity;
    bool constrained = false;

    // Near plane has no object-size inset in the original.
    if (OnOrInsidePlane(&object->apiobj.collision_position, &PlayPlane[0].point, &PlayPlane[0].normal, NULL, 0.0f,
                        NULL) != 0) {
        Surface_Deflect(&PlayPlane[0].normal, &constrained_movement, &constrained_movement, 0);
        constrained_movement.x += PlayPlane[0].normal.x * push_distance;
        constrained_movement.z += PlayPlane[0].normal.z * push_distance;
        constrained = true;
    }

    // Test the right plane first, then the left plane.  Each response is
    // calculated from the unmodified velocity, exactly as in the target.
    PLAYPLANE_s *side_plane = NULL;
    if (OnOrInsidePlane(&object->apiobj.collision_position, &PlayPlane[2].point, &PlayPlane[2].normal, NULL,
                        -object->apiobj.field_0x1dc, NULL) != 0) {
        side_plane = &PlayPlane[2];
    } else if (OnOrInsidePlane(&object->apiobj.collision_position, &PlayPlane[1].point, &PlayPlane[1].normal, NULL,
                               -object->apiobj.field_0x1dc, NULL) != 0) {
        side_plane = &PlayPlane[1];
    }
    if (side_plane != NULL) {
        Surface_Deflect(&side_plane->normal, &object->apiobj.velocity, &constrained_movement, 0);
        constrained_movement.x += side_plane->normal.x * push_distance;
        constrained_movement.z += side_plane->normal.z * push_distance;
        constrained = true;
    }

    if (KEEPONSCREEN_SIDESONLY == 0) {
        PLAYPLANE_s *depth_plane = NULL;
        if (OnOrInsidePlane(&object->apiobj.collision_position, &PlayPlane[4].point, &PlayPlane[4].normal, NULL,
                            -object->apiobj.field_0x1e0, NULL) != 0) {
            // Crossing the lower screen plane pushes toward the depth plane
            // selected by the camera pitch.
            depth_plane = GameCam->dir.y > 0.0f ? &PlayPlane[5] : &PlayPlane[0];
        } else if (OnOrInsidePlane(&object->apiobj.collision_position, &PlayPlane[3].point, &PlayPlane[3].normal, NULL,
                                   -object->apiobj.field_0x1e0, NULL) != 0) {
            // The upper screen plane uses the opposite depth response.  This
            // distinction is important: using the same plane for both sides
            // can cancel movement in the wrong world-space direction.
            depth_plane = GameCam->dir.y > 0.0f ? &PlayPlane[0] : &PlayPlane[5];
        }

        if (depth_plane != NULL) {
            Surface_Deflect(&depth_plane->normal, &object->apiobj.velocity, &constrained_movement, 0);
            constrained_movement.x += depth_plane->normal.x * push_distance;
            constrained_movement.z += depth_plane->normal.z * push_distance;
            constrained = true;
        }
    }

    if (constrained) {
        object->apiobj.movement_direction.x = constrained_movement.x;
        object->apiobj.movement_direction.z = constrained_movement.z;
        object->field_0xda8 = previous_keep_time + FRAMETIME;
        SpecialMove_Cancel(object);
    }
}

void ViewCamGetTgt() {
}

void ViewCamGetMode() {
}

void SetDepthOfField() {
}

void SpeedBlur_Apply(WORLDINFO_s *) {
}

void SpeedBlur_Update() {
}

void ViewCamSetActive(i32, GAMEPAD_s *) {
}

void KeepPointOnScreen(nuvec_s *, nuvec_s *) {
}

void SetCameraMatrices() {
    NuRndrLightingStateCurrent.field_0x60 = 1;
    NuRndrLightingStateCurrent.field_0x74 = 0;
    NuRndrSetSpecularLightPS(NULL, NULL);

    NUMTX effect_matrix;
    NUVEC scale = {1.0f, 1.0f, 1.0f};
    NuMtxInvR(&effect_matrix, &global_camera.mtx);
    NuMtxScale(&effect_matrix, &scale);
    effect_matrix.m30 = 1.0f;
    effect_matrix.m31 = 1.0f;
    effect_matrix.m32 = 0.0f;
    effect_matrix.m33 = 1.0f;
    effect_matrix.m23 = 0.0f;
    effect_matrix.m13 = 0.0f;
    effect_matrix.m03 = 0.0f;
    NuRndrSetFxMtx(&effect_matrix);
}

void ViewCamGetGamePad() {
}

void KeepVehicleOnScreen(GameObject_s *, i32, i32, i32) {
}

void CentreTwoPlayerCamera(nuvec_s *center, nuvec_s *player_a, nuvec_s *player_b, nuvec_s *reference) {
    const f32 distance_a = NuVecDist(reference, player_a, NULL);
    const f32 distance_b = NuVecDist(reference, player_b, NULL);
    const f32 blend = distance_a / (distance_a + distance_b);
    center->x = player_a->x + (player_b->x - player_a->x) * blend;
    center->y = player_a->y + (player_b->y - player_a->y) * blend;
    center->z = player_a->z + (player_b->z - player_a->z) * blend;
}

void do_Pad_flymode_camera(edcam_s *camera, float delta_time, nupad_s *pad) {
    constexpr f32 kNominalFrameTime = 1.0f / 60.0f;
    constexpr f32 kPitchSpeedScale = 32.0f;
    constexpr f32 kYawSpeedScale = 64.0f;
    constexpr f32 kPositionSpeedScale = 0.01f;
    constexpr i32 kPadDeadZone = 32;
    constexpr i32 kPitchLimit = 0x4000;

    const f32 frame_scale = delta_time / kNominalFrameTime;
    const f32 move_speed = camera->auto_move_dist_scale == 0.0f
                               ? 1.0f
                               : camera->auto_move_base + NuFabs(camera->distance) * camera->auto_move_dist_scale;
    const f32 zoom_speed = camera->auto_zoom_dist_scale == 0.0f
                               ? 1.0f
                               : camera->auto_zoom_base + NuFabs(camera->distance) * camera->auto_zoom_dist_scale;

    NUMTX rotation = numtx_identity;
    NuMtxRotateX(&rotation, camera->pitch);
    NuMtxRotateY(&rotation, camera->yaw);

    NUVEC opposite_offset = {0.0f, 0.0f, -camera->distance};
    NuVecMtxRotate(&opposite_offset, &opposite_offset, &rotation);
    NUVEC old_opposite;
    NuVecAdd(&old_opposite, &camera->position, &opposite_offset);

    const i32 pad_yaw = NuPs2ApplyDeadZone(pad->analog_right_x, kPadDeadZone);
    const i32 pad_pitch = NuPs2ApplyDeadZone(pad->analog_right_y, kPadDeadZone);
    i32 pitch_delta =
        static_cast<i32>(static_cast<f32>(pad_pitch * camera->pad_pitch_speed) * delta_time * kPitchSpeedScale);
    if ((camera->freedoms & EDCAM_FREEDOM_INVERT_PAD_PITCH) != 0) {
        pitch_delta = -pitch_delta;
    }
    if ((camera->freedoms & EDCAM_FREEDOM_PITCH) != 0) {
        camera->pitch += pitch_delta;
        if (camera->pitch > kPitchLimit) {
            camera->pitch = kPitchLimit;
        }
        if (camera->pitch < -kPitchLimit) {
            camera->pitch = -kPitchLimit;
        }
    }
    if ((camera->freedoms & EDCAM_FREEDOM_YAW) != 0) {
        camera->yaw +=
            static_cast<i32>(static_cast<f32>(pad_yaw * camera->pad_yaw_speed) * delta_time * kYawSpeedScale);
    }

    rotation = numtx_identity;
    NuMtxRotateX(&rotation, camera->pitch);
    NuMtxRotateY(&rotation, camera->yaw);
    opposite_offset = {0.0f, 0.0f, -camera->distance};
    NuVecMtxRotate(&opposite_offset, &opposite_offset, &rotation);
    NUVEC new_opposite;
    NuVecAdd(&new_opposite, &camera->position, &opposite_offset);
    NuVecAdd(&camera->position, &camera->position, &new_opposite);
    NuVecSub(&camera->position, &camera->position, &old_opposite);

    NUVEC movement = {0.0f, 0.0f, 0.0f};
    const i32 pad_move_x = NuPs2ApplyDeadZone(pad->analog_left_x, kPadDeadZone);
    movement.x -= static_cast<f32>(pad_move_x) * delta_time * camera->distance * kPositionSpeedScale;
    const i32 pad_move_z = NuPs2ApplyDeadZone(pad->analog_left_y, kPadDeadZone);
    movement.z += static_cast<f32>(pad_move_z) * delta_time * camera->distance * kPositionSpeedScale;

    movement.y += static_cast<f32>(pad->analog_l1) * camera->position_speed.y * move_speed * frame_scale * 0.5f;
    movement.y -= static_cast<f32>(pad->analog_l2) * camera->position_speed.y * move_speed * frame_scale * 0.5f;

    if ((camera->freedoms & EDCAM_FREEDOM_DISTANCE) != 0) {
        camera->distance += static_cast<f32>(pad->analog_r1) * camera->distance_speed * zoom_speed * frame_scale;
        camera->distance -= static_cast<f32>(pad->analog_r2) * camera->distance_speed * zoom_speed * frame_scale;
        if (camera->distance > -camera->minimum_distance) {
            camera->distance = -camera->minimum_distance;
        }
    }

    NuVecMtxRotate(&movement, &movement, &rotation);
    if ((camera->freedoms & EDCAM_FREEDOM_POSITION_X) != 0) {
        camera->position.x += movement.x;
    }
    if ((camera->freedoms & EDCAM_FREEDOM_POSITION_Y) != 0) {
        camera->position.y += movement.y;
    }
    if ((camera->freedoms & EDCAM_FREEDOM_POSITION_Z) != 0) {
        camera->position.z += movement.z;
    }
}

void InitCameraTargetMaterial() {
}

void GetTopBot(GameObject_s *object) {
    CHARACTERDATA *character = object->apiobj.character_data;
    const f32 bottom = character->field15_0x34;
    const f32 top = character->field16_0x38;
    object->field_0xffc = bottom;
    object->field_0x1000 = top;

    if (object->field_0x1008 == 0.0f) {
        object->collision_y_scale = 0.0f;
    } else {
        object->collision_y_scale = (top - bottom) / (object->field_0x1008 * 2.0f);
    }
}

extern "C" {

    void cbNearClipAtCursor(void) {
    }

    void do_Pad_Standard_camera(edcam_s *, f32, nupad_s *) {
    }

    void do_maya_mouse_camera(edcam_s *) {
    }

    void do_mouse_flymode_camera(edcam_s *, f32) {
    }

} // extern "C"
