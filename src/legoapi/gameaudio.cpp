#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

struct GameObject_s;
struct WORLDINFO_s;

f32 nusound_fade_start;
f32 nusound_fade_end;
i32 (*SetSoundFadeDistCallBackFn)(WORLDINFO_s *world);
bool (*CheckMusicOtherFn)();
bool (*GameAudio_ActionMusicFn)();

void GameAudio_PlaySfx(i32, NUVEC *, i32, i32) {
}

void GameAudio_PlaySfxById(i32, NUVEC *, i32, i32) {
}

void PlayFootStepSfx(GameObject_s *) {
}

i32 GameAudio_GetSfxId(i32) {
    return -1;
}

extern "C" i32 PlaySfx(const char *, NUVEC *) {
    return -1;
}

extern "C" void PlaySfxAndSetVolume(char *, NUVEC *, f32) {
}

void ChatterSfx(GameObject_s *, i32, f32) {
}

void PlayJumpSfx(GameObject_s *, i32) {
}

void PlayLandSfx(GameObject_s *, i32) {
}
