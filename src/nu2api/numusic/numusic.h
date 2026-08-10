#pragma once

#include "globals.h"
#include "legoapi/level.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nusound/nusound.h"

#ifdef __cplusplus

struct Track;
struct NuMusic;

enum voice_status_e : u32 {
    VOICE_STATUS_1 = 1,
    VOICE_STATUS_6 = 6,
    VOICE_STATUS_7 = 7,
};
typedef enum voice_status_e VOICE_STATUS;

enum : u32 {
    TRACK_CLASS_QUIET = 0x1,
    TRACK_CLASS_ACTION = 0x2,
    TRACK_CLASS_4 = 0x4,
    TRACK_CLASS_8 = 0x8,
    TRACK_CLASS_CUTSCENE = 0x10,
    TRACK_CLASS_NOMUSIC = 0x20,
};
typedef u32 TRACK_CLASS;
typedef u32 TRACK_FLAGS;

#ifdef __cplusplus
class Track {
  public:
    char *path;
    char *name;
    char *ident;
    i32 file_indexes[2];
    TRACK_CLASS clazz;
    void *field12_0x18;
    i32 index_count;
    u8 field17_0x20;
    u8 field18_0x21;
    u8 field19_0x22;
    u8 field20_0x23;
    f32 field21_0x24;
    i32 pitch;
    f32 field23_0x2c;
    f32 field24_0x30;
    f32 field25_0x34;
    TRACK_FLAGS flags;
};

class NuMusic {
    struct Voice {
        i32 stream_index;
        Track *tracks[4];
        i32 track_index;
        VOICE_STATUS status;
        f32 field7_0x1c;
        f32 field8_0x20;
        f32 field9_0x24;
        f32 field10_0x28;
        f32 field16_0x2c;
        u8 flags;

        bool Load(Track *track, int trackIndex);
        void SetStatusFn(VOICE_STATUS status);
        i32 Play();
    };

    class Album {
      public:
        char *name;
        Track *tracks_source;
        i32 tracks_count;
        Track *tracks[6];

        Track *GetTrack(TRACK_CLASS class_);
        void Initialise();
    };

  private:
    Album *albums;
    i32 album_count;
    Track *tracks;
    i32 track_count;
    f32 *indexes;
    i32 index_count;
    i32 string_pool_size;
    Album *current_album;
    Track *current_track;
    char current_path[256];
    bool strict_mode;
    char *string_pool_start;
    char *string_pool_end;
    Voice voices[2];
    f32 field108_0x198;
    f32 field109_0x19c;
    f32 field110_0x1a0;
    i32 field111_0x1a4;
    f32 field112_0x1a8;
    f32 field113_0x1ac;
    f32 field114_0x1b0;
    u32 field115_0x1b4;
    u32 field116_0x1b8;
    u32 field117_0x1bc;
    u32 field118_0x1c0;
    u32 field119_0x1c4;
    u32 field120_0x1c8;
    f32 field139_0x1cc;
    i32 track_index;
    char *language;
    Album *album;
    nusound_filename_info_s *fileinfo;
    i32 field346_0x1e0;

  public:
    i32 Initialise(const char *file, char *null, VARIPTR *buffer_start, VARIPTR buffer_end);
    void GetSoundFiles(nusound_filename_info_s **finfo, i32 *null);

    bool SelectTrackByHandle(TRACK_CLASS clazz, i32 trackIndex);
    i32 PlayTrack(TRACK_CLASS track);
    i32 GetTrackHandle(TRACK_CLASS clazz, const char *name);

  private:
    void InitData(const char *file, VARIPTR *buffer_start, VARIPTR buffer_end);
    void InitVoiceManager();
    void BuildSoundTable(variptr_u *buffer_start, variptr_u buffer_end);
    i32 FindOrCreateSoundFile(nusound_filename_info_s *files, i32 *count, const char *name, i32 param_4, i32 unused);

    Voice *FindVoiceByClassAndStatus(TRACK_CLASS class_, VOICE_STATUS status);
    Voice *FindVoiceByTrack(Track *track);
    Voice *FindVoiceByClass(TRACK_CLASS class_);
    i32 StopAll(i32 toggle);
    Voice *FindIdleVoice();
    i32 PlayTrackI(TRACK_CLASS track);

    static i32 ClassToIX(u32 i);

    void ParseTrack(u32 category, nufpar_s *fpar);

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

    static NUFPCOMJMPCTX top_jmp_tab[11];
    static NUFPCOMJMPCTX track_jmp_tab[15];
};

extern "C" {
#endif
    extern struct NuMusic music_man;

    void RegisterMusic(NUSOUND_FILENAME_INFO *files);
#ifdef __cplusplus
}
#endif

i32 GamePlayMusic(LEVELDATA *level, i32 zero, OPTIONSSAVE *options);

#endif
