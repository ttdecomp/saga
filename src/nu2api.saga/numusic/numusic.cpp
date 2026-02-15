#include <stdlib.h>
#include <string.h>

#include "decomp.h"

#include "nu2api.saga/nufile/nufpar.h"
#include "nu2api.saga/numusic/numusic.h"
#include "nu2api.saga/nusound/nusound.h"

static NuMusic *the_music_player = NULL;

extern "C" {
    NuMusic music_man;
};

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

Track *NuMusic::Album::GetTrack(u32 clazz) {
    i32 index = ClassToIX(clazz);
    if (index == -1) {
        return NULL;
    } else {
        return this->tracks[index];
    }
}

i32 NuMusic::Initialise(const char *file, char *null, VARIPTR *buffer_start, VARIPTR buffer_end) {
    the_music_player = this;
    this->language = null;
    // this->field6_0x18 = 0x1000;
    this->current_path[0] = '\0';
    this->strict_mode = 0;
    InitVoiceManager();
    InitData(file, buffer_start, buffer_end);
    // this->field111_0x1a4 = 0;
    // this->field114_0x1b0 = 0.5;
    // this->field108_0x198 = 1.0;
    this->album = this->albums;
    // this->field109_0x19c = 1.0;
    // this->field110_0x1a0 = 1.0;
    // this->field112_0x1a8 = 1.0;
    // this->field113_0x1ac = 1.0;
    // this->field139_0x1cc = 1.0;
    return 1;
}

void NuMusic::InitVoiceManager() {
    memset(&this->voices[0], 0, sizeof(Voice));
    this->voices[0].stream_index = 0;
    this->voices[0].status = VOICE_STATUS_1;

    memset(&this->voices[1], 0, sizeof(Voice));
    this->voices[1].stream_index = 1;
    this->voices[1].status = VOICE_STATUS_1;
}

void NuMusic::GetSoundFiles(nusound_filename_info_s **finfo, int *null) {
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
            *null = this->field346_0x1e0;
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

    Track *tracksPtr;
    Track *alloced;
    Track *pTVar3;

    Album *albumPtr = this->albums;
    album = (Album *)((usize)this->string_pool_end + 3U & 0xfffffffc);
    if (albumPtr == album) {
        tracksPtr = this->tracks;
        alloced = (Track *)((uint)((usize)&albumPtr[count].name + 3U) & 0xfffffffc);
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
                // album->tracks_source = (Track *)((usize)album->tracks_source + ((usize)tracksPtr - (usize)pTVar3));
                album->tracks_source += (tracksPtr - pTVar3);
            }
        }
        tracksPtr = this->tracks;
    }

LAB_003203e6:
    f32 *pfVar2 = this->indexes;
    f32 *__dest = (f32 *)((uint)((usize)&tracksPtr[track_count].path + 3U) & 0xfffffffc);
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
                // if (tracksPtr->field8_0x18 != (void *)0x0) {
                // tracksPtr->field8_0x18 = (void *)((int)tracksPtr->field8_0x18 + ((int)__dest - (int)pfVar2));
                //}
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

