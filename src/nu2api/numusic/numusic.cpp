// NuMusic - the music player (nu2api.2013/numusic/numusic.cpp).
//
// Decompressed from the original libTTapp.so. The player owns a parsed
// music.cfg "album" table (albums -> tracks), a shared entry-time pool and two
// stereo-stream voices. The boot path for the title music is:
//
//   LoadPermData -> ConfigureMusic -> NuMusic::Initialise("audio\\music.cfg")
//   NuMain -> GamePlayMusic(titles level) -> SelectTrackByHandle x3
//           -> PlayTrack(0x20) -> PlayTrackI -> Voice::Load -> Voice::Play
//           -> NuSound3PlayStereoV (host nusound streamer)
//   per-frame: NuMusic::Process (fades, ducking, volume mixing)
//
// Track handles pack (albumIndex << 11) | trackIndexInAlbum. The original
// constructs/validates them through reciprocal multiplies because
// sizeof(Album) == 36 == 4 * 9 and sizeof(Track) == 60 == 4 * 15; those
// literal forms are kept so the arithmetic matches the original binary.

#include <stdlib.h>
#include <string.h>

#include "decomp.h"

#include "nu2api/nufile/nufpar.h"
#include "legoapi/audio/audio.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/nusound/nusound.h"

static NuMusic *the_music_player = NULL;

NuMusic music_man;

// Globals driving GamePlayMusic. The original keeps these in the batman TU.
i32 NOMUSIC = 0;
i32 MusicOther = 0;
static i32 CurrentMusicPair_Quiet = 0;
static i32 last_currentmusicpair_quiet = 0;
i32 sticky_attack __asm__("_ZL13sticky_attack") = 0;
f32 sticky_attack_time __asm__("_ZL18sticky_attack_time") = 0.0f;
i32 PlayersUnderAttack = 0;
i32 (*CheckMusicOtherFn)(void) = NULL;

// GamePlayMusic consults this option byte (original: SuperOptions field 0x14).
// Non-zero selects the quiet/action attack tracks instead of the plain theme.
NuMusic::NuMusic() {
    for (i32 i = 0; i < 6; i++) {
        this->class_volumes[i] = 1.0f;
    }
    this->albums = NULL;
    this->album_count = 0;
    this->fileinfo = NULL;
    this->file_count = 0;
    this->track_index = 0;
}

NuMusic::~NuMusic() {
}

i32 NuMusic::ClassToIX(u32 i) {
    switch (i) {
        case TRACK_CLASS_QUIET:
            return 0;
        case TRACK_CLASS_ACTION:
            return 1;
        case TRACK_CLASS_4:
            return 2;
        case TRACK_CLASS_8:
            return 3;
        case TRACK_CLASS_CUTSCENE:
            return 4;
        case TRACK_CLASS_NOMUSIC:
            return 5;
        default:
            return -1;
    }
}

NuMusic::Track *NuMusic::Album::GetTrack(u32 clazz) {
    i32 index = ClassToIX(clazz);
    if (index == -1) {
        return NULL;
    } else {
        return this->tracks[index];
    }
}

i32 NuMusic::Album::GetTracks(u32 class_mask, Track **out_tracks) {
    i32 count = 0;
    for (i32 i = 0; i < 6; i++) {
        if ((class_mask & (1u << i)) != 0) {
            out_tracks[i] = this->tracks[i];
            count++;
        } else {
            out_tracks[i] = NULL;
        }
    }
    return count;
}

void NuMusic::Album::Initialise() {
    LOG_DEBUG("this=%p, this->tracks_source=%p", this, this->tracks_source);

    i32 j;
    TRACK_CLASS clazz;
    Track *track;
    i32 count;

    i32 i = 0;
    count = this->tracks_count;
    do {
        while (this->tracks[i] = NULL, count < 1) {
        LAB_0031f139:
            i = i + 1;
            if (i == 6) {
                return;
            }
        }

        j = 0;
        clazz = this->tracks_source->clazz;
        track = this->tracks_source;
        while ((i32)clazz != 1 << ((u8)i & 0x1f)) {
            j = j + 1;
            if (j == count)
                goto LAB_0031f139;
            clazz = track[1].clazz;
            track = track + 1;
        }
        this->tracks[i] = track;
        i = i + 1;
        if (i == 6) {
            return;
        }
    } while (true);
}

i32 NuMusic::Initialise(const char *file, char *null, VARIPTR *buffer_start, VARIPTR buffer_end) {
    the_music_player = this;
    this->language = null;
    this->pitch_default = 0x1000; // unity pitch in the game's fixed-point format
    this->current_path[0] = '\0';
    this->strict_mode = 0;
    InitVoiceManager();
    InitData(file, buffer_start, buffer_end);
    this->master_volume = 1.0f;
    this->duck_rate = 0.5f;
    this->fader_rate = 0.0f;
    this->album = this->albums;
    this->fader_current = 1.0f;
    this->fader_target = 1.0f;
    this->duck_gain = 1.0f;
    this->duck_current = 1.0f;
    this->global_attenuation = 1.0f;
    return 1;
}

void NuMusic::InitVoiceManager() {
    // Original memsets each 0x34-byte voice, then sets stream index, status,
    // the fade state and clears the fade-out flag.
    memset(&this->voices[0], 0, sizeof(Voice));
    this->voices[0].stream_index = 0;
    this->voices[0].status = VOICE_STATUS_READY;
    this->voices[0].fade_rate = 0.0f;
    this->voices[0].gain = 1.0f;
    this->voices[0].last_volume = -1.0f;
    this->voices[0].play_time = 0.0f;
    this->voices[0].flags &= ~1u;

    memset(&this->voices[1], 0, sizeof(Voice));
    this->voices[1].stream_index = 1;
    this->voices[1].status = VOICE_STATUS_READY;
    this->voices[1].fade_rate = 0.0f;
    this->voices[1].gain = 1.0f;
    this->voices[1].last_volume = -1.0f;
    this->voices[1].play_time = 0.0f;
    this->voices[1].flags &= ~1u;
}

void NuMusic::GetSoundFiles(nusound_filename_info_s **finfo, i32 *null) {
    if (this == NULL) {
        if (finfo != NULL) {
            *finfo = NULL;
        }
        if (null != NULL) {
            *null = 0;
        }
    } else {
        if (finfo != NULL) {
            *finfo = this->fileinfo;
        }
        if (null != NULL) {
            *null = this->file_count;
        }
    }
}

