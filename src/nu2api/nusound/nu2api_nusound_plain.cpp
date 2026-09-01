#include "decomp.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nusound/nusound.h"

typedef void (*SoundBitCallback)(i32 sound_id);

i32 GroupBuffer_GetNumInGroup(i32 group_id);
i32 GroupBuffer_GetSampleByIndex(i32 group_id, i32 sample_index);

extern "C" {

    u16 SfxBits[100];
    f32 MASTERVOLUME = 1.0f;
    i32 gcutSoundMusVol = 30;
    i32 gcutSoundVol = 100;

    MusicPlayback Music = {
        MUSIC_PLAYBACK_STOPPED, -1, 0, -1, 0, -1, 0, 0, false, false, false, false, NULL, 0.0f, 0, -1, NULL,
    };

    void NuSound3FlushLoops(void);
    void NuSound3KillAllAudio(void);
    void SoundStopMusic(void);

    f32 GetSoundVolume(void) {
        return MASTERVOLUME;
    }
    void MaskSounds(const u16 *mask) {
        for (i32 i = 0; i < 100; ++i) {
            SfxBits[i] &= mask[i];
        }
    }
    void PrepareSounds(const u16 *sounds) {
        for (i32 i = 0; i < 100; ++i) {
            SfxBits[i] |= sounds[i];
        }
    }
    void SetSoundBitsById(const i32 *sound_ids, SoundBitCallback set_bit) {
        while (true) {
            i32 sound_id = *sound_ids++;
            if (sound_id == -1) {
                break;
            }
            if (sound_id >= 0) {
                set_bit(sound_id);

                i32 group_id = g_soundInfo[sound_id].group;
                if (group_id != -1) {
                    i32 sample_count = GroupBuffer_GetNumInGroup(group_id);
                    for (i32 i = 0; i < sample_count; ++i) {
                        set_bit(GroupBuffer_GetSampleByIndex(group_id, i));
                    }
                }
            }
        }
    }
    void SetSoundBitsBySingleId(i32 sound_id, SoundBitCallback set_bit) {
        if (sound_id >= 0) {
            set_bit(sound_id);

            i32 group_id = g_soundInfo[sound_id].group;
            if (group_id != -1) {
                i32 sample_count = GroupBuffer_GetNumInGroup(group_id);
                for (i32 i = 0; i < sample_count; ++i) {
                    set_bit(GroupBuffer_GetSampleByIndex(group_id, i));
                }
            }
        }
    }
    void SetSoundVolume(f32 volume) {
        MASTERVOLUME = volume;
    }
    void SoundKillAll(void) {
        SoundStopMusic();
        NuSound3FlushLoops();
        NuSound3KillAllAudio();
    }
    void SoundStopMusic(void) {
        if (NOSOUND == 0 && NOMUSIC == 0) {
            NuSound3StopStereoStream(0);
            NuSound3StopStereoStream(1);

            Music.transition = 0.0f;
            Music.state = MUSIC_PLAYBACK_STOPPED;
            Music.requested_track = -1;
            Music.current_track = -1;
            Music.queued_track = -1;
            Music.primary_stream = 0;
            Music.transition_frames = 0;
            Music.pause_requested = false;
            Music.resume_frames = 0;
            Music.field_0x12 = false;
            Music.field_0x13 = false;
            Music.update_delay = 0;
            Music.restore_requested = false;
            Music.resume_track = -1;
        }
    }
    void SoundUpdate(void) {
    }
    void edanimSoundDestroy(void) {
    }
    void edbitsSoundPlay(void) {
    }
    void gcutSetSoundVol(i32 sound_volume, i32 music_volume) {
        gcutSoundVol = sound_volume;
        gcutSoundMusVol = music_volume;
    }
}