void NuMusic::Album::Initialise() {
    int j;
    TRACK_CLASS clazz;
    Track *track;
    int count;

    LOG_DEBUG("this=%p, this->tracks_source=%p", this, this->tracks_source);

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
        while ((i32)clazz != 1 << ((byte)i & 0x1f)) {
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

i32 NuMusic::PlayTrack(TRACK_CLASS track) {
    return PlayTrackI(track);
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

bool NuMusic::SelectTrackByHandle(TRACK_CLASS clazz, i32 trackIndex) {
    Album *album;

    if (this != NULL && the_music_player != NULL && this->album != NULL) {
        u32 index = ClassToIX(clazz);
        if (index < 6) {
            if (trackIndex == -1) {
                this->album->tracks[index] = NULL;
            } else {
                album = this->album;
                if (
                    // (((int)album - (int)this->albums >> 2) * 0x38e38e39 - (trackIndex >> 0xb) == 0) &&
                    trackIndex < album->tracks_count) {
                    album->tracks[index] = &album->tracks_source[trackIndex];
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
                if (this->album->tracks[index] != NULL) {
                    // return ((int)album->tracks[index] - (int)album->tracks_source >> 2) * -0x11111111 |
                    //        ((int)pAVar1 - (int)this->albums >> 2) * 0x1c71c800;
                    return this->album->tracks[index] - this->album->tracks_source;
                }
            }
        } else {
            LOG_DEBUG("Searching for track name '%s' of class %d, tracks_count=%d", name, clazz,
                      this->album->tracks_count);
            for (i32 i = 0; i < this->album->tracks_count; i++) {
                if (this->album->tracks_source[i].clazz == clazz && this->album->tracks_source[i].ident != NULL) {
                    if (NuStrICmp(this->album->tracks_source[i].ident, name) == 0) {
                        // return ((int)this->album - (int)this->albums >> 2) * 0x1c71c800 | i;
                        return i;
                    }
                }
            }
        }
    }

    return -1;
}

void NuMusic::Voice::SetStatusFn(VOICE_STATUS status) {
    if (this->status != status) {
        this->field16_0x2c = NULL;
        this->status = status;
    }
}

extern "C" void NuSound3StopStereoStream(i32 streamIndex) {
    LOG_WARN("Unimplemented: NuSound3StopStereoStream(%d)", streamIndex);
}

bool NuMusic::Voice::Load(Track *track, int trackIndex) {
    bool bVar1;
    int index;

    bVar1 = this->tracks[this->track_index] != track;
    if (bVar1) {
        this->track_index = (uint)(track->clazz == TRACK_CLASS_8);
        NuSound3StopStereoStream(this->stream_index);
        index = this->track_index;
        this->tracks[index] = track;
        this->tracks[index + 2] = (Track *)trackIndex;
        this->flags &= 0xfd;
        SetStatusFn(VOICE_STATUS_1);
    }

    return bVar1;
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

i32 NuMusic::StopAll(i32 toggle) {
    UNIMPLEMENTED();
}

i32 NuMusic::Voice::Play() {
    LOG_DEBUG("");

#define ARG(key, value) key, value

    NuSound3PlayStereoV(                                      //
        ARG(NUSOUNDPLAYTOK_STEREOSTREAM, this->stream_index), //
        ARG(NUSOUNDPLAYTOK_SAMPLE, NULL),                     //
        ARG(NUSOUNDPLAYTOK_PITCH, 0.0f),                      //
        ARG(NUSOUNDPLAYTOK_VOL, 1.0f),                        //
        ARG(NUSOUNDPLAYTOK_STARTOFFSET, 0.0f),                //
        ARG(NUSOUNDPLAYTOK_LOOPTYPE, 0),                      //
        NUSOUNDPLAYTOK_END                                    //
    );

    return 1;
}

i32 NuMusic::PlayTrackI(TRACK_CLASS clazz) {
    if (this == NULL || the_music_player == NULL) {
        return -1;
    }

    if (this->album == NULL) {
        return -2;
    }

    Track *track = this->album->GetTrack(clazz);
    LOG_DEBUG("Playing track class %d: track=%p", clazz, track);

    if (track == NULL || track->file_indexes[this->track_index] == -1) {
        return -3;
    }

    if (track->clazz != TRACK_CLASS_CUTSCENE) {
        if (FindVoiceByClassAndStatus(TRACK_CLASS_CUTSCENE, VOICE_STATUS_6) != 0) {
            return -5;
        }
        if (FindVoiceByClassAndStatus(TRACK_CLASS_CUTSCENE, VOICE_STATUS_7) != 0) {
            return -5;
        }
    }

    Voice *voice = FindVoiceByTrack(track);
    if (voice != NULL) {
        return voice->Play();
    }

    Voice *vooice;
    if (track->clazz == TRACK_CLASS_8) {
        if (vooice != NULL) {
            vooice = FindVoiceByClass(TRACK_CLASS_8);
            if ((vooice->status & 0xfffffffdU) != 1) {
                return -5;
            }
        }
    } else {
        if (track->clazz == TRACK_CLASS_CUTSCENE) {
            StopAll(2);
        }

        vooice = FindIdleVoice();
        if (vooice == NULL) {
            return -5;
        }
    }

    if (vooice->Load(track, this->track_index) == 0) {
        return -5;
    }

    return vooice->Play();
}

void NuMusic::ParseTrack(u32 category, nufpar_s *fpar) {
    Track *track = &this->tracks[this->track_count++];
    if (this->current_album != NULL) {
        this->current_album->tracks_count++;
    }

    memset(track, 0, sizeof(Track));

    this->current_track = track;

    track->clazz = category;
    track->field23_0x2c = 1.0;
    track->field24_0x30 = 1.0;
    track->field25_0x34 = 1.0;

    if ((i32)category < 17 && ((1 << ((byte)category & 31) & 0x10110U) != 0)) {
        *(i32 *)&track->flags &= 0xfd;
    } else {
        *(i32 *)&track->flags |= 2;
    }

    char buf[512];
    char buf2[512];
    NuFParGetWord(fpar);
    NuStrCpy(buf, this->current_path);
    NuStrCat(buf, fpar->word_buf);
    SubstituteString(buf2, buf, "$lang", this->language);

    track->path = AllocString(buf2);
    track->ident = RemovePath(track->path);

    nufpcomfn *errorHandler = NULL;
    if (this->strict_mode != 0) {
        errorHandler = NuFParSetInterpreterErrorHandler(TrackParseErrorFn);
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

    if (this->strict_mode != 0) {
        NuFParSetInterpreterErrorHandler(errorHandler);
    }

    LOG_DEBUG("Parsed track: class=%d, path='%s', ident='%s'", track->clazz, track->path, track->ident);
}

char *NuMusic::RemovePath(char *str) {
    char *str_;
    char c;
    char *pcVar1;

    c = *str;
    if (c != '\0') {
        pcVar1 = NULL;
        str_ = str;

        do {
            if (c == '/' || c == '\\') {
                pcVar1 = str_;
            }
            str_ = str_ + 1;
            c = *str_;
        } while (c != '\0');

        if (pcVar1 != NULL) {
            str = pcVar1 + 1;
        }
    }

    return str;
}

void NuMusic::SubstituteString(char *dst, char *src, char *find, char *subst) {
    while (true) {
        char *pcVar2 = NuStrIStr(src, find);
        if (pcVar2 == NULL)
            break;

        int pos = (usize)pcVar2 - (usize)src;
        NuStrNCpy(dst, src, pos + 1);
        NuStrCpy(dst + pos, subst);

        src = pcVar2 + NuStrLen(find);
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
    UNIMPLEMENTED();
}
void NuMusic::xSignature(nufpar_s *fpar) {
    UNIMPLEMENTED();
}
void NuMusic::xCutscene(nufpar_s *fpar) {
    ParseTrack(TRACK_CLASS_CUTSCENE, fpar);
}
void NuMusic::xNoMusicC(nufpar_s *fpar) {
    ParseTrack(TRACK_CLASS_NOMUSIC, fpar);
}
void NuMusic::xGlobalAttenuation(nufpar_s *fpar) {
    UNIMPLEMENTED();
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
    this->current_track->index_count++;
}
void NuMusic::xNoMusic(nufpar_s *fpar) {
    UNIMPLEMENTED();
}
void NuMusic::xNoDuck(nufpar_s *fpar) {
    UNIMPLEMENTED();
}
void NuMusic::xDuck(nufpar_s *fpar) {
    UNIMPLEMENTED();
}
void NuMusic::xLooping(nufpar_s *fpar) {
    UNIMPLEMENTED();
}
void NuMusic::xNonLooping(nufpar_s *fpar) {
    UNIMPLEMENTED();
}
void NuMusic::xAttenuation(nufpar_s *fpar) {
    UNIMPLEMENTED();
}

void NuMusic::GlobalParseErrorFn(nufpar_s *param_1) {
}

void NuMusic::TrackParseErrorFn(nufpar_s *param_1) {
}

i32 GamePlayMusic(LEVELDATA *level, i32 zero, OPTIONSSAVE *options) {
    music_man.SelectTrackByHandle(TRACK_CLASS_QUIET, level->music_tracks[0][0]);

    return music_man.PlayTrack(TRACK_CLASS_QUIET);
}
