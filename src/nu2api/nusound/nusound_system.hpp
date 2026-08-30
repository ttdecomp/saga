#pragma once

#include "nu2api/nucore/NuMemoryManager.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuelist.hpp"
#include "nu2api/nucore/nuvuvec.hpp"
#include "nu2api/nusound/nusound_memorymanager.hpp"
#include "nu2api/nusound/nusound_source.hpp"
#include "nu2api/nusound/nusound_streamdesc.hpp"

#include "nu2api/nufile/nufile.h"

#include "decomp.h"

struct VuMtx;

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
    enum class EffectProcessStage : u32 {
        ZERO = 0,
        ONE = 1,
    };
    virtual ~NuSoundEffect();
};

// --- subsystem classes without a dedicated header (definitions live in their
// --- .cpp files; the declarations used to sit in the legacy types catalog) ---

class NuSoundClock {
  public:
    struct Callback {};

    NuSoundClock();
    ~NuSoundClock();

    void AddCallback(Callback *callback);
    void RemoveCallback(Callback *callback);
    void HandleCallbacks();
    void GetClockFrequency() const;
    void GetTicks() const;
};

class NuSoundListener {
  public:
    NuSoundListener();
    ~NuSoundListener();

    void Enable();
    void Disable();
    void IsEnabled() const;
    void SetHeadMatrix(const VuMtx *mtx);
    void GetHeadMatrix() const;
    void SetFocusPosition(const VuVec *position);
    void GetFocusPosition() const;
    void EnableFocusPosition();
    void DisableFocusPosition();
    void IsFocusPositionEnabled() const;
    void Set2DScreenPosition(const VuVec *position);
    void Get2DScreenPosition() const;
    void SetVelocity(const VuVec &velocity);
    void GetVelocity() const;
    void SetSensitivity(f32 sensitivity);
    void GetSensitivity() const;
    void SetOutputDevices(i32 devices);
    void GetOutputDevices() const;
    void GetAttenuationPosition(const VuVec &position) const;
    void GetAttenuationDistance(const VuVec &position) const;
    void GetHeadDistance(const VuVec &position) const;
};

class NuSoundEffectDoppler {
  public:
    NuSoundEffectDoppler();
    virtual ~NuSoundEffectDoppler();

    void ProcessVoice(NuSoundVoice *voice, f32 frametime);
    void SetParameters(f32 a, f32 b, const NuEList<NuSoundListener, DefaultElist> *listeners);
};

class NuSoundEffectFader {
  public:
    struct Curve {};
    struct FinishState {};

    NuSoundEffectFader();
    virtual ~NuSoundEffectFader();

    void AttachBus(NuSoundBus *bus);
    void AttachVoice(NuSoundVoice *voice);
    void Enable();
    void Disable();
    void Process(f32 frametime);
    void ProcessBus(NuSoundBus *bus, f32 frametime);
    void ProcessVoice(NuSoundVoice *voice, f32 frametime);
    void SetCurveParams(const Curve &curve);
    void SetParameters(f32 a, f32 b, FinishState state);
};

class NuSoundEffectPitchRamp {
  public:
    struct FinishState {};

    NuSoundEffectPitchRamp();
    virtual ~NuSoundEffectPitchRamp();

    void AttachVoice(NuSoundVoice *voice);
    void Process(f32 frametime);
    void ProcessVoice(NuSoundVoice *voice, f32 frametime);
    void SetParameters(f32 a, f32 b, FinishState state);
};

// Voice factories (nu2api.2013/nusound/nusound.cpp): one factory per decoded
// data format, indexed by NuSoundStreamDesc::DataFormat.
class NuSoundVoiceFactory {
  public:
    virtual ~NuSoundVoiceFactory() {
    }
    virtual NuSoundVoice *CreateVoice(NuSoundSource *source, bool loop) = 0;
};

class NuSoundVoiceFactoryAndroid_PCM : public NuSoundVoiceFactory {
  public:
    NuSoundVoice *CreateVoice(NuSoundSource *source, bool loop) override;
};

class NuSoundVoiceFactoryList {
  public:
    NuSoundVoiceFactory *factories[16];

  public:
    NuSoundVoiceFactoryList();
    void RegisterFactory(NuSoundVoiceFactory *factory, NuSoundStreamDesc::DataFormat format);
    NuSoundVoiceFactory *GetFactory(NuSoundStreamDesc::DataFormat format);
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
    enum class FalloffType { LINEAR = 0 };
    enum class SurroundMode { ZERO = 0 };

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

  public:
    pthread_mutex_t mutex;

  public:
    // Voice bookkeeping: an intrusive doubly-linked list of all live voices
    // (links live in the voices at +0x24/+0x28), the per-format voice factory
    // and the audio clock.
    NuSoundVoice *voice_list_start;
    NuSoundVoice *voice_list_end;
    i32 voice_count;
    NuSoundVoiceFactoryList factory_list;
    NuSoundClock clock;
    bool initialised;

    // OpenSL ES handles (set by InitAudioDevice; a host override only has to
    // provide what these point at).
    void *engine_object; // SL engine object (realize / GetInterface / destroy)
    void *audio_engine;  // SLEngineItf (CreateAudioPlayer / CreateOutputMix)
    void *output_mix;    // output mix object

  public:
    static NuSoundBus *sMasterBus;
    static NuSoundRoutingTable *sDefaultRoutingTable;
    static i32 sNumAvailableOutputDevices;
    static i32 sOutputConfig;

    static i32 sAllocdMemory[3];
    static i32 sTotalMemory[3];

