#pragma once

struct NuMusic {
    struct Track {
        void ManageEntryTime();
        void SetEntryTime(float);
    };
    struct Album {
        void GetTracks(u32, NuMusic::Track **);
    };
    struct VOICE_STATUS {};
    struct Voice {
        void Cue();
        void Load(NuMusic::Track *, i32);
        void SetStatusFn(i32, i32);
        void Unload();
    };
    void ClassToName(u32);
    void CueTrack(u32);
    void Debug(i32, i32);
    void FindVoiceByClassAndStatus(u32, NuMusic::VOICE_STATUS);
    void FindVoiceByTrack(NuMusic::Track *);
    void GetAlbumHandle(char const *);
    void GetPlaybackTime(u32);
    void GetPlayer();
    void GetStatus(u32, i32 *);
    void NoMusic(i32);
    NuMusic();
    void PauseTrack(u32);
    void PlayTrack(u32, u32);
    void PlayTrackI(u32, u32);
    void Process(float);
    void ResumeTrack(u32);
    void SelectTrack(u32, char const *);
    void SetAlbum(char const *);
    void SetAlbum(i32);
    void SetClassVolume(u32, float);
    void SetFader(float, float);
    void SetMasterVolume(float);
    void SetTrackEntryTimeByClass(u32, float);
    void StopTrack(u32, i32);
    ~NuMusic();
};
