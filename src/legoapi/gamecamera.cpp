#include "legoapi/gameobject.h"

struct GAMECAMERA_s;

u8 *GameCam;
i32 newgamecam;

void GameCam_Blend(GAMECAMERA_s *, f32, f32, i32) {}

void GameCam_Judder(GAMECAMERA_s *, f32, i32, NUVEC *) {}

void GameCam_NewShake(GAMECAMERA_s *, f32, f32, f32) {}

void GameCam_HitJudder() {}
