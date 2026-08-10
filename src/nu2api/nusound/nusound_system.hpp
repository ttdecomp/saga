#pragma once

#include "nu2api/nucore/NuMemoryManager.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nusound/nusound_memorymanager.hpp"
#include "nu2api/nusound/nusound_source.hpp"

#include "decomp.h"

#include <vorbis/vorbisfile.h>

class NuSoundLoader;
class NuSoundBus;
class NuSoundSample;
class NuSoundVoice;
class NuSoundEffect {
  public:
    enum class EffectType : u32;
};

class NuSoundSystem {
  public:
    enum class MemoryDiscipline : u32 {
        SCRATCH = 0,
        SAMPLE = 1,
        DECODER = 2,
    };

    struct ChannelConfig {
        u32 channels;
    };

    // "wav", "adp", "ima", "caf", "xma", "ogg",  "dsp", "msf", "vag", "gcm", "wua", "cbx"
    enum class FileType : u32 {
        WAV = 0,
        ADP = 1,
        IMA = 2,
        CAF = 3,
        XMA = 4,
        OGG = 5,
        DSP = 6,
        MSF = 7,
        VAG = 8,
        GCM = 9,
        WUA = 10,
        CBX = 11,
        _COUNT = 12,
        INVALID = 13,
    };

  private:
    u8 unknown_0x4[0x4c];
    NuSoundSample **samples;
    u32 sample_count;
    pthread_mutex_t mutex;

  public:
    static NuSoundBus *sMasterBus;

    static i32 sAllocdMemory[3];
    static i32 sTotalMemory[3];

    static void *sScratchMemory;
    static void *sSampleMemory;
    static void *sDecoderMemory;

    static NuSoundMemoryManager *s_mmSample;
    static NuSoundMemoryManager *s_mmDecoder;

    static const char *sFileExtensions[12];

    static NuSoundSystem *s_staticInstance;

    static NuMemoryManager *sScratchMemMgr;

    NuSoundSystem();

  public:
    static NuSoundLoader *CreateFileLoader(FileType type);
    static void ReleaseFileLoader(NuSoundLoader *loader);

    bool Initialise(i32 size);

    static void *_AllocMemory(MemoryDiscipline disc, u32 size, u32 align, const char *name);

    static u32 FreeMemory(MemoryDiscipline disc, usize address, u32 size);

    static u32 GetStreamBufferSize();
    static u32 GetScratchMemorySize();
    static u32 GetDecoderMemorySize();
    static u32 GetFreeMemory(MemoryDiscipline disc);

    NuSoundSample *AddSample(const char *name, FileType file_type, NuSoundSource::FeedType feed_type);

    const char *GetFileExtension(FileType type);
    static FileType DetermineFileType(const char *path);

    NuSoundSample *GetSample(const char *path);

    i32 GenerateHash(const char *str);

    virtual ~NuSoundSystem() = default;
    virtual NuSoundEffect *CreateEffect(NuSoundEffect::EffectType type) = 0;
    virtual void ReleaseEffect(NuSoundEffect *effect) {}
    virtual NuSoundBus *CreateBus(const char *name, bool is_master);
    virtual NuSoundBus *GetBus(const char *name);
    virtual void ReleaseBus(NuSoundBus *bus) {}
    virtual bool IsUserPlayingMusic() { return false; }
    virtual void PauseUserMusic() {}
    virtual void ResumeUserMusic() {}
    virtual bool TitleHasUserMusicControl() { return true; }
    virtual void OnEnterSystemMenu() {}
    virtual void OnExitSystemMenu() {}
    virtual bool InitAudioDevice() = 0;
    virtual void ShutdownAudioDevice() = 0;
    virtual void UpdateAudioDevice() = 0;
    virtual NuSoundVoice *CreateVoice(NuSoundSource *source, bool flag) = 0;

}; // namespace NuSoundSystem

class NuSoundOutOfMemCallback {
  public:
    virtual void operator()() = 0;
};
