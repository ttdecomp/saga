#include "legoapi_types.h"

void GameCam_Blend(GAMECAMERA_s *, float, float, i32) {
}

void GameCam_Reset(GAMECAMERA_s *) {
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

void GameCam_ResetLookRot(GAMECAMERA_s *) {
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

static __used__ void PlayerCamPos(GameObject_s *, nuvec_s *, nuvec_s *) {
}

static __used__ variptr_u buildFrustrum(nuvec_s*, nuvec_s*, i16) { return {}; }

extern "C" {

    void CameraEmitterDistance(void) {
    }

} // extern "C"
