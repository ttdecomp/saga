#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numusic/numusic.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

// Faithful Star Wars theme wiring — opening music triggered after intro.
// Original SetBackgroundMusic(1) at LoadPerm entry starts the legal/intro
// bed; SetBackgroundMusic(-1) stops it at 0x1bfe52; TITLES_LDATA's quiet
// track (Star Wars main theme) is then started via GamePlayMusic.

void PlayAMusic(i32 a, i32 b, i32 c, i32 d) {
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    LOG_DEBUG("PlayAMusic %d %d %d %d", a, b, c, d);
}
void GetMusicIndex(char *name, nusound_filename_info_s *table, i32 def) {
    (void)name;
    (void)table;
    (void)def;
    if (name == nullptr || table == nullptr)
        return;
    for (auto *p = table; p->filename != nullptr; ++p)
        if (NuStrICmp(p->filename, name) == 0)
            return;
}
void MusicClearAll() {
    LOG_DEBUG("MusicClearAll");
}
void SpaceAudioPoint() {
}
void legoSetCutVolume(float v) {
    (void)v;
}
void GetAudioFadeLevel() {
}
void PS2VolumeToScalar(i32 v) {
    (void)v;
}
void SetBackgroundMusic(i32 track) {
    LOG_INFO("SetBackgroundMusic track=%d (Star Wars theme after intro when track==1)", track);
    if (track == -1) {
        LOG_INFO("SetBackgroundMusic: stop");
        music_man.PlayTrack(TRACK_CLASS_NOMUSIC);
        return;
    }
    i32 res = music_man.PlayTrack(TRACK_CLASS_QUIET);
    if (res < 0)
        res = music_man.PlayTrack(TRACK_CLASS_ACTION);
    LOG_INFO("SetBackgroundMusic: PlayTrack result %d", res);
    (void)res;
}
void legoSetMusicVolume(float v) {
    // Original: NuMusic::SetClassVolume(0x23, v) — the mask covers QUIET (1),
    // ACTION (2) and the 0x20 theme class.
    music_man.SetClassVolume(0x23, v);
}
void ProcessMusicChanges(LEVELDATA_s *level, OPTIONSSAVE_s *opts) {
    (void)opts;
    if (level == nullptr)
        return;
    LOG_DEBUG("ProcessMusicChanges level=%p", (void *)level);
    extern i32 GamePlayMusic(LEVELDATA * l, i32 zero, OPTIONSSAVE * o);
    GamePlayMusic((LEVELDATA *)level, 0, (OPTIONSSAVE *)opts);
}
void SpaceResetAudioPoint() {
}
void CheckMusicSwapInstant() {
    LOG_DEBUG("CheckMusicSwapInstant");
}
void UpdateBackgroundMusic() {
    LOG_DEBUG("UpdateBackgroundMusic");
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
}
