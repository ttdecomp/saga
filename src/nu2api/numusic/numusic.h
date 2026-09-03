#pragma once

#include "globals.h"
#include "legoapi/world/level.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nusound/nusound.h"

#ifdef __cplusplus

struct NuMusic;

// C-linkage free function (original symbol `numusicGetDuckVolume`), befriended
// by NuMusic below so it can read the duck gain.
extern "C" f32 numusicGetDuckVolume(void);

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
class NuMusic {
  public:
    // Voice state machine (original NuMusic::Voice, 0x34 bytes at NuMusic+0x130):
    // status 1 = ready/idle, 2/3 = stopping/stopped, 4 = cued, 5 = stream ended,
    // 6 = playing, 7 = playing (stream loaded, eligible for ducking).
    enum VOICE_STATUS : u32 {
        VOICE_STATUS_NONE = 0,
        VOICE_STATUS_READY = 1,
        VOICE_STATUS_STOPPING = 2,
        VOICE_STATUS_STOPPED = 3,
        VOICE_STATUS_CUED = 4,
        VOICE_STATUS_ENDED = 5,
        VOICE_STATUS_PLAYING = 6,
        VOICE_STATUS_PLAYING_LOADED = 7,
    };

    struct Track {
        char *path;          // 0x00 full path with $lang substitution applied
        char *name;          // 0x04 alternate "NOMUSIC" file inside the track block
        char *ident;         // 0x08 bare filename, used for handle lookup by name
        i32 file_indexes[2]; // 0x0c sound-table indexes: [0] = path file, [1] = name file
        TRACK_CLASS clazz;   // 0x14
        f32 *entry_times;    // 0x18 slice of the player's shared entry-time pool
        i32 entry_count;     // 0x1c number of INDEX entries for this track
        i32 entry_index;     // 0x20 round-robin cursor into entry_times
        f32 start_offset;    // 0x24 entry time applied on next Play
        i32 pitch;           // 0x28 raw fixed-point pitch (0x1000 = unity), pushed as dword
        f32 duck_volume;     // 0x2c DUCK <dB|amp> — max gain while this track ducks others
        f32 duck_fade;       // 0x30 DUCK's second value — 1/duck_fade is the duck ramp rate
        f32 attenuation;     // 0x34 ATTENUATION <dB|amp>
        TRACK_FLAGS flags;   // 0x38 bit0 = NODUCK, bit1 = LOOPING (class default set in ParseTrack)

        void ManageEntryTime();
        void SetEntryTime(float);
    };

    struct Voice {
        i32 stream_index;    // 0x00 stereo-stream slot 0/1
        Track *tracks[2];    // 0x04 loaded track per sub-stream
        i32 track_sub[2];    // 0x0c file_indexes sub-slot per sub-stream (0 = path file, 1 = name file)
        i32 track_index;     // 0x14 active sub-stream (class-8 tracks use sub 1)
        VOICE_STATUS status; // 0x18
        f32 gain;            // 0x1c fade gain, 1.0 = unity (Process ramps by fade_rate)
        f32 volume;          // 0x20 final volume computed by Process
        f32 last_volume;     // 0x24 last volume pushed to the stream (-1.0 = force update)
        f32 fade_rate;       // 0x28 gain change per second; negative = fading out
        f32 play_time;       // 0x2c playback seconds accumulated by Process
        u8 flags;            // 0x30 bit0 = fade-out requested, bit1 = paused

        i32 Load(Track *track, i32 trackIndex);
        void SetStatusFn(i32 status, i32 tag);
        i32 Play();

        void Cue();
        i32 Unload();
    };

    class Album {
      public:
        char *name;
        Track *tracks_source;
        i32 tracks_count;
        Track *tracks[6];

        void Initialise();
        Track *GetTrack(TRACK_CLASS class_);
        i32 GetTracks(u32, NuMusic::Track **);
    };

  private:
    Album *albums;                     // +0x000
    i32 album_count;                   // +0x004
    Track *tracks;                     // +0x008
    i32 track_count;                   // +0x00c
    f32 *indexes;                      // +0x010
    i32 index_count;                   // +0x014
    i32 pitch_default;                 // +0x018
    Album *current_album;              // +0x01c
    Track *current_track;              // +0x020
    char current_path[256];            // +0x024
    bool strict_mode;                  // +0x124
    char *string_pool_start;           // +0x128
    char *string_pool_end;             // +0x12c
    Voice voices[2];                   // +0x130
    f32 master_volume;                 // +0x198
    f32 fader_current;                 // +0x19c
    f32 fader_target;                  // +0x1a0
    f32 fader_rate;                    // +0x1a4
    f32 duck_gain;                     // +0x1a8
    f32 duck_current;                  // +0x1ac
    f32 duck_rate;                     // +0x1b0
    f32 class_volumes[6];              // +0x1b4
    f32 global_attenuation;            // +0x1cc
    i32 track_index;                   // +0x1d0
    char *language;                    // +0x1d4
    Album *album;                      // +0x1d8
    nusound_filename_info_s *fileinfo; // +0x1dc
    i32 file_count;                    // +0x1e0

    friend f32 numusicGetDuckVolume(void);

  public:
    NuMusic();
    ~NuMusic();

    i32 Initialise(const char *file, char *null, VARIPTR *buffer_start, VARIPTR buffer_end);
    void GetSoundFiles(nusound_filename_info_s **finfo, i32 *null);

    bool SelectTrackByHandle(TRACK_CLASS clazz, i32 trackIndex);
    i32 PlayTrack(TRACK_CLASS track);
    i32 GetTrackHandle(TRACK_CLASS clazz, const char *name);
    i32 StopAll(i32 toggle);
    i32 ResumeTrack(u32);
    void SetFader(float, float);

  private:
    void InitData(const char *file, VARIPTR *buffer_start, VARIPTR buffer_end);
    void InitVoiceManager();
    void BuildSoundTable(variptr_u *buffer_start, variptr_u buffer_end);
    i32 FindOrCreateSoundFile(nusound_filename_info_s *files, i32 *count, const char *name, i32 param_4, i32 unused);

    Voice *FindVoiceByClassAndStatus(TRACK_CLASS class_, VOICE_STATUS status);
    Voice *FindVoiceByTrack(Track *track);
    Voice *FindVoiceByClass(TRACK_CLASS class_);
    Voice *FindIdleVoice();
    i32 PlayTrackI(TRACK_CLASS track, u32 unused);

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

    // Gameplay control API — the game TU drives these directly (legoSetMusicVolume,
    // NuMain's per-frame update, level flow).
  public:
    void ClassToName(u32);
    i32 CueTrack(u32);
    void Debug(i32, i32);
    void GetAlbumHandle(char const *);
    void GetPlaybackTime(u32);
    void GetPlayer();
    void GetStatus(u32, i32 *);
    void NoMusic(i32);
    i32 PauseTrack(u32);
    void PlayTrack(u32, u32);
    void Process(float);
    bool SelectTrack(u32, char const *);
    void SetAlbum(char const *);
    void SetAlbum(i32);
    void SetClassVolume(u32, float);
    void SetMasterVolume(float);
    i32 SetTrackEntryTimeByClass(u32, float);
    i32 StopTrack(u32, i32);
};

extern "C" {
#endif
    extern struct NuMusic music_man;

    void RegisterMusic(NUSOUND_FILENAME_INFO *files);
#ifdef __cplusplus
}
#endif

i32 GamePlayMusic(LEVELDATA *level, i32 zero, OPTIONSSAVE *options);

extern i32 NOMUSIC;

#endif
