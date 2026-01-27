#include "numusic.h"

#include "nu2api.saga/nufile/nufpar.h"

#include <string.h>

static NuMusic *the_music_player = NULL;

extern "C" {
    NuMusic music_man;
};

int32_t NuMusic::ClassToIX(Track::Class i) {
    switch (i) {
        case Track::Class::CATEGORY_QUIET:
            return 0;
        case Track::Class::CATEGORY_ACTION:
            return 1;
        case Track::Class::CATEGORY_4:
            return 2;
        case Track::Class::CATEGORY_8:
            return 3;
        case Track::Class::CATEGORY_CUTSCENE:
            return 4;
        case Track::Class::CATEGORY_NOMUSIC:
            return 5;
        default:
            return -1;
    }
}

Track *NuMusic::Album::GetTrack(Track::Class class_) {
    int32_t index = ClassToIX(class_);
    if (index == -1) {
        return NULL;
    } else {
        return this->tracks[index];
    }
}

int32_t NuMusic::Initialise(const char *file, char *null, VARIPTR *bufferStart, VARIPTR bufferEnd) {
    the_music_player = this;

    this->language = null;
    // this->field6_0x18 = 0x1000;
    // this->field15_0x24 = 0;
    this->strictMode = false;

    // InitVoiceManager(this);
    InitData(file, bufferStart, bufferEnd);

    // this->field336_0x1a4 = 0;
    // this->field339_0x1b0 = 0.5;
    // this->field333_0x198 = 1.0;
    this->album = this->albums;
    // this->field334_0x19c = 1.0;
    // this->field335_0x1a0 = 1.0;
    // this->field337_0x1a8 = 1.0;
    // this->field338_0x1ac = 1.0;
    // this->field341_0x1cc = 1.0;

    return 1;
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

void NuMusic::InitData(const char *file, VARIPTR *bufferStart, VARIPTR bufferEnd) {
    this->stringPoolStart = BUFFER_ALLOC_ARRAY(&bufferStart->void_ptr, 0x10000, char);
    this->stringPoolEnd = this->stringPoolStart;

    this->albums = BUFFER_ALLOC_ARRAY(&bufferStart->void_ptr, 512, Album);
    this->tracks = BUFFER_ALLOC_ARRAY(&bufferStart->void_ptr, 2048, Track);
    this->indexes = BUFFER_ALLOC_ARRAY(&bufferStart->void_ptr, 2048, float);

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
}

void NuMusic::PlayTrack(Track::Class track) {
    PlayTrackI(track);
}

int32_t NuMusic::PlayTrackI(Track::Class class_) {

    if (this == NULL || the_music_player == NULL) {
        return -1;
    }

    if (this->album == NULL) {
        return -2;
    }

    Track *track = this->album->GetTrack(class_);

    // if (track == NULL || track->field6_0xc[(int)this->track] == -1) {
    //     return -3;
    // }

    if (track->category != Track::Class::CATEGORY_CUTSCENE) {
        // if (FindVoiceByClassAndStatus(this, Track::Class::CATEGORY_CUTSCENE, 6) != 0) {
        //     return -5;
        // }
        // if (FindVoiceByClassAndStatus(this, Track::Class::CATEGORY_CUTSCENE, 7) != 0) {
        //     return -5;
        // }
    }

    /*
    struct Voice;
    int iVar1;
    Voice *voice;
    Voice *vooice;

    voice = (Voice *)FindVoiceByTrack(this, track);
    if (voice != (Voice *)0x0) {
        iVar1 = Voice::Play(voice);
        return iVar1;
    }
    iVar1 = track->category;
    if (iVar1 == 8) {
        vooice = FindVoiceByClass(this, 8);
        if (vooice != (Voice *)0x0) {
            if ((vooice->field9_0x18 & 0xfffffffdU) != 1) {
                return -5;
            }
            goto LAB_003242a3;
        }
        iVar1 = track->category;
    }
    if (iVar1 == 0x10) {
        StopAll(this, 2);
    }
    vooice = (Voice *)FindIdleVoice(this);
    if (vooice == (Voice *)0x0) {
        return -5;
    }
LAB_003242a3:
    iVar1 = Voice::Load(vooice, track, this->track);
    if (iVar1 == 0) {
        return -5;
    }
    iVar1 = Voice::Play(vooice);
    return iVar1;
    */

    UNIMPLEMENTED();
}

void NuMusic::ParseTrack(Track::Class category, nufpar_s *fpar) {
    Track *track = &this->tracks[this->trackCount++];
    memset(track, 0, sizeof(Track));

    this->currentTrack = track;

    track->category = category;
    track->field23_0x2c = 1.0;
    track->field24_0x30 = 1.0;
    track->field25_0x34 = 1.0;

    if ((int32_t)category < 17 && ((1 << ((byte)category & 31) & 0x10110U) != 0)) {
        *(int32_t *)&track->flags &= 0xfd;
    } else {
        *(int32_t *)&track->flags |= 2;
    }

    char buf[512];
    char buf2[512];
    NuFParGetWord(fpar);
    NuStrCpy(buf, this->currentPath);
    NuStrCat(buf, fpar->word_buf);
    SubstituteString(buf2, buf, "$lang", this->language);

    track->path = AllocString(buf2);

    track->ident = RemovePath(track->path);

    nufpcomfn *errorHandler = NULL;
    if (this->strictMode != 0) {
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

    if (this->strictMode != 0) {
        NuFParSetInterpreterErrorHandler(errorHandler);
    }
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

        int pos = (size_t)pcVar2 - (size_t)src;
        NuStrNCpy(dst, src, pos + 1);
        NuStrCpy(dst + pos, subst);

        src = pcVar2 + NuStrLen(find);
        dst += pos + NuStrLen(subst);
    }

    NuStrCpy(dst, src);
}

char *NuMusic::FindString(const char *str) {
    for (char *ptr = this->stringPoolStart; ptr < this->stringPoolEnd; ptr += NuStrLen(ptr) + 1) {
        if (NuStrICmp(ptr, str) == 0) {
            return ptr;
        }
    }

    return NULL;
}

char *NuMusic::AllocString(const char *str) {
    char *ptr = FindString(str);

    if (ptr == NULL) {
        ptr = this->stringPoolEnd;
        this->stringPoolEnd += NuStrLen(str) + 1;
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
    Album *album = &this->albums[this->albumCount++];
    this->currentAlbum = album;

    // album->field1_0x4 = this->field2_0x8[this->field3_0xc];
    // album->field2_0x8 = 0;

    NuFParGetWord(fpar);
    this->currentAlbum->name = AllocString(fpar->word_buf);
}
void NuMusic::xAction(nufpar_s *fpar) {
    ParseTrack(Track::Class::CATEGORY_ACTION, fpar);
}
void NuMusic::xQuiet(nufpar_s *fpar) {
    ParseTrack(Track::Class::CATEGORY_QUIET, fpar);
}
void NuMusic::xOverlay(nufpar_s *fpar) {
    UNIMPLEMENTED();
}
void NuMusic::xSignature(nufpar_s *fpar) {
    UNIMPLEMENTED();
}
void NuMusic::xCutscene(nufpar_s *fpar) {
    ParseTrack(Track::Class::CATEGORY_CUTSCENE, fpar);
}
void NuMusic::xNoMusicC(nufpar_s *fpar) {
    ParseTrack(Track::Class::CATEGORY_NOMUSIC, fpar);
}
void NuMusic::xGlobalAttenuation(nufpar_s *fpar) {
    UNIMPLEMENTED();
}
void NuMusic::xPath(nufpar_s *fpar) {
    NuFParGetWord(fpar);
    NuStrCpy(this->currentPath, fpar->word_buf);
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
    this->currentTrack->ident = AllocString(fpar->word_buf);
}
void NuMusic::xIndex(nufpar_s *fpar) {
    this->indexes[this->indexCount++] = NuFParGetFloatRDP(fpar);
    this->currentTrack->indexCount++;
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

void GamePlayMusic(LEVELDATA *level, int32_t zero, OPTIONSSAVE *options) {
    music_man.PlayTrack(Track::Class::CATEGORY_ACTION);
}