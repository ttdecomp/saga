#pragma once

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/nusound/nusound_memorymanager.hpp"
#include "nu2api.saga/nusound/nusound_source.hpp"

#include "decomp.h"

class NuSoundLoader;
class NuSoundBus;
class NuSoundSample;

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

    static struct : NuMemoryManager::IEventHandler {
        u32 unknown;
        void *scratch;
        u32 scratch_size;

        virtual bool AllocatePage(NuMemoryManager *manager, u32 size, u32 _unknown) {
            UNIMPLEMENTED("g_handler::AllocatePage");
        }
        virtual bool ReleasePage(NuMemoryManager *manager, void *ptr, u32 _unknown) {
            UNIMPLEMENTED("g_handler::ReleasePage");
        }
    } g_handler;

    static NuMemoryManager *sScratchMemMgr;

    NuSoundSystem();

  public:
    static NuSoundLoader *CreateFileLoader(FileType type);

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

    // vtable:
    // create_effect
    // release_effect
    // release_bus
    // is_user_playing_music
    // pause_user_music
    // resume_user_music
    // title_has_user_music_control
    // on_enter_system_menu
    // on_exit_system_menu
    // init_audio_device
    // shutdown_audio_device
    // update_audio_device

    virtual ~NuSoundSystem() = default;
    virtual bool InitAudioDevice() = 0;
    virtual NuSoundBus *CreateBus(const char *name, bool is_master);
    virtual NuSoundBus *GetBus(const char *name);

}; // namespace NuSoundSystem

class NuSoundOutOfMemCallback {
  public:
    virtual void operator()() = 0;
};

class NuSoundStreamDesc {};

enum class LoadState {
    NOT_LOADED = 0,
    LOADED = 1,
};
enum class ErrorState {
    NONE = 0,
};
