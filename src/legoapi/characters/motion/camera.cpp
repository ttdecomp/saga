#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"

void Minicam_InitSystem(void);
void GameCam_ResetLookRot(GAMECAMERA_s *camera);

void GameCam_Blend(GAMECAMERA_s *, float, float, i32) {
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
    camera->field_0x1c0 = 0.0f;
    camera->field_0x1bc = 0.0f;
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

void GameCam_NewShake(GAMECAMERA_s *, float, float, float) {
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

void GameCam_GetAdjustedYRot(GAMECAMERA_s *) {
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
}

void ViewCamGetGamePad() {
}

void KeepVehicleOnScreen(GameObject_s *, i32, i32, i32) {
}

void CentreTwoPlayerCamera(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *) {
}

void do_Pad_flymode_camera(edcam_s *, float, nupad_s *) {
}

void InitCameraTargetMaterial() {
}

void GetTopBot(GameObject_s *) {
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
