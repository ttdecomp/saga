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
// track (Star Wars main theme) is then started via GamePlayMusic. On
// HOST_BUILD the call is faithful (music_man.PlayTrack) but not required
// for the window test — missing MusicInfo or NOSOUND just logs and returns.

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
#ifdef HOST_BUILD
    if (NOSOUND)
        return;
#endif
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
#ifdef HOST_BUILD
    if (NOSOUND) {
        LOG_INFO("HOST_BUILD NOSOUND — faithful call skipped, window test not blocked");
        return;
    }
    if (MusicInfo == nullptr) {
        LOG_WARN("SetBackgroundMusic: MusicInfo null — audio not required for host window test");
        return;
    }
#endif
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
    (void)v;
    LOG_DEBUG("legoSetMusicVolume %f", v);
}
void ProcessMusicChanges(LEVELDATA_s *level, OPTIONSSAVE_s *opts) {
    (void)opts;
    if (level == nullptr)
        return;
    LOG_DEBUG("ProcessMusicChanges level=%p", (void *)level);
#ifdef HOST_BUILD
    if (NOSOUND || MusicInfo == nullptr)
        return;
#endif
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
#ifdef HOST_BUILD
    if (NOSOUND || MusicInfo == nullptr)
        return;
#endif
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