void NuMusic::InitData(const char *file, VARIPTR *buffer_start, VARIPTR buffer_end) {
    VARIPTR buffer_original = *buffer_start;

    this->string_pool_start = BUFFER_ALLOC_ARRAY(buffer_start, 0x10000, char);
    this->string_pool_end = this->string_pool_start;

    this->albums = BUFFER_ALLOC_ARRAY(buffer_start, 512, Album);
    this->tracks = BUFFER_ALLOC_ARRAY(buffer_start, 2048, Track);
    this->indexes = BUFFER_ALLOC_ARRAY(buffer_start, 2048, f32);
    LOG_DEBUG("this->albums=%p, this->tracks=%p, this->indexes=%p", this->albums, this->tracks, this->indexes);

    NuFParSetInterpreterErrorHandler(0);
    nufpar_s *fpar = NuFParCreate(const_cast<char *>(file));
    if (fpar != NULL) {
        NuFParPushComCTX(fpar, top_jmp_tab);

        while (NuFParGetLine(fpar), *fpar->line_buf != '\0') {
            NuFParGetWord(fpar);
            NuFParInterpretWordCTX(fpar, this);
        }

        NuFParDestroy(fpar);
    }
    NuFParSetInterpreterErrorHandler(0);
    LOG_DEBUG("Loaded %d albums, %d tracks, %d indexes", this->album_count, this->track_count, this->index_count);
    LOG_DEBUG("this->albums=%p, this->tracks=%p, this->indexes=%p", this->albums, this->tracks, this->indexes);

    i32 count = this->album_count;
    if (count == 0 || (track_count = this->track_count, track_count == 0)) {
        *buffer_start = buffer_original;
        this->albums = NULL;
        this->album_count = 0;
        this->tracks = NULL;
        this->track_count = 0;
        return;
    }

    // Compact the parse workspace down to just what was filled in, and rebase
    // the intra-pool pointers (album->tracks_source, track->entry_times).
    Track *tracksPtr;
    Track *alloced;
    Track *pTVar3;

    Album *albumPtr = this->albums;
    Album *album = (Album *)ALIGN((usize)this->string_pool_end, 4);
    if (albumPtr == album) {
        tracksPtr = this->tracks;
        alloced = (Track *)ALIGN((usize)&albumPtr[count].name, 4);
        pTVar3 = tracksPtr;
        if (tracksPtr == alloced)
            goto LAB_003203e6;
    LAB_003204e0:
        memmove(alloced, pTVar3, track_count * sizeof(Track));
        track_count = this->track_count;
        count = this->album_count;
        tracksPtr = alloced;
    } else {
        memmove(album, albumPtr, count * sizeof(Album));
        count = this->album_count;
        this->albums = album;
        track_count = this->track_count;
        tracksPtr = (Track *)(album + count);
        pTVar3 = this->tracks;
        if (pTVar3 == tracksPtr)
            goto LAB_003203e6;
        alloced = tracksPtr;
        if (track_count != 0)
            goto LAB_003204e0;
    }

    this->tracks = tracksPtr;
    if (0 < count) {
        for (Album *album = this->albums; album < &this->albums[count]; album++) {
            if (album->tracks_source != NULL) {
                album->tracks_source += (tracksPtr - pTVar3);
            }
        }
        tracksPtr = this->tracks;
    }

LAB_003203e6:
    f32 *pfVar2 = this->indexes;
    f32 *__dest = (f32 *)ALIGN((usize)&tracksPtr[track_count].path, 4);
    i32 local_20;
    if (pfVar2 == __dest) {
        local_20 = this->index_count;
    } else {
        local_20 = 0;

        if (this->index_count != 0) {
            memmove(__dest, pfVar2, this->index_count << 2);
            track_count = this->track_count;
            local_20 = this->index_count;
        }
        this->indexes = __dest;

        if (0 < track_count) {
            tracksPtr = this->tracks;
            pTVar3 = tracksPtr + track_count;
            do {
                if (tracksPtr->entry_times != NULL) {
                    tracksPtr->entry_times += (__dest - pfVar2);
                }
                tracksPtr = tracksPtr + 1;
            } while (tracksPtr != pTVar3);
            __dest = this->indexes;
        }
    }

    buffer_start->void_ptr = __dest + local_20;
    LOG_DEBUG("this->albums=%p, this->tracks=%p, this->indexes=%p", this->albums, this->tracks, this->indexes);
    for (i32 i = 0; i < this->album_count; i++) {
        this->albums[i].Initialise();
    }

    BuildSoundTable(buffer_start, buffer_end);
}

void NuMusic::BuildSoundTable(variptr_u *buffer_start, variptr_u buffer_end) {
    nusound_filename_info_s *finfo = (nusound_filename_info_s *)buffer_start->void_ptr;
    this->fileinfo = finfo;

    i32 count = 0;

    i32 i = 0;
    for (; i < this->track_count; i++) {
        Track *track = &this->tracks[i];

        // Track flags bit1 (looping) decides whether the file registers as a
        // streaming sample; the pitch rides along for the loader.
        track->file_indexes[0] =
            FindOrCreateSoundFile(finfo, &count, track->path, (i32)((track->flags << 6) >> 7), track->pitch);
        track->file_indexes[1] =
            FindOrCreateSoundFile(this->fileinfo, &count, track->name, (i32)((track->flags << 6) >> 7), track->pitch);
    }

    i += count;

    nusound_filename_info_s *puVar1 = &finfo[i];
    puVar1->filename = NULL;
    puVar1->field1_0x4 = 0;
    puVar1->index = -1;

    buffer_start->void_ptr = &finfo[i + 1];
}

i32 NuMusic::FindOrCreateSoundFile(nusound_filename_info_s *files, i32 *count, const char *filename, i32 param_4,
                                   i32 unused)

{
    if (filename == NULL) {
        return -1;
    }

    i32 index = 0;
    for (; index < *count; index++) {
        if (NuStrICmp(files[index].filename, filename) == 0) {
            return index;
        }
    }

    nusound_filename_info_s *puVar1 = &files[index];
    puVar1->filename = filename;
    puVar1->index = index;
    puVar1->field3_0xc = 0;
    puVar1->field1_0x4 = (param_4 == 0);

    (*count)++;

    return index;
}

