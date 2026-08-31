#include "decomp.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numusic/numusic.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern i32 DoubleScore;
extern i32 Paused;
i32 (*GameAudio_ActionMusicFn)(void) = NULL;
static f32 sticky_attack_timeout[2] = {1.0f, 6.0f};
static i32 CurrentMusicPair_Quiet = -1;
static i32 sticky_attack;
static f32 sticky_attack_time;
static i32 MusicPlrsUnderAttack;
static i32 MusicActive = -1;
static i32 MusicActiveReset;
static f32 MusicSwapDelay;
static i32 MusicSeekIndex;
static i32 MusicInside;
static i32 MusicFindCut;
static i32 MusicRestoreTrack;
static i32 MusicOnFlag = 1;
static i32 MusicPlrsHoldAttack;

struct MUSIC_CUT_STOP_INFO {
    u8 pad_00[0xec];
    i16 level_index;
};

DECOMP_ASSERT(offsetof(MUSIC_CUT_STOP_INFO, level_index) == 0xec, "MUSIC_CUT_STOP_INFO level offset");

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
    MusicPlrsUnderAttack = 0;
    MusicActive = -1;
    sticky_attack_time = 0.0f;
    MusicActiveReset = 0;
    MusicSwapDelay = 0.0f;
    MusicSeekIndex = 0;
    PlayersUnderAttack = 0;
    MusicInside = 0;
    MusicFindCut = 0;
    MusicRestoreTrack = 0;
    MusicOnFlag = 1;
    MusicPlrsHoldAttack = 0;
    CurrentMusicPair_Quiet = -1;
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

    if (GameAudio_ActionMusicFn != NULL) {
        PlayersUnderAttack = GameAudio_ActionMusicFn();
    } else {
        PlayersUnderAttack = 0;
        if (DoubleScore != 0 || Cheat_PowerUpActive(-1) != 0) {
            PlayersUnderAttack = 1;
        } else {
            for (i32 i = 0; i < 2; ++i) {
                GameObject_s *player = Player[i];
                if (player == NULL) {
                    continue;
                }
                if (player->ai.opponent != NULL) {
                    PlayersUnderAttack = 1;
                    break;
                }
                GameObject_s *opponent = (GameObject_s *)player->ai.nearest_opponent;
                if (opponent != NULL && opponent->apiobj.field_0x287 == 0 &&
                    player->ai.nearest_opponent_metric < 0.0009765625f) {
                    PlayersUnderAttack = 1;
                    break;
                }
            }
        }
    }

    MusicOther = CheckMusicOtherFn != NULL ? CheckMusicOtherFn() : 0;

    if (sticky_attack != PlayersUnderAttack) {
        if (Paused == 0) {
            sticky_attack_time -= FRAMETIME;
        }
        if (sticky_attack_time <= 0.0f) {
            sticky_attack = PlayersUnderAttack;
            sticky_attack_time = sticky_attack_timeout[sticky_attack];
        }
    }

    LEVELDATA_s *music_level = level;
    MUSIC_CUT_STOP_INFO *cut_stop = (MUSIC_CUT_STOP_INFO *)CutStopInfo;
    if (cut_stop != NULL && cut_stop->level_index != -1) {
        LEVELDATA_s *cut_level = &LDataList[cut_stop->level_index];
        if ((cut_level->flags & LEVEL_CONFIG_LOADED) != 0) {
            music_level = cut_level;
        }
    }

    music_man.SelectTrackByHandle(TRACK_CLASS_QUIET, music_level->music_tracks[0][MusicOther]);
    music_man.SelectTrackByHandle(TRACK_CLASS_ACTION, music_level->music_tracks[1][MusicOther]);
    music_man.SelectTrackByHandle(TRACK_CLASS_NOMUSIC, music_level->music_tracks[2][MusicOther]);

    if (SuperOptions.music_enabled == 0) {
        music_man.PlayTrack(TRACK_CLASS_NOMUSIC);
    } else if (sticky_attack != 0) {
        if (music_man.GetTrackHandle(TRACK_CLASS_ACTION, NULL) != -1) {
            music_man.PlayTrack(TRACK_CLASS_ACTION);
        } else if (music_man.GetTrackHandle(TRACK_CLASS_QUIET, NULL) != -1) {
            music_man.PlayTrack(TRACK_CLASS_QUIET);
        } else {
            music_man.PlayTrack(TRACK_CLASS_NOMUSIC);
        }
    } else if (music_man.GetTrackHandle(TRACK_CLASS_QUIET, NULL) != -1) {
        music_man.PlayTrack(TRACK_CLASS_QUIET);
    } else if (music_man.GetTrackHandle(TRACK_CLASS_ACTION, NULL) != -1) {
        music_man.PlayTrack(TRACK_CLASS_ACTION);
    } else {
        music_man.PlayTrack(TRACK_CLASS_NOMUSIC);
    }

    music_man.Process(FRAMETIME);
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
