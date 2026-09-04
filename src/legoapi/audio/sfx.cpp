#include "legoapi/world/world_shared.h"
#include "legoapi/audio/audio.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nusound/nusound.h"
#include "decomp_assert.h"

#include <string.h>

struct SoundTable;

enum RepeatSfxState : u8 {
    REPEAT_SFX_INACTIVE = 0,
    REPEAT_SFX_INITIAL_DELAY = 1,
    REPEAT_SFX_PLAY = 2,
    REPEAT_SFX_INTERVAL = 3,
};

struct RepeatSfx {
    i16 sfx_id;
    RepeatSfxState state;
    i8 plays_remaining;
    f32 timer;
    f32 interval;
    nuvec_s *position;
};

DECOMP_ASSERT(sizeof(RepeatSfx) == 0x10, "RepeatSfx size");

static i32 repsfxcount;
static RepeatSfx repsfxtab[32];

extern "C" {
    u16 GlobalSfxBits[100];
}

void NuSound3CreateVoice(nuvec_s *position, i32 sample_index, f32 volume_bits, f32 pitch, i32 falloff_near,
                         i32 falloff_far, f32 pan, bool has_3d);

extern "C" void PlaySfxByIdEx(i32 sfx_id, nuvec_s *position, f32 volume, f32 pitch);
void GameAudio_PlaySfxById(i32 sfx_id, nuvec_s *position, i32 flags, i32 volume);

i32 ActionFromQuiet(i32 idx) {
    static i16 ActionPairTab[14] = {-1};
    if (idx != -1) {
        i16 *pair = ActionPairTab;
        while (*pair != -1) {
            if (*pair == idx) {
                return pair[1];
            }
            pair += 14;
        }
    }
    return -1;
}
i32 AmbientFromQuiet(i32 idx) {
    static i16 AmbientPairTab[2] = {-1};
    if (idx != -1) {
        i16 *pair = AmbientPairTab;
        while (*pair != -1) {
            if (*pair == idx) {
                return pair[1];
            }
            pair += 2;
        }
    }
    return -1;
}

extern "C" void ResetSounds(void) {
    memcpy(SfxBits, GlobalSfxBits, sizeof(SfxBits));
}

void SetLevelSfxBits(WORLDINFO *world) {
    (void)world;
}
void ResetLevSfx(WORLDINFO *world) {
    for (i32 i = 0; i < 0x40; i++) {
        world->level_sfx[i].id = -1;
    }
    world->level_sfx_count = 0;
}

void InitSpecialSfx(WORLDINFO *world) {
    (void)world;
}
void LoadSpecialSfxFile(WORLDINFO *world) {
    (void)world;
}

i32 ActionMusicFn() {
    return {};
}

i32 CheckMusicOther() {
    return {};
}

extern "C" {

    f32 sfx_wait;

    void GetLogicalSfxCount(void) {
    }

    void GetSfxCount(void) {
    }

    i32 GetSfxIdN(char *, i32) {
        return -1;
    }

    void GetSfxName(void) {
    }

    i32 IsSfxLooping(i32) {
        return 0;
    }

    void PauseGameAudio(void) {
    }

    void PauseGameMusic(void) {
    }

    void PauseGameSfx(void) {
    }

    void PlayAltGameMusic(void) {
    }

    void PlayCutMusic(void) {
    }

    void PlayMusic(void) {
    }

    void PlaySfx(char *, struct nuvec_s *) {
    }

    void PlaySfxAndSetPitch(void) {
    }

    void PlaySfxAndSetVolume(void) {
    }

    void PlaySfxAndSetVolumeAndPitch(void) {
    }

    void PlaySfxById(i32 sfx_id, nuvec_s *position) {
        PlaySfxByIdEx(sfx_id, position, 1.0f, 1.0f);
    }

    void PlaySfxByIdAndSetPitch(void) {
    }

    void PlaySfxByIdAndSetVolume(i32 sfx_id, nuvec_s *position, f32 volume) {
        PlaySfxByIdEx(sfx_id, position, volume, 1.0f);
    }

    void PlaySfxByIdAndSetVolumeAndPitch(void) {
    }

    void PlaySfxByIdEx(i32 sfx_id, nuvec_s *position, f32 volume, f32 pitch) {
        if (sfx_id == -1 || g_soundInfo == NULL) {
            return;
        }

        NUSOUNDINFO *sound = &g_soundInfo[sfx_id];
        if (sound->disabled != 0 || sound->comment != 0 || sound->index < 0) {
            return;
        }

        i32 voice_volume = static_cast<i32>(static_cast<f32>(sound->volume) * volume);
        voice_volume = CLAMP(voice_volume, 0, 0x3fff);

        // NuSound3CreateVoice keeps the original ABI: its float slot carries
        // the raw PS2-volume dword which the update path converts to 0..1.
        f32 volume_bits;
        memcpy(&volume_bits, &voice_volume, sizeof(volume_bits));

        nuvec_s origin = {0.0f, 0.0f, 0.0f};
        nuvec_s *voice_position = position != NULL ? position : &origin;
        NuSound3CreateVoice(voice_position, sound->index, volume_bits, pitch, static_cast<i32>(sound->falloff_near),
                            static_cast<i32>(sound->falloff_far), sound->pan, position != NULL);
    }

    void PlayingCutMusic(void) {
    }

    void PrepareAllSounds(void) {
    }

    void RegisterSounds(void) {
    }

    void ResetPreSeek(void) {
        if (Music.state == MUSIC_PLAYBACK_DUAL_STREAM) {
            Music.current_track = -1;
        } else {
            Music.queued_track = -1;
            Music.requested_track = -1;
            Music.pause_requested = false;
        }
        Music.track_data = NULL;
    }

    void RestoreGameMusic(void) {
    }

    void ResumeGameAudio(void) {
    }

    void SOUND_SFXRequest_Table(void) {
    }

    void SetAudioFadeLevel(void) {
    }

    void SetCutVolume(void) {
    }

    void SetLinkedCutSceneMusic(void) {
    }

    void SetMusicVolume(void) {
    }

    void SetPreSeekStartPoint(void) {
    }

    void SetSfxBitTab_Off(void) {
    }

    void SetSfxBitTab_On(void) {
    }

    void SetSfxBit_Off(void) {
    }

    void SetSfxBit_On(void) {
    }

    void SfxBit(void) {
    }

    void SfxBitMaskTable(void) {
    }

    void SfxBitTab(void) {
    }

    void SfxBitsRestore(void) {
    }

    void SfxBitsSetAll(void) {
    }

    void SfxBitsStore(void) {
    }

    void StopAltGameMusic(void) {
    }

    void SwapMusic(void) {
    }

} // extern "C"