NuMusic::Voice *NuMusic::FindVoiceByClassAndStatus(TRACK_CLASS clazz, VOICE_STATUS status) {
    Track *track = this->voices[0].tracks[this->voices[0].track_index];

    if (track == NULL || track->clazz != clazz || this->voices[0].status != status) {
        track = this->voices[1].tracks[this->voices[1].track_index];
        if (track == NULL || track->clazz != clazz || this->voices[1].status != status) {
            return NULL;
        }
        return &this->voices[1];
    } else {
        return &this->voices[0];
    }
}

NuMusic::Voice *NuMusic::FindVoiceByTrack(Track *track) {
    if (track == NULL) {
        return NULL;
    }

    i32 index = 0;
    if (this->voices[0].tracks[this->voices[0].track_index] != track) {
        if (this->voices[1].tracks[this->voices[1].track_index] != track) {
            return NULL;
        }
        index = 1;
    }

    return &this->voices[index];
}

NuMusic::Voice *NuMusic::FindVoiceByClass(TRACK_CLASS clazz) {
    i32 index;
    Track *track;

    track = this->voices[0].tracks[this->voices[0].track_index];
    if (track == NULL || track->clazz != clazz) {
        track = this->voices[1].tracks[this->voices[1].track_index];
        if (track == NULL || track->clazz != clazz) {
            return NULL;
        }
        index = 1;
    } else {
        index = 0;
    }

    return &this->voices[index];
}

NuMusic::Voice *NuMusic::FindIdleVoice() {
    i32 i = this->voices[0].status;
    if (i != 1) {
        if (this->voices[1].status == 1) {
            return &this->voices[1];
        }
        if (i != 3) {
            if (this->voices[1].status != 3) {
                return NULL;
            }
            return &this->voices[1];
        }
    }

    return &this->voices[0];
}

bool NuMusic::SelectTrackByHandle(TRACK_CLASS clazz, i32 trackHandle) {
    if (this != NULL && the_music_player != NULL && this->album != NULL) {
        u32 index = (u32)ClassToIX(clazz);
        if (index < 6) {
            if (trackHandle == -1) {
                this->album->tracks[index] = NULL;
            } else {
                Album *album = this->album;
                // Validate the album encoded in the handle's high bits against
                // the album being selected (see header note on handle packing).
                if ((((i32)((char *)album - (char *)this->albums) >> 2) * (i32)0x38e38e39 - (trackHandle >> 0xb) ==
                     0) &&
                    ((trackHandle & 0x7ff) < (u32)album->tracks_count)) {
                    album->tracks[index] = &album->tracks_source[trackHandle & 0x7ff];
                    return true;
                }
            }
        }
    }

    return false;
}

i32 NuMusic::GetTrackHandle(TRACK_CLASS clazz, const char *name) {
    if (this != NULL && the_music_player != NULL && this->album != NULL) {
        if (name == NULL) {
            i32 index = ClassToIX(clazz);
            if (index != -1) {
                Track *track = this->album->tracks[index];
                if (track != NULL) {
                    return (((i32)((char *)track - (char *)this->album->tracks_source) >> 2) * (i32)0xEEEEEEEF) |
                           (((i32)((char *)this->album - (char *)this->albums) >> 2) * (i32)0x1C71C800);
                }
            }
        } else {
            LOG_DEBUG("Searching for track name '%s' of class %d, tracks_count=%d", name, clazz,
                      this->album->tracks_count);
            for (i32 i = 0; i < this->album->tracks_count; i++) {
                if (this->album->tracks_source[i].clazz == clazz && this->album->tracks_source[i].ident != NULL) {
                    if (NuStrICmp(this->album->tracks_source[i].ident, name) == 0) {
                        return (((i32)((char *)this->album - (char *)this->albums) >> 2) * (i32)0x1C71C800) | i;
                    }
                }
            }
        }
    }

    return -1;
}

bool NuMusic::SelectTrack(TRACK_CLASS clazz, const char *name) {
    if (this != NULL && the_music_player != NULL) {
        return SelectTrackByHandle(clazz, GetTrackHandle(clazz, name));
    }
    return false;
}

void NuMusic::Voice::SetStatusFn(i32 status, i32 tag) {
    (void)tag; // original passes a source-line constant for error reporting
    if (this->status != (VOICE_STATUS)status) {
        this->play_time = 0.0f;
        this->status = (VOICE_STATUS)status;
    }
}

bool NuMusic::Voice::Load(Track *track, i32 trackIndex) {
    bool changed = this->tracks[this->track_index] != track;
    if (changed) {
        // Class-8 (overlay) tracks use the voice's second sub-stream so the
        // previous track keeps playing underneath.
        this->track_index = (track->clazz == TRACK_CLASS_8) ? 1 : 0;
        NuSound3StopStereoStream(this->stream_index);
        i32 index = this->track_index;
        this->tracks[index] = track;
        this->track_sub[index] = trackIndex;
        this->flags &= ~2u; // clear paused
        SetStatusFn(VOICE_STATUS_READY, 0x123);
    }

    return changed;
}

