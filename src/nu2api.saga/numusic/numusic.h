#pragma once

#include <stdint.h>

#include "game/level.h"
#include "globals.h"
#include "nu2api.saga/nufile/nufpar.h"

#ifdef __cplusplus

struct Track;

enum voice_status_e : uint32_t {
    VOICE_STATUS_1 = 1,
    VOICE_STATUS_6 = 6,
    VOICE_STATUS_7 = 7,
};
typedef enum voice_status_e VOICE_STATUS;

enum : uint32_t {
    TRACK_CLASS_QUIET = 0x1,
    TRACK_CLASS_ACTION = 0x2,
    TRACK_CLASS_4 = 0x4,
    TRACK_CLASS_8 = 0x8,
    TRACK_CLASS_CUTSCENE = 0x10,
    TRACK_CLASS_NOMUSIC = 0x20,
};
typedef uint32_t TRACK_CLASS;
typedef uint32_t TRACK_FLAGS;

class Track {
  public:
    char *path;
    uint8_t field1_0x4;
    uint8_t field2_0x5;
    uint8_t field3_0x6;
    uint8_t field4_0x7;
    char *ident;
    int field6_0xc[2];
    uint8_t field7_0x10;
    uint8_t field8_0x11;
    uint8_t field9_0x12;
    uint8_t field10_0x13;
    TRACK_CLASS class_;
    void *field12_0x18;
    int32_t index_count;
    uint8_t field17_0x20;
    uint8_t field18_0x21;
    uint8_t field19_0x22;
    uint8_t field20_0x23;
    float field21_0x24;
    uint32_t field22_0x28;
    float field23_0x2c;
    float field24_0x30;
    float field25_0x34;
    TRACK_FLAGS flags;
};

class NuMusic {
    struct Voice {
        int32_t stream_index;
        Track *tracks[2];
        int32_t track_index;
        VOICE_STATUS status;
        uint32_t flags;
        void *field16_0x2c;

        bool Load(Track *track, int trackIndex);
        void SetStatusFn(VOICE_STATUS status);
        int32_t Play();
    };

    class Album {
      public:
        char *name;
        Track *tracks_source;
        int32_t tracks_count;
        Track *tracks[6];

        Track *GetTrack(TRACK_CLASS class_);
        void Initialise();
    };

  private:
    nusound_filename_info_s *fileinfo;
    int32_t field346_0x1e0;

    float *indexes;
    int32_t index_count;

    char current_path[256];
    char *string_pool_start;
    char *string_pool_end;
    bool strict_mode;
    Album *albums;
    int32_t album_count;
    Album *current_album;
    Track *tracks;
    int32_t track_count;
    Track *current_track;
    Voice voices[2];
    char *language;

    Album *album;
    int32_t track_index;

  public:
    int32_t Initialise(const char *file, char *null, VARIPTR *bufferStart, VARIPTR bufferEnd);
    void GetSoundFiles(nusound_filename_info_s **finfo, int32_t *null);

    int32_t PlayTrack(TRACK_CLASS track);

  private:
    void InitData(const char *file, VARIPTR *bufferStart, VARIPTR bufferEnd);
    void InitVoiceManager();

    Voice *FindVoiceByClassAndStatus(TRACK_CLASS class_, VOICE_STATUS status);
    Voice *FindVoiceByTrack(Track *track);
    Voice *FindVoiceByClass(TRACK_CLASS class_);
    int32_t StopAll(int32_t toggle);
    Voice *FindIdleVoice();
    int32_t PlayTrackI(TRACK_CLASS track);

    static int32_t ClassToIX(unsigned int i);

    void ParseTrack(unsigned int category, nufpar_s *fpar);

    char *RemovePath(char *str);
    void SubstituteString(char *dst, char *src, char *find, char *subst);
    char *FindString(const char *str);
    char *AllocString(const char *str);

