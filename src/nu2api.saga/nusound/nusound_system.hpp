#pragma once

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/nusound/nusound_memorymanager.hpp"

#include "decomp.h"

class NuSoundLoader;
class NuSoundBus;

class NuSoundSystem {
  public:
    enum MemoryDiscipline : u32 {
        SCRATCH = 0,
        SAMPLE = 1,
        DECODER = 2,
    };

    struct ChannelConfig {
        u32 channels;
    };

    enum FileType : u32 { FILE_TYPE_WAV = 0, FILE_TYPE_OGG = 5 };

  private:
    void **samples;
    u32 sample_count;

  public:
    static NuSoundBus *sMasterBus;

    static i32 sAllocdMemory[3];
    static i32 sTotalMemory[3];

    static void *sScratchMemory;
    static void *sSampleMemory;
    static void *sDecoderMemory;

    static NuSoundMemoryManager *s_mmSample;
    static NuSoundMemoryManager *s_mmDecoder;

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

  public:
    static NuSoundLoader *CreateFileLoader(FileType type);

    bool Initialise(i32 size);

    static void *_AllocMemory(MemoryDiscipline disc, u32 size, u32 align, const char *name);

    static u32 FreeMemory(MemoryDiscipline disc, usize address, u32 size);

    static u32 GetStreamBufferSize();
    static u32 GetScratchMemorySize();
    static u32 GetDecoderMemorySize();
    static u32 GetFreeMemory(MemoryDiscipline disc);

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

enum LoadState {
    LOADSTATE_LOADED = 1,
};
enum ErrorState {
    ERRORSTATE_NONE = 0,
};