i32 NuMusic::Voice::Play() {
    if ((u32)(this->status - VOICE_STATUS_PLAYING) <= 1) {
        return 1; // already playing (status 6/7)
    }

    i32 idx = this->track_index;
    Track *track = this->tracks[idx];
    if (track == NULL) {
        return -4;
    }
    i32 sub = this->track_sub[idx];
    if (track->file_indexes[sub] == -1) {
        return -4;
    }

    // The other voice decides whether this voice needs a fade-in.
    Voice *other = &the_music_player->voices[1 - this->stream_index];
    Track *otherTrack = other->tracks[other->track_index];

    this->flags &= ~1u; // clear fade-out request

    f32 target_gain;
    f32 fade_rate;
    if (track->clazz == TRACK_CLASS_CUTSCENE || ((u8 *)&track->flags)[1] == 0) {
        // Cutscene tracks and first plays start at full gain.
        this->gain = 1.0f;
        target_gain = 1.0f;
        fade_rate = 0.0f;
    } else {
        // Restart of a looping track: fade in from silence.
        target_gain = 0.0f;
        fade_rate = 1.0f;
    }
    this->fade_rate = fade_rate;

    if (track->clazz != TRACK_CLASS_8 && (otherTrack == NULL || otherTrack->clazz != TRACK_CLASS_8)) {
        if (other->status == VOICE_STATUS_PLAYING_LOADED) {
            // The other voice holds a loaded overlay stream: crossfade - fade
            // this voice in to its existing gain while the other fades out.
            other->flags &= ~1u;
            other->fade_rate = -2.0f;
            target_gain = this->gain;
            this->fade_rate = 1.0f;
        } else if (track->start_offset != 0.0f) {
            this->fade_rate = 1.0f;
            target_gain = 0.0f;
        }
    }

    i32 key_status = NuSound3StreamKeyStatus(this->stream_index);
    if (key_status != 0) {
        if (this->status == VOICE_STATUS_STOPPED) {
            NuSound3ResumeStereoStream(this->stream_index);
            SetStatusFn(VOICE_STATUS_PLAYING, 0x1e3);
            return 1;
        }
    } else if (this->status == VOICE_STATUS_STOPPED) {
        this->status = VOICE_STATUS_READY;
    }
    if ((u32)(this->status - VOICE_STATUS_PLAYING) <= 1) {
        return 1;
    }

    this->gain = target_gain;

    Track *play_track = this->tracks[this->track_index];
    i32 res = NuSound3PlayStereoV(NUSOUNDPLAYTOK_STEREOSTREAM, this->stream_index, //
                                  NUSOUNDPLAYTOK_SAMPLE,
                                  play_track->file_indexes[this->track_sub[this->track_index]], //
                                  NUSOUNDPLAYTOK_PITCH, play_track->pitch,                      //
                                  NUSOUNDPLAYTOK_VOL, 0,                                        //
                                  NUSOUNDPLAYTOK_STARTOFFSET, (f64)play_track->start_offset,    //
                                  NUSOUNDPLAYTOK_LOOPTYPE, (u32)(play_track->clazz - 1U < 2),   //
                                  NUSOUNDPLAYTOK_END);
    this->last_volume = 0.0f;
    if (res == 0) {
        return -6;
    }
    SetStatusFn(VOICE_STATUS_PLAYING, 0x1e3);
    this->tracks[this->track_index]->ManageEntryTime();
    return 1;
}

void NuMusic::Voice::Cue() {
    Play();
}

i32 NuMusic::Voice::Unload() {
    i32 idx = this->track_index;
    if (this->tracks[idx] == NULL || idx == 0) {
        return 0;
    }

    if (this->tracks[idx]->clazz == TRACK_CLASS_8) {
        // Step the class-8 voice back down to its base sub-stream.
        this->track_index = idx - 1;
        if (this->tracks[this->track_index] == NULL) {
            SetStatusFn(VOICE_STATUS_NONE, 0x1e8);
            this->tracks[this->track_index] = NULL;
        } else {
            this->gain = 0.0f;
            NuSound3StopStereoStream(this->stream_index);
            Track *track = this->tracks[this->track_index];
            i32 res = NuSound3PlayStereoV(NUSOUNDPLAYTOK_STEREOSTREAM, this->stream_index, //
                                          NUSOUNDPLAYTOK_SAMPLE,
                                          track->file_indexes[this->track_sub[this->track_index]], //
                                          NUSOUNDPLAYTOK_PITCH, 0,                                 //
                                          NUSOUNDPLAYTOK_VOL, 0,                                   //
                                          NUSOUNDPLAYTOK_STARTOFFSET, (f64)track->start_offset,    //
                                          NUSOUNDPLAYTOK_LOOPTYPE, (u32)(track->clazz - 1U < 2),   //
                                          NUSOUNDPLAYTOK_END);
            this->last_volume = 0.0f;
            if (res != 0) {
                SetStatusFn(VOICE_STATUS_CUED, 0x1e9);
            }
        }
    }

    return 1;
}

i32 NuMusic::PlayTrackI(TRACK_CLASS clazz, u32 unused) {
    (void)unused;
    if (this == NULL || the_music_player == NULL) {
        return -1;
    }

    if (this->album == NULL) {
        return -2;
    }

    Track *track = this->album->GetTrack(clazz);
    if (track == NULL || track->file_indexes[this->track_index] == -1) {
        return -3;
    }

    // A playing cutscene track mutes everything else.
    if (track->clazz != TRACK_CLASS_CUTSCENE) {
        if (FindVoiceByClassAndStatus(TRACK_CLASS_CUTSCENE, VOICE_STATUS_PLAYING) != NULL) {
            return -5;
        }
        if (FindVoiceByClassAndStatus(TRACK_CLASS_CUTSCENE, VOICE_STATUS_PLAYING_LOADED) != NULL) {
            return -5;
        }
    }

    Voice *voice = FindVoiceByTrack(track);
    if (voice != NULL) {
        return voice->Play();
    }

    Voice *target;
    if (track->clazz == TRACK_CLASS_8) {
        // Overlays reuse the class-8 voice if it is idle or stopped.
        target = FindVoiceByClass(TRACK_CLASS_8);
        if (target != NULL) {
            if ((target->status & 0xfffffffdU) != 1) {
                return -5;
            }
            goto load_and_play;
        }
    }

    if (track->clazz == TRACK_CLASS_CUTSCENE) {
        StopAll(2);
    }

    target = FindIdleVoice();
    if (target == NULL) {
        return -5;
    }

load_and_play:
    if (target->Load(track, this->track_index) == 0) {
        return -5;
    }

    return target->Play();
}

i32 NuMusic::PlayTrack(TRACK_CLASS track) {
    return PlayTrackI(track, 0);
}

void NuMusic::PlayTrack(u32 track, u32 unused) {
    PlayTrackI((TRACK_CLASS)track, unused);
}

i32 NuMusic::StopAll(i32 toggle) {
    if ((toggle & 2) == 0) {
        // Stop both voices outright, cutscene tracks included.
        NuSound3StopStereoStream(this->voices[0].stream_index);
        this->voices[0].SetStatusFn(VOICE_STATUS_READY, 0x1f0);
        this->voices[0].fade_rate = 0.0f;
        this->voices[0].gain = 0.0f;
        this->voices[0].track_index = 0;
        this->voices[0].tracks[0] = NULL;
        this->voices[0].tracks[1] = NULL;
        NuSound3StopStereoStream(this->voices[1].stream_index);
    } else {
        // Keep playing cutscene tracks; stop everything else.
        Track *track0 = this->voices[0].tracks[this->voices[0].track_index];
        if (track0 == NULL || track0->clazz != TRACK_CLASS_CUTSCENE) {
            NuSound3StopStereoStream(this->voices[0].stream_index);
            this->voices[0].SetStatusFn(VOICE_STATUS_READY, 0x1f0);
            this->voices[0].fade_rate = 0.0f;
            this->voices[0].gain = 0.0f;
            this->voices[0].track_index = 0;
            this->voices[0].tracks[0] = NULL;
            this->voices[0].tracks[1] = NULL;
        }

        Track *track1 = this->voices[1].tracks[this->voices[1].track_index];
        if (track1 != NULL && track1->clazz == TRACK_CLASS_CUTSCENE) {
            return 1;
        }
        NuSound3StopStereoStream(this->voices[1].stream_index);
    }

    this->voices[1].SetStatusFn(VOICE_STATUS_READY, 0x1f0);
    this->voices[1].fade_rate = 0.0f;
    this->voices[1].gain = 0.0f;
    this->voices[1].track_index = 0;
    this->voices[1].tracks[0] = NULL;
    this->voices[1].tracks[1] = NULL;
    return 1;
}

