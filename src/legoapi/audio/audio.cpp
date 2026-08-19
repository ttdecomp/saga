#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void PlayAMusic(i32, i32, i32, i32) {
}

void GetMusicIndex(char *, nusound_filename_info_s *, i32) {
}

void MusicClearAll() {
}

void SpaceAudioPoint() {
}

void legoSetCutVolume(float) {
}

void GetAudioFadeLevel() {
}

void PS2VolumeToScalar(i32) {
}

void SetBackgroundMusic(i32) {
}

void legoSetMusicVolume(float) {
}

void ProcessMusicChanges(LEVELDATA_s *, OPTIONSSAVE_s *) {
}

void SpaceResetAudioPoint() {
}

void CheckMusicSwapInstant() {
}

void UpdateBackgroundMusic() {
}

extern "C" {

    void GetCurPreSeek(void) {
    }

    void GetCurrentMusicId(void) {
    }

    void GetOppMusicId(void) {
    }

    void MusicPreSeek(void) {
    }

    void MusicPreSeekNow(void) {
    }

    void MusicSeekOffset(void) {
    }

    void MusicSeeking(void) {
    }

    void MusicState(void) {
    }

} // extern "C"
