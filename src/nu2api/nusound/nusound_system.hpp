#pragma once

#include "nu2api/nucore/NuMemoryManager.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuelist.hpp"
#include "nu2api/nucore/nuvuvec.hpp"
#include "nu2api/nusound/nusound_memorymanager.hpp"
#include "nu2api/nusound/nusound_source.hpp"

#include "nu2api/nufile/nufile.h"

#include "decomp.h"

#include <vorbis/vorbisfile.h>

class NuSoundLoader;
class NuSoundBus;
class NuSoundSample;
class NuSoundListener;
class NuSoundRoutingTable;
class NuSoundDecoder;
class NuSoundVoice;
class NuThread;
class NuSoundOutOfMemCallback;

class NuSoundEffect {
  public:
    struct EffectType {};
    struct EffectProcessStage {};
    virtual ~NuSoundEffect();
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

    struct AudioChannel {};
    struct CurveData {};
    struct DownmixType {};
    struct FalloffType {};
    struct SurroundMode {};

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

    virtual ~NuSoundSystem();
    virtual bool InitAudioDevice() = 0;
    virtual NuSoundBus *CreateBus(const char *name, bool is_master);
    virtual NuSoundBus *GetBus(const char *name);

    void AddListener(NuSoundListener *);
    void AddRoutingTable(NuSoundRoutingTable *);
    void AmplitudeTodB(float);
    void CalculateCrossfadeHeight(NuSoundSystem::CurveData const &, float) const;
    void CreateCrossfadeCurve(unsigned int);
    void CreateDecoder(NuSoundSource *);
    void CreateEffect(NuSoundEffect::EffectType);
    void CreateVoice(NuSoundSource *, bool);
    void DefragmentSampleMemory();
    void DetermineFileType(NUFILETYPE);
    void Disable();
    void FileTypeSupported(NuSoundSystem::FileType);
    void Get();
    void GetAllocdMemory(NuSoundSystem::MemoryDiscipline);
    void GetBufferAlignment();
    void GetClosestSupportedConfig(int);
    void GetCrossfadeCurve(unsigned int) const;
    void GetDefaultFileType(NuSoundSource::FeedType);
    void GetDefaultRoutingTable();
    void GetGfxMemorySize();
    void GetLanguageString(bool);
    void GetLargestMemoryFragment(NuSoundSystem::MemoryDiscipline);
    void GetListeners();
    NuSoundListener *GetNearestRealListener(NuEList<NuSoundListener, DefaultElist> const &, VuVec const &);
    NuSoundListener *GetNearestFocusListener(NuEList<NuSoundListener, DefaultElist> const &, VuVec const &, float &);
    void GetNumAvailableOutputDevices();
    void GetOldestVoice(NuSoundSample *, float &);
    void GetOutputChannelConfig();
    void GetPeakAllocdMemory(NuSoundSystem::MemoryDiscipline);
    void GetPlatformString();
    void GetQuietestVoice(NuSoundSample *, float &);
    void GetRoutingTable(char const *);
    void GetTotalMemory(NuSoundSystem::MemoryDiscipline);
    void LoadSample(NuSoundSample *, void *, int, NuSoundOutOfMemCallback *);
    void PauseAllVoices();
    void PauseVoices(int);
    void ReAllocMemory(NuSoundSystem::MemoryDiscipline, unsigned int, unsigned int);
    void ReleaseBus(NuSoundBus *);
    void ReleaseCrossfadeCurve(unsigned int);
    void ReleaseDecoder(NuSoundDecoder *);
    void ReleaseEffect(NuSoundEffect *);
    void ReleaseSample(NuSoundSample *);
    void ReleaseVoice(NuSoundVoice *);
    void RemoveListener(NuSoundListener *);
    void ResumeAllVoices();
    void ResumeVoices(int);
    void SetDefaultRoutingTable(NuSoundRoutingTable *);
    void SetGfxMemorySize(unsigned int);
    void SetMainThreadID(NuThread *);
    void Shutdown();
    void SourceRequiresDecoder(NuSoundSource *);
    void StopAllVoices();
    void StopVoices(NuSoundSource const &);
    void StopVoices(int);
    void UnloadAllSamples();
    void UnloadSample(NuSoundSample *);
    void Update(float);
    void dBToAmplitude(float);
}; // namespace NuSoundSystem

class NuSoundOutOfMemCallback {
  public:
    virtual void operator()() = 0;
};