i32 NuMusic::StopTrack(u32 clazz, i32 fade) {
    if (this == NULL || the_music_player == NULL || this->album == NULL) {
        return 0;
    }

    Track *tracks[6];
    if (this->album->GetTracks(clazz, tracks) == 0) {
        return 0;
    }

    i32 result = 0;
    for (i32 i = 0; i < 6; i++) {
        Track *track = tracks[i];
        if (track == NULL) {
            continue;
        }
        Voice *voice = FindVoiceByTrack(track);
        if (voice == NULL) {
            continue;
        }

        if ((u32)(voice->status - VOICE_STATUS_PLAYING) <= 1) {
            if ((fade & 1) != 0) {
                // Request a fade-out; Process finishes the stop at zero gain.
                voice->fade_rate = 2.0f;
                voice->flags |= 1u;
                result = 1;
            } else {
                NuSound3StopStereoStream(voice->stream_index);
                voice->SetStatusFn(VOICE_STATUS_NONE, 0x464);
                result = 1;
            }
        } else if ((u32)(voice->status - VOICE_STATUS_STOPPING) <= 1) {
            NuSound3StopStereoStream(voice->stream_index);
            voice->SetStatusFn(VOICE_STATUS_NONE, 0x46a);
            result = 1;
        }
    }

    return result;
}

i32 NuMusic::PauseTrack(u32 clazz) {
    if (this == NULL || the_music_player == NULL || this->album == NULL) {
        return 0;
    }

    Track *tracks[6];
    if (this->album->GetTracks(clazz, tracks) == 0) {
        return 0;
    }

    i32 result = 0;
    for (i32 i = 0; i < 6; i++) {
        Track *track = tracks[i];
        if (track == NULL) {
            continue;
        }
        Voice *voice = FindVoiceByTrack(track);
        if (voice == NULL || (u32)(voice->status - VOICE_STATUS_PLAYING) > 1) {
            continue;
        }
        if ((voice->flags & 2) == 0) {
            NuSound3PauseStereoStream(voice->stream_index);
            voice->flags |= 2u;
            result = 1;
        }
    }

    return result;
}

i32 NuMusic::ResumeTrack(u32 clazz) {
    if (this == NULL || the_music_player == NULL || this->album == NULL) {
        return 0;
    }

    Track *tracks[6];
    if (this->album->GetTracks(clazz, tracks) == 0) {
        return 0;
    }

    i32 result = 0;
    for (i32 i = 0; i < 6; i++) {
        Track *track = tracks[i];
        if (track == NULL) {
            continue;
        }
        Voice *voice = FindVoiceByTrack(track);
        if (voice == NULL || (u32)(voice->status - VOICE_STATUS_PLAYING) > 1) {
            continue;
        }
        if ((voice->flags & 2) != 0) {
            NuSound3ResumeStereoStream(voice->stream_index);
            voice->flags &= ~2u;
            result = 1;
        }
    }

    return result;
}

i32 NuMusic::CueTrack(u32 clazz) {
    if (this == NULL || the_music_player == NULL) {
        return -1;
    }
    if (this->album == NULL) {
        return -2;
    }
    Track *track = this->album->GetTrack(clazz);
    if (track == NULL || track->file_indexes[this->track_index] == -1) {
        return -3;
    }
    Voice *voice = FindVoiceByTrack(track);
    if (voice != NULL) {
        return 1;
    }
    if (track->clazz == TRACK_CLASS_8) {
        voice = FindVoiceByClass(TRACK_CLASS_8);
        if (voice != NULL) {
            if ((u32)(voice->status - VOICE_STATUS_PLAYING) < 2) {
                return -5;
            }
            if (voice->Load(track, this->track_index) != 0) {
                voice->Cue();
                return 1;
            }
        }
    }
    voice = FindIdleVoice();
    if (voice == NULL || voice->Load(track, this->track_index) == 0) {
        return -5;
    }
    voice->Cue();
    return 1;
}

void NuMusic::SetClassVolume(u32 class_mask, f32 volume) {
    if (this == NULL || the_music_player == NULL) {
        return;
    }
    if (volume <= 1.0f) {
        if (volume < 0.0f) {
            volume = 0.0f;
        } else if (volume <= 1.0f) {
            // keep
        } else {
            volume = 1.0f;
        }
    } else {
        volume = 1.0f;
    }

    for (i32 i = 0; i < 6; i++) {
        if ((class_mask & (1u << i)) != 0) {
            this->class_volumes[i] = volume;
        }
    }
}

void NuMusic::SetMasterVolume(f32 volume) {
    if (this == NULL || the_music_player == NULL) {
        return;
    }
    if (volume <= 1.0f) {
        if (volume < 0.0f) {
            volume = 0.0f;
        } else if (volume <= 1.0f) {
            // keep
        } else {
            volume = 1.0f;
        }
    } else {
        volume = 1.0f;
    }
    this->master_volume = volume;
}

void NuMusic::SetFader(f32 target, f32 fade_seconds) {
    if (this == NULL || the_music_player == NULL) {
        return;
    }
    if (fade_seconds != 0.0f) {
        this->fader_target = target;
        this->fader_rate = 1.0f / fade_seconds;
        return;
    }
    // Zero fade time snaps both ends of the fader.
    this->fader_target = target;
    this->fader_current = target;
    this->fader_rate = 0.0f;
}

i32 NuMusic::SetTrackEntryTimeByClass(u32 clazz, f32 entry_time) {
    if (this == NULL || the_music_player == NULL || this->album == NULL) {
        return 0;
    }
    i32 index = ClassToIX(clazz);
    if (index == -1) {
        return 0;
    }
    this->album->tracks[index]->SetEntryTime(entry_time < 0.0f ? 0.0f : entry_time);
    return 1;
}

