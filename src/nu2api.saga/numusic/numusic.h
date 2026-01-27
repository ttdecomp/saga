#pragma once

#include <stdint.h>

#include "game/level.h"
#include "globals.h"
#include "nu2api.saga/nufile/nufpar.h"

#ifdef __cplusplus

struct Track {
    enum class Flags : uint32_t {};

    enum class Class : uint32_t {
        CATEGORY_QUIET = 0x1,
        CATEGORY_ACTION = 0x2,
        CATEGORY_4 = 0x4,
        CATEGORY_8 = 0x8,
        CATEGORY_CUTSCENE = 0x10,
        CATEGORY_NOMUSIC = 0x20,
    };

    char *path;
    uint8_t field1_0x4;
    uint8_t field2_0x5;
    uint8_t field3_0x6;
    uint8_t field4_0x7;
    char *ident;
    int field6_0xc[1];
    uint8_t field7_0x10;
    uint8_t field8_0x11;
    uint8_t field9_0x12;
    uint8_t field10_0x13;
    Class category;
    void *field12_0x18;
    int32_t indexCount;
    uint8_t field17_0x20;
    uint8_t field18_0x21;
    uint8_t field19_0x22;
    uint8_t field20_0x23;
    float field21_0x24;
    uint32_t field22_0x28;
    float field23_0x2c;
    float field24_0x30;
    float field25_0x34;
    Flags flags;
};

class NuMusic {
    class Album {
      public:
        char *name;
        char *_1;
        int32_t _2;
        Track *tracks[6];

        Track *GetTrack(Track::Class class_);
    };

  private:
    nusound_filename_info_s *fileinfo;
    int32_t field346_0x1e0;

    float *indexes;
    int32_t indexCount;

    char currentPath[256];
    char *stringPoolStart;
    char *stringPoolEnd;
    bool strictMode;
    Album *albums;
    int32_t albumCount;
    Album *currentAlbum;
    Track *tracks;
    int32_t trackCount;
    Track *currentTrack;
    char *language;

    Album *album;

  public:
    int32_t Initialise(const char *file, char *null, VARIPTR *bufferStart, VARIPTR bufferEnd);
    void GetSoundFiles(nusound_filename_info_s **finfo, int32_t *null);

    void PlayTrack(Track::Class track);

  private:
    void InitData(const char *file, VARIPTR *bufferStart, VARIPTR bufferEnd);

    int32_t PlayTrackI(Track::Class track);

    static int32_t ClassToIX(Track::Class i);

    void ParseTrack(Track::Class category, nufpar_s *fpar);

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

void GamePlayMusic(LEVELDATA *level, int32_t zero, OPTIONSSAVE *options);

#endif