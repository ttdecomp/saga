#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

void Minicam_InitSystem(void);
void GameCam_ResetLookRot(GAMECAMERA_s *camera);

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

void GameCam_UpdateLookRot(GAMECAMERA_s *) {
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

void KeepOnScreen(GameObject_s *) {
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

void do_Pad_flymode_camera(edcam_s *, float, nupad_s *) {
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
        object->collision_height_scale = 0.0f;
    } else {
        object->collision_height_scale = (top - bottom) / (object->field_0x1008 * 2.0f);
    }
}

extern "C" {

    void cbNearClipAtCursor(void) {
    }

    void do_Pad_Standard_camera(void) {
    }

    void do_maya_mouse_camera(void) {
    }

    void do_mouse_flymode_camera(void) {
    }

} // extern "C"