void NuMusic::Track::ManageEntryTime() {
    // Advance to the next INDEX entry; the value becomes this track's
    // start offset the next time it is played.
    f32 value = 0.0f;
    if (this->entry_count != 0) {
        i32 index = (this->entry_index + 1) % this->entry_count;
        value = this->entry_times[index];
        this->entry_index = index;
    }
    this->start_offset = value;
}

void NuMusic::Track::SetEntryTime(f32 entry_time) {
    this->start_offset = entry_time;
}

void NuMusic::Process(f32 delta) {
    if (this == NULL || the_music_player == NULL) {
        return;
    }

    // Ramp the fader toward its target, snapping when it arrives.
    if (this->fader_current != this->fader_target) {
        f32 step = delta * this->fader_rate;
        if (this->fader_target <= this->fader_current) {
            this->fader_current -= step;
            if (this->fader_current <= this->fader_target) {
                this->fader_rate = 0.0f;
                this->fader_current = this->fader_target;
            }
        } else {
            this->fader_current += step;
            if (this->fader_target <= this->fader_current) {
                this->fader_rate = 0.0f;
                this->fader_current = this->fader_target;
            }
        }
    }

    // Ducking: while a loaded track with a DUCK volume plays, the duck gain
    // sinks to min(1, duck_volume) and ramps at max(1, 1/duck_fade) per second.
    this->duck_gain = 1.0f;
    this->duck_rate = 1.0f;

    Track *track0 = this->voices[0].tracks[this->voices[0].track_index];
    f32 duck_target = 1.0f;
    if (track0 != NULL && this->voices[0].status == VOICE_STATUS_PLAYING_LOADED) {
        duck_target = track0->duck_volume;
        if (duck_target > 1.0f) {
            duck_target = 1.0f;
        }
        this->duck_gain = duck_target;

        f32 rate = 1.0f / track0->duck_fade;
        if (rate <= 1.0f) {
            rate = 1.0f;
        }
        this->duck_rate = rate;
    }

    Track *track1 = this->voices[1].tracks[this->voices[1].track_index];
    if (track1 != NULL && this->voices[1].status == VOICE_STATUS_PLAYING_LOADED) {
        f32 vol = track1->duck_volume;
        if (vol <= duck_target) {
            duck_target = vol;
        }
        this->duck_gain = duck_target;

        f32 rate = 1.0f / track1->duck_fade;
        if (rate <= this->duck_rate) {
            rate = this->duck_rate;
        }
        this->duck_rate = rate;
    }

    // Ramp duck_current toward duck_gain without overshooting.
    f32 duck_current = this->duck_current;
    if (duck_current <= duck_target) {
        if (duck_current < duck_target) {
            duck_current += delta * this->duck_rate;
            if (duck_target <= duck_current) {
                duck_current = duck_target;
            }
            this->duck_current = duck_current;
        }
    } else {
        duck_current -= delta * this->duck_rate;
        if (duck_target <= duck_current) {
            duck_current = duck_current;
        } else {
            duck_current = duck_target;
        }
        this->duck_current = duck_current;
    }

    for (i32 vi = 0; vi < 2; vi++) {
        Voice *voice = &this->voices[vi];
        Track *track = this->voices[vi].tracks[this->voices[vi].track_index];

        // Reconcile the voice status with the stream's key status:
        // 0 = nothing queued, 1 = loaded, 2 = finished.
        i32 key_status = NuSound3StreamKeyStatus(voice->stream_index);
        if (key_status == 0) {
            NuSound3StopStereoStream(voice->stream_index);
            voice->SetStatusFn(VOICE_STATUS_READY, 0x1e0);
        } else if (key_status == 2) {
            voice->SetStatusFn(VOICE_STATUS_ENDED, 0x1e1);
        } else if (key_status == 1) {
            voice->SetStatusFn(VOICE_STATUS_PLAYING_LOADED, 0x1e2);
        }

        // Fade gain; a fade that reaches zero (or below) stops the stream.
        if (voice->status == VOICE_STATUS_PLAYING_LOADED && voice->fade_rate != 0.0f) {
            f32 gain = voice->fade_rate * delta + voice->gain;
            if (gain < 1.0f) {
                if (gain < 0.0f) {
                    voice->gain = 0.0f;
                } else {
                    voice->gain = gain;
                    if (gain != 0.0f) {
                        goto volume_mix;
                    }
                }
                // Fade finished: reset for the next play and mark the track as
                // having played (so its next Play fades in instead of snapping).
                voice->fade_rate = 0.0f;
                NuSound3StopStereoStream(voice->stream_index);
                voice->SetStatusFn(VOICE_STATUS_READY, 0x1e4);
                voice->fade_rate = 1.0f;
                Track *cur = this->voices[vi].tracks[this->voices[vi].track_index];
                if (cur != NULL) {
                    ((u8 *)&cur->flags)[1] = 1;
                }
            } else {
                voice->gain = 1.0f;
            }
        }

    volume_mix:
        // Final mix: duck * master * voice gain * class volume * fader *
        // track attenuation * global attenuation, pushed as a 14-bit volume.
        f32 duck = this->duck_current;
        f32 attenuation = 1.0f;
        f32 class_volume = 1.0f;
        if (track != NULL) {
            if (((track->flags & 1) != 0) || track->duck_volume != 1.0f) {
                // NODUCK tracks ignore the duck gain.
                duck = 1.0f;
            }
            i32 index = ClassToIX(track->clazz);
            attenuation = track->attenuation;
            class_volume = this->class_volumes[index];
        }

        f32 volume = duck * this->master_volume * voice->gain * class_volume * this->fader_current * attenuation *
                     this->global_attenuation;
        voice->volume = volume;
        if (volume != voice->last_volume) {
            NuSound3SetStereoStreamVolume(voice->stream_index, (i32)(volume * 16383.0f));
            voice->last_volume = voice->volume;
        }

        voice->play_time = delta + voice->play_time;
    }
}

extern "C" f32 numusicGetDuckVolume(void) {
    if (the_music_player != NULL) {
        return the_music_player->duck_current;
    }
    return 1.0f;
}