void PlayDieSfx(GameObject_s *) {
}

void PlayHurtSfx(GameObject_s *) {
}

void PlayJumpSfx(GameObject_s *, i32) {
}

void PlayLandSfx(GameObject_s *, i32, i32) {
}

void SfxBitTabEx(SoundTable const *, i32) {
}

void TickTockSfx() {
}

void AddFootSteps(GameObject_s *) {
}

void PlayGruntSfx(GameObject_s *) {
}

void PlaySabreSfx(char *, GameObject_s *, nuvec_s *, i32) {
}

void LevChatterSfx(char *, nuvec_s *) {
}

void PlayRepeatSfx(char *name, i32 sfx_id, f32 initial_delay, char play_count, f32 interval, nuvec_s *position) {
    if (play_count == 1 && initial_delay == 0.0f) {
        if (sfx_id != -1) {
            GameAudio_PlaySfxById(sfx_id, position, 0, 0);
        } else {
            PlaySfx(name, position);
        }
        return;
    }

    if (initial_delay > 0.0f) {
        repsfxtab[repsfxcount].state = REPEAT_SFX_INITIAL_DELAY;
    } else {
        repsfxtab[repsfxcount].state = REPEAT_SFX_PLAY;
    }

    if (sfx_id == -1)
        sfx_id = GetSfxId(name);

    repsfxtab[repsfxcount].sfx_id = static_cast<i16>(sfx_id);
    RepeatSfx &repeat = repsfxtab[repsfxcount];
    repsfxcount = (repsfxcount + 1) & 31;
    repeat.timer = initial_delay;
    repeat.plays_remaining = play_count;
    repeat.interval = interval;
    repeat.position = position;
}

void ResetRepeatSfx() {
    repsfxcount = 0;
    memset(repsfxtab, 0, sizeof(repsfxtab));
    repsfxtab[0].sfx_id = -1;
}

void SetSfxBit_OnEx(i32) {
}

void UpdateLevelSfx(WORLDINFO_s *, i32) {
}

void PlayFootStepSfx(GameObject_s *) {
}

void SetSfxBit_OffEx(i32) {
}

void UpdateRepeatSfx() {
    for (RepeatSfx &repeat : repsfxtab) {
        switch (repeat.state) {
            case REPEAT_SFX_INITIAL_DELAY:
                if (repeat.timer > 0.0f) {
                    repeat.timer -= FRAMETIME;
                } else {
                    repeat.state = REPEAT_SFX_PLAY;
                }
                break;

            case REPEAT_SFX_PLAY:
                GameAudio_PlaySfxById(repeat.sfx_id, repeat.position, 0, 0);
                --repeat.plays_remaining;
                if (repeat.plays_remaining > 0) {
                    repeat.timer = repeat.interval;
                    repeat.state = REPEAT_SFX_INTERVAL;
                } else {
                    memset(&repeat, 0, sizeof(repeat));
                }
                break;

            case REPEAT_SFX_INTERVAL:
                if (repeat.timer > 0.0f) {
                    repeat.timer -= FRAMETIME;
                } else {
                    repeat.state = REPEAT_SFX_PLAY;
                }
                break;

            default:
                break;
        }
    }
}

void AddLevelSfxFromId(i32, i32 *, i32 *, i32) {
}

void SetSfxBitTab_OnEx(SoundTable *, i32) {
}

void SetSfxBitTab_OffEx(SoundTable *, i32) {
}

void SfxCheckMusicOnOff(OPTIONSSAVE_s *) {
}

void AddLevelSfxFromName(char *, i32 *, i32 *, i32) {
}

void AddLevelSfxGizmoSys(GIZMOSYS_s *, void *, i32 *, i32 *, i32) {
}

void BlockSfx(GameObject_s *) {
}

void SfxBitEx(i32) {
}

void AddLevSfx(WORLDINFO_s *, nuvec_s *, char *, i32) {
}