    static void *sScratchMemory;
    static void *sSampleMemory;
    static void *sDecoderMemory;

    static NuSoundMemoryManager *s_mmSample;
    static NuSoundMemoryManager *s_mmDecoder;

    static const char *sFileExtensions[12];

    static NuSoundSystem *s_staticInstance;

    static NuSoundSystem *GetInstance() {
        return s_staticInstance;
    }

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

    virtual NuSoundVoice *CreateVoice(NuSoundSource *source, bool loop);
    void ReleaseVoice(NuSoundVoice *voice);
    bool SourceRequiresDecoder(NuSoundSource *source);
    virtual void UpdateAudioDevice() = 0;
    i32 GetNumAvailableOutputDevices();
    NuSoundRoutingTable *GetDefaultRoutingTable();
    void Update(f32 frametime);

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
    NuSoundDecoder *CreateDecoder(NuSoundSource *source);
    void CreateEffect(NuSoundEffect::EffectType);
    void DefragmentSampleMemory();
    void DetermineFileType(NUFILETYPE);
    void Disable();
    void FileTypeSupported(NuSoundSystem::FileType);
    void Get();
    void GetAllocdMemory(NuSoundSystem::MemoryDiscipline);
    void GetBufferAlignment();
    i32 GetClosestSupportedConfig(i32 config);
    void GetCrossfadeCurve(unsigned int) const;
    void GetDefaultFileType(NuSoundSource::FeedType);
    void GetGfxMemorySize();
    void GetLanguageString(bool);
    void GetLargestMemoryFragment(NuSoundSystem::MemoryDiscipline);
    void GetListeners();
    NuSoundListener *GetNearestRealListener(NuEList<NuSoundListener, DefaultElist> const &, VuVec const &);
    NuSoundListener *GetNearestFocusListener(NuEList<NuSoundListener, DefaultElist> const &, VuVec const &, float &);
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
    void ReleaseDecoder(NuSoundDecoder *decoder);
    void ReleaseEffect(NuSoundEffect *);
    void ReleaseSample(NuSoundSample *);
    void RemoveListener(NuSoundListener *);
    void ResumeAllVoices();
    void ResumeVoices(int);
    void SetDefaultRoutingTable(NuSoundRoutingTable *);
    void SetGfxMemorySize(unsigned int);
    void SetMainThreadID(NuThread *);
    void Shutdown();
    void StopAllVoices();
    void StopVoices(NuSoundSource const &);
    void StopVoices(int);
    void UnloadAllSamples();
    void UnloadSample(NuSoundSample *);
    void dBToAmplitude(float);
};
// One row of a routing table: an N-in by M-out gain matrix pointing at one of
// the static NuSoundRoutingTable* matrices.
struct NuSoundMixMatrix {
    u32 in;
    u32 out;
    f32 *matrix;
    u32 flag;
};

class NuSoundMixer {
  public:
    struct OutputLayout {
        u32 layout;
    };

    NuSoundMixer(NuSoundSystem::ChannelConfig config, NuSoundSystem::ChannelConfig output,
                 NuSoundMixer::OutputLayout layout, NuSoundSystem::DownmixType downmix, NuSoundRoutingTable *table);
    ~NuSoundMixer();

    void Mix(f32 *in, f32 *out);
    void GetOutputIndex(i32 output, i32 index);
};
class NuSoundRoutingTable {
  public:
    NuSoundRoutingTable(const char *name);
    NuSoundRoutingTable(const char *name, const NuSoundRoutingTable *parent);
    ~NuSoundRoutingTable();

    void SetMatrix(NuSoundSystem::ChannelConfig from, NuSoundSystem::ChannelConfig to, NuSoundMixMatrix *matrix);
    void GetMatrix(NuSoundSystem::ChannelConfig from, NuSoundSystem::ChannelConfig to) const;
    void GetConfig(i32 config);
    void GetIndex(NuSoundSystem::ChannelConfig config);
    void GetName() const;
};
class NuSoundHandle {
  public:
    NuSoundHandle();
    NuSoundHandle(NuSoundHandle &other);
    ~NuSoundHandle();

    void SetVoice(NuSoundVoice *voice);
    void GetVoice() const;
    void InvalidateVoice();
    void Play();
    void Pause();
    void Resume();
    void Stop();
    void SetVolume(f32 volume);
    void GetVolume() const;
    void SetPitch(f32 pitch);
    void GetPitch() const;
    void SetPosition(VuVec *position);
    void GetPosition() const;
    void SetVelocity(const VuVec &velocity);
    void GetVelocity() const;
    void SetFalloff(f32 near, f32 far, NuSoundSystem::FalloffType type);
    void GetFalloffType() const;
    void GetNear() const;
    void GetFar() const;
    void GetState() const;
    void IsLooping() const;
    void GetPlaybackPositionSamples();
    void GetPlaybackPositionSeconds() const;
    void GetTotalLengthSamples() const;
    void GetTotalLengthSeconds() const;
    void GetLastAttenuation() const;
    void GetLastAttenuationListener() const;
    void GetLastDistanceAttenuation() const;
    void GetLastListenerDistance() const;
    void GetLastPositionalListener() const;
    void GetSurroundMode() const;
    void AddEffect(NuSoundEffect *effect);
    void RemoveEffect(NuSoundEffect *effect);
    void GetEffect(NuSoundEffect::EffectType type);
    void ResetFrameCount();
    void operator=(NuSoundHandle &other);
    void operator==(NuSoundHandle const &other);
}; // namespace NuSoundSystem

class NuSoundOutOfMemCallback {
  public:
    virtual void operator()() = 0;
};