// i32 GamePlayMusic(LEVELDATA_s *level, i32 check, OPTIONSSAVE_s *options)
//
// Picks the level's music tracks and starts playback. `check` is 0 on the
// boot/init call and 1 when re-invoked from the main loop (where it bails
// out early if the "other" pair hasn't changed). Classes: 1 = quiet,
// 2 = action, 0x20 = plain theme (the title music lives here).
i32 GamePlayMusic(LEVELDATA_s *level, i32 check, OPTIONSSAVE_s *options) {
    (void)options;
    i32 other = MusicOther;
    last_currentmusicpair_quiet = CurrentMusicPair_Quiet;
    MusicOther = 0;
    if (NOSOUND != 0) {
        last_currentmusicpair_quiet = CurrentMusicPair_Quiet;
        MusicOther = 0;
        return NOSOUND;
    }
    if (NOMUSIC != 0) {
        last_currentmusicpair_quiet = CurrentMusicPair_Quiet;
        MusicOther = 0;
        return (i32)(usize)&NOMUSIC; // original quirk: the error code is NOMUSIC's address
    }

    i32 music_other = 0;
    if (CheckMusicOtherFn != NULL) {
        music_other = CheckMusicOtherFn();
    }

    if (check == 0) {
        sticky_attack_time = 0;
        sticky_attack = PlayersUnderAttack;
    } else if (music_other == other) {
        MusicOther = music_other;
        return music_other;
    }

    MusicOther = music_other;
    // music_tracks is [class][pair]: the quiet slot of the selected pair,
    // plus the action/ambient slots of the MusicOther pair.
    music_man.SelectTrackByHandle(TRACK_CLASS_QUIET, level->music_tracks[0][music_other]);
    music_man.SelectTrackByHandle(TRACK_CLASS_ACTION, level->music_tracks[1][MusicOther]);
    music_man.SelectTrackByHandle(TRACK_CLASS_NOMUSIC, level->music_tracks[2][MusicOther]);

    if (SuperOptions.music_enabled == 0) {
        return music_man.PlayTrack(TRACK_CLASS_NOMUSIC);
    }

    // Attack mode: prefer the pair matching the attack state.
    if (sticky_attack == 0) {
        i32 handle = music_man.GetTrackHandle(TRACK_CLASS_QUIET, NULL);
        if (handle != -1) {
            return music_man.PlayTrack(TRACK_CLASS_QUIET);
        }
        handle = music_man.GetTrackHandle(TRACK_CLASS_ACTION, NULL);
        if (handle == -1) {
            return music_man.PlayTrack(TRACK_CLASS_NOMUSIC);
        }
        return music_man.PlayTrack(TRACK_CLASS_ACTION);
    } else {
        i32 handle = music_man.GetTrackHandle(TRACK_CLASS_ACTION, NULL);
        if (handle == -1) {
            handle = music_man.GetTrackHandle(TRACK_CLASS_QUIET, NULL);
            if (handle != -1) {
                return music_man.PlayTrack(TRACK_CLASS_QUIET);
            }
            return music_man.PlayTrack(TRACK_CLASS_NOMUSIC);
        }
        return music_man.PlayTrack(TRACK_CLASS_ACTION);
    }
}

void NuMusic::ClassToName(u32) {
}

void NuMusic::Debug(i32, i32) {
}

void NuMusic::GetAlbumHandle(char const *) {
}

void NuMusic::GetPlaybackTime(u32) {
}

void NuMusic::GetPlayer() {
}

void NuMusic::GetStatus(u32, i32 *) {
}

void NuMusic::NoMusic(i32) {
}

void NuMusic::SetAlbum(char const *) {
}

void NuMusic::SetAlbum(i32) {
}

void NuMusic::ParseTrack(u32 category, nufpar_s *fpar) {
    Track *track = &this->tracks[this->track_count++];
    this->current_track = track;
    if (this->current_album != NULL) {
        this->current_album->tracks_count++;
    }

    memset(track, 0, sizeof(Track));

    // Entry times carve sequential slices out of the shared INDEX pool.
    track->entry_times = this->indexes + this->index_count;

    track->clazz = category;
    track->pitch = this->pitch_default;
    track->duck_volume = 1.0f;
    track->duck_fade = 1.0f;
    track->attenuation = 1.0f;

    // Signature (4), overlay (8) and cutscene (16) tracks default to
    // non-looping; everything else loops until NONLOOPING says otherwise.
    if ((i32)category < 0x11 && ((1u << ((u8)category & 0x1f)) & 0x10110u) != 0) {
        ((u8 *)&track->flags)[0] &= 0xfd;
    } else {
        ((u8 *)&track->flags)[0] |= 2;
    }

    char buf[256];
    char buf2[264];
    NuFParGetWord(fpar);
    NuStrCpy(buf, this->current_path);
    NuStrCat(buf, fpar->word_buf);
    SubstituteString(buf2, buf, "$lang", this->language);

    track->path = AllocString(buf2);
    track->ident = RemovePath(track->path);

    nufpcomfn *prev_handler = NULL;
    if (this->strict_mode) {
        prev_handler = NuFParSetInterpreterErrorHandler(TrackParseErrorFn);
    }

    NuFParPushComCTX(fpar, track_jmp_tab);
    if (*fpar->word_buf != '\0' && *fpar->word_buf != ';') {
        do {
            NuFParGetWord(fpar);
            NuFParInterpretWordCTX(fpar, this);
            if (*fpar->word_buf == ';')
                break;
        } while (*fpar->word_buf != '\0');
    }
    NuFParPopCom(fpar);

    if (this->strict_mode) {
        NuFParSetInterpreterErrorHandler(prev_handler);
    }

    LOG_DEBUG("Parsed track: class=%d, path='%s', ident='%s'", track->clazz, track->path, track->ident);
}

char *NuMusic::RemovePath(char *str) {
    char *str_;
    char c;

    c = *str;
    if (c != '\0') {
        char *last_sep = NULL;
        str_ = str;

        do {
            if (c == '/' || c == '\\') {
                last_sep = str_;
            }
            str_ = str_ + 1;
            c = *str_;
        } while (c != '\0');

        if (last_sep != NULL) {
            str = last_sep + 1;
        }
    }

    return str;
}

void NuMusic::SubstituteString(char *dst, char *src, char *find, char *subst) {
    while (true) {
        char *hit = NuStrIStr(src, find);
        if (hit == NULL)
            break;

        i32 pos = (usize)hit - (usize)src;
        NuStrNCpy(dst, src, pos + 1);
        NuStrCpy(dst + pos, subst);

        src = hit + NuStrLen(find);
        dst += pos + NuStrLen(subst);
    }

    NuStrCpy(dst, src);
}