    static void xsAlbum(nufpar_s *fpar, void *thisptr);
    static void xsAction(nufpar_s *fpar, void *thisptr);
    static void xsQuiet(nufpar_s *fpar, void *thisptr);
    static void xsOverlay(nufpar_s *fpar, void *thisptr);
    static void xsSignature(nufpar_s *fpar, void *thisptr);
    static void xsCutscene(nufpar_s *fpar, void *thisptr);
    static void xsNoMusicC(nufpar_s *fpar, void *thisptr);
    static void xsGlobalAttenuation(nufpar_s *fpar, void *thisptr);
    static void xsPath(nufpar_s *fpar, void *thisptr);
    static void xsStrict(nufpar_s *fpar, void *thisptr);

    void xAlbum(nufpar_s *fpar);
    void xAction(nufpar_s *fpar);
    void xQuiet(nufpar_s *fpar);
    void xOverlay(nufpar_s *fpar);
    void xSignature(nufpar_s *fpar);
    void xCutscene(nufpar_s *fpar);
    void xNoMusicC(nufpar_s *fpar);
    void xGlobalAttenuation(nufpar_s *fpar);
    void xPath(nufpar_s *fpar);
    void xStrict(nufpar_s *fpar);

    static void xsIdent(nufpar_s *fpar, void *thisptr);
    static void xsIndex(nufpar_s *fpar, void *thisptr);
    static void xsNoMusic(nufpar_s *fpar, void *thisptr);
    static void xsNoDuck(nufpar_s *fpar, void *thisptr);
    static void xsDuck(nufpar_s *fpar, void *thisptr);
    static void xsLooping(nufpar_s *fpar, void *thisptr);
    static void xsNonLooping(nufpar_s *fpar, void *thisptr);
    static void xsAttenuation(nufpar_s *fpar, void *thisptr);

    void xIdent(nufpar_s *fpar);
    void xIndex(nufpar_s *fpar);
    void xNoMusic(nufpar_s *fpar);
    void xNoDuck(nufpar_s *fpar);
    void xDuck(nufpar_s *fpar);
    void xLooping(nufpar_s *fpar);
    void xNonLooping(nufpar_s *fpar);
    void xAttenuation(nufpar_s *fpar);

    static void GlobalParseErrorFn(nufpar_s *param_1);
    static void TrackParseErrorFn(nufpar_s *param_1);

    NUFPCOMJMPCTX top_jmp_tab[11] = {
        {"ALBUM", NuMusic::xsAlbum},
        {"ACTION", NuMusic::xsAction},
        {"QUIET", NuMusic::xsQuiet},
        {"OVERLAY", NuMusic::xsOverlay},
        {"SIGNATURE", NuMusic::xsSignature},
        {"CUTSCENE", NuMusic::xsCutscene},
        {"NOMUSIC", NuMusic::xsNoMusicC},
        {"GLOBALATTENUATION", NuMusic::xsGlobalAttenuation},
        {"PATH", NuMusic::xsPath},
        {"STRICT", NuMusic::xsStrict},
        {NULL, NULL},
    };

    NUFPCOMJMPCTX track_jmp_tab[15] = {
        {"IDENT", NuMusic::xsIdent},
        {"ID", NuMusic::xsIdent},
        {"INDEX", NuMusic::xsIndex},
        {"IX", NuMusic::xsIndex},
        {"NOMUSIC", NuMusic::xsNoMusic},
        {"STARTPOINT", NuMusic::xsIndex},
        {"NODUCK", NuMusic::xsNoDuck},
        {"DUCK", NuMusic::xsDuck},
        {"LOOPING", NuMusic::xsLooping},
        {"LOOP", NuMusic::xsLooping},
        {"NONLOOPING", NuMusic::xsNonLooping},
        {"NOLOOP", NuMusic::xsNonLooping},
        {"ATTENUATION", NuMusic::xsAttenuation},
        {NULL, NULL},
    };
};

extern "C" {
    extern NuMusic music_man;
};

int32_t GamePlayMusic(LEVELDATA *level, int32_t zero, OPTIONSSAVE *options);

#endif
