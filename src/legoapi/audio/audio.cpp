#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numusic/numusic.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

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
void SetBackgroundMusic(i32 track) {
    // libTTapp.so 0x4df8b0: platform stub (eight NOPs and ret).
    // Title music is selected and started later by GamePlayMusic.
    (void)track;
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