char *NuMusic::FindString(const char *str) {
    for (char *ptr = this->string_pool_start; ptr < this->string_pool_end; ptr += NuStrLen(ptr) + 1) {
        if (NuStrICmp(ptr, str) == 0) {
            return ptr;
        }
    }

    return NULL;
}

char *NuMusic::AllocString(const char *str) {
    char *ptr = FindString(str);

    if (ptr == NULL) {
        ptr = this->string_pool_end;
        this->string_pool_end += NuStrLen(str) + 1;
        NuStrCpy(ptr, str);
    }

    return ptr;
}

void NuMusic::xsAlbum(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xAlbum(fpar);
}
void NuMusic::xsAction(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xAction(fpar);
}
void NuMusic::xsQuiet(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xQuiet(fpar);
}
void NuMusic::xsOverlay(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xOverlay(fpar);
}
void NuMusic::xsSignature(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xSignature(fpar);
}
void NuMusic::xsCutscene(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xCutscene(fpar);
}
void NuMusic::xsNoMusicC(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xNoMusicC(fpar);
}
void NuMusic::xsGlobalAttenuation(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xGlobalAttenuation(fpar);
}
void NuMusic::xsPath(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xPath(fpar);
}
void NuMusic::xsStrict(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xStrict(fpar);
}

void NuMusic::xAlbum(nufpar_s *fpar) {
    this->current_album = &this->albums[this->album_count++];
    this->current_album->tracks_source = &this->tracks[this->track_count];
    this->current_album->tracks_count = 0;

    NuFParGetWord(fpar);
    this->current_album->name = AllocString(fpar->word_buf);
    LOG_DEBUG("Parsing album '%s'", this->current_album->name);
}
void NuMusic::xAction(nufpar_s *fpar) {
    ParseTrack(TRACK_CLASS_ACTION, fpar);
}
void NuMusic::xQuiet(nufpar_s *fpar) {
    ParseTrack(TRACK_CLASS_QUIET, fpar);
}
void NuMusic::xOverlay(nufpar_s *fpar) {
    ParseTrack(TRACK_CLASS_8, fpar);
}
void NuMusic::xSignature(nufpar_s *fpar) {
    ParseTrack(TRACK_CLASS_4, fpar);
}
void NuMusic::xCutscene(nufpar_s *fpar) {
    ParseTrack(TRACK_CLASS_CUTSCENE, fpar);
}
void NuMusic::xNoMusicC(nufpar_s *fpar) {
    ParseTrack(TRACK_CLASS_NOMUSIC, fpar);
}
void NuMusic::xGlobalAttenuation(nufpar_s *fpar) {
    this->global_attenuation = NuFParGetFloatRDP(fpar);
    if (this->global_attenuation < 0.0f) {
        // Negative values are decibels.
        this->global_attenuation = NuSound3dBToAmplitude(this->global_attenuation);
    }
}
void NuMusic::xPath(nufpar_s *fpar) {
    NuFParGetWord(fpar);
    NuStrCpy(this->current_path, fpar->word_buf);
}
void NuMusic::xStrict(nufpar_s *fpar) {
    fpar->line_buf[28] = '\x01';
    NuFParSetInterpreterErrorHandler(GlobalParseErrorFn);
}

void NuMusic::xsIdent(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xIdent(fpar);
}
void NuMusic::xsIndex(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xIndex(fpar);
}
void NuMusic::xsNoMusic(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xNoMusic(fpar);
}
void NuMusic::xsNoDuck(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xNoDuck(fpar);
}
void NuMusic::xsDuck(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xDuck(fpar);
}
void NuMusic::xsLooping(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xLooping(fpar);
}
void NuMusic::xsNonLooping(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xNonLooping(fpar);
}
void NuMusic::xsAttenuation(nufpar_s *fpar, void *thisptr) {
    ((NuMusic *)thisptr)->xAttenuation(fpar);
}

void NuMusic::xIdent(nufpar_s *fpar) {
    NuFParGetWord(fpar);
    this->current_track->ident = AllocString(fpar->word_buf);
}
void NuMusic::xIndex(nufpar_s *fpar) {
    this->indexes[this->index_count++] = NuFParGetFloatRDP(fpar);
    this->current_track->entry_count++;
}
void NuMusic::xNoMusic(nufpar_s *fpar) {
    NuFParGetWord(fpar);
    char buf[268];
    SubstituteString(buf, fpar->word_buf, "$lang", this->language);
    this->current_track->name = AllocString(buf);
}
void NuMusic::xNoDuck(nufpar_s *fpar) {
    (void)fpar;
    ((u8 *)&this->current_track->flags)[0] |= 1;
}
void NuMusic::xDuck(nufpar_s *fpar) {
    Track *track = this->current_track;
    track->duck_volume = NuFParGetFloatRDP(fpar);
    if (track->duck_volume < 0.0f) {
        // Negative values are decibels.
        track->duck_volume = NuSound3dBToAmplitude(track->duck_volume);
    }
    track->duck_fade = NuFParGetFloatRDP(fpar);
}
void NuMusic::xLooping(nufpar_s *fpar) {
    (void)fpar;
    ((u8 *)&this->current_track->flags)[0] |= 2;
}
void NuMusic::xNonLooping(nufpar_s *fpar) {
    (void)fpar;
    ((u8 *)&this->current_track->flags)[0] &= 0xfd;
}
void NuMusic::xAttenuation(nufpar_s *fpar) {
    Track *track = this->current_track;
    track->attenuation = NuFParGetFloatRDP(fpar);
    if (track->attenuation < 0.0f) {
        // Negative values are decibels.
        track->attenuation = NuSound3dBToAmplitude(track->attenuation);
    }
}

void NuMusic::GlobalParseErrorFn(nufpar_s *param_1) {
}

void NuMusic::TrackParseErrorFn(nufpar_s *param_1) {
}

void RegisterMusic(NUSOUND_FILENAME_INFO *files) {
    LOG_DEBUG("files=%p", files);

    g_music = files;
    SFX_MUSIC_COUNT = 0;

    if (files != NULL) {
        for (; files->filename != NULL; files = files + 1) {
            NuStrLen(files->filename);
            LOG_DEBUG("Registered music file: %s", files->filename);
            SFX_MUSIC_COUNT++;
        }
    }
}
