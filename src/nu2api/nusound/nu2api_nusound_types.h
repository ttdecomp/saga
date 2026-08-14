#ifndef NU2API_NUSOUND_TYPES_H
#define NU2API_NUSOUND_TYPES_H
#pragma once

#include "nu2api/nucore/fixed_width.h"

#include "nu2api/nucore/nuelist.hpp"
#include "nu2api/nucore/nuvuvec.hpp"
#include "nu2api/nusound/nusound_system.hpp"
#include "nu2api/nusound/nusound_weakptr.hpp"

class NuSoundBufferCallback;

struct FileHeaderWAV;
struct NuSoundAndroid;
struct NuSoundBuffer;
struct NuSoundBus;
struct NuSoundClock;
struct NuSoundDecodeThread;
struct NuSoundDecoder;
struct NuSoundDecoderOGG;
struct NuSoundEffect;
struct NuSoundEffectDoppler;
struct NuSoundEffectFader;
struct NuSoundEffectPitchRamp;
struct NuSoundHandle;
struct NuSoundListener;
struct NuSoundLoader;
struct NuSoundLoaderOGG;
struct NuSoundLoaderWAV;
struct NuSoundMemoryBuffer;
struct NuSoundMemoryManager;
struct NuSoundMixMatrix;
struct NuSoundMixer;
struct NuSoundOutOfMemCallback;
struct NuSoundRoutingTable;
struct NuSoundSample;
struct NuSoundSource;
struct NuSoundStreamDesc;
struct NuSoundStreamer;
struct NuSoundStreamingSample;
struct NuSoundSystem;
struct NuSoundVoice;
struct NuSoundVoiceFactory;
struct NuSoundVoiceFactoryAndroid_PCM;
struct NuSoundVoiceFactoryList;
struct NuThread;
struct OPTIONSSAVE_s;
struct VuMtx;
struct VuVec;
struct WORLDINFO_s;
struct nuvec_s;

struct FileHeaderWAV {};
struct NuSoundMixMatrix {};
struct NuSoundVoiceFactory {};
struct OPTIONSSAVE_s;
struct WORLDINFO_s;
struct nuvec_s;

struct NuSoundBuffer {
    void GetCurrentContext();
    void GetSegmentAddress(u32, u32, u32) const;
    void GetSegmentSize(u32, u32) const;
    ~NuSoundBuffer();
};
struct NuSoundBus {
    void AddEffect(NuSoundEffect *);
    void ApplyFinalMix(float *);
    void GetOutputMix(float *);
    NuSoundBus(char const *, NuSoundBus *);
    void RemoveEffect(NuSoundEffect *);
    void SetOutputBus(NuSoundBus *);
    void SetOutputMix(float);
    void SetOutputMix(float *);
    ~NuSoundBus();
};
struct NuSoundClock {
    struct Callback {};
    void AddCallback(NuSoundClock::Callback *);
    void GetClockFrequency() const;
    void GetTicks() const;
    void HandleCallbacks();
    NuSoundClock();
    void RemoveCallback(NuSoundClock::Callback *);
    ~NuSoundClock();
};
struct NuSoundDecodeThread {
    void Shutdown();
    void RequestDecode(NuSoundDecoder &, NuSoundBuffer &, NuSoundWeakPtr<NuSoundBufferCallback>, bool);
    ~NuSoundDecodeThread();
};
struct NuSoundDecoder {
    void CloseStream();
    void GetNumInitialBuffers() const;
    void GetNumRingBuffers() const;
    void Initialise();
    void IsLocked() const;
    void IsStreamOpen() const;
    void Lock();
    NuSoundDecoder(char const *, NuSoundSource *);
    void OpenStream(bool);
    void RequestBuffer(bool, NuSoundWeakPtr<NuSoundBufferCallback>);
    void Shutdown();
    void Unlock();
    void VoiceReference();
    void VoiceRelease();
    virtual ~NuSoundDecoder();
};
struct NuSoundDecoderOGG {
    struct OGGReadCallbacksDecoder {
        void Close();
        void GetPosition() const;
        OGGReadCallbacksDecoder();
        void Read(void *, u32);
        void Seek(i32, u32);
        void SetDecoder(NuSoundDecoderOGG *);
    };
    void Decode(NuSoundSource &, NuSoundBuffer &, bool);
    void DecodeOggChunk(char *, u32);
    NuSoundDecoderOGG(char const *, NuSoundSource *);
    void Reset();
    void SubmitBuffer(NuSoundBuffer *);
    virtual ~NuSoundDecoderOGG();
};
struct NuSoundEffectDoppler {
    NuSoundEffectDoppler();
    void ProcessVoice(NuSoundVoice *, float);
    void SetParameters(float, float, NuEList<NuSoundListener, DefaultElist> const *);
    virtual ~NuSoundEffectDoppler();
};
struct NuSoundEffectFader {
    struct Curve {};
    struct FinishState {};
    void AttachBus(NuSoundBus *);
    void AttachVoice(NuSoundVoice *);
    void Disable();
    void Enable();
    NuSoundEffectFader();
    void Process(float);
    void ProcessBus(NuSoundBus *, float);
    void ProcessVoice(NuSoundVoice *, float);
    void SetCurveParams(NuSoundEffectFader::Curve const &);
    void SetParameters(float, float, NuSoundEffectFader::FinishState);
    virtual ~NuSoundEffectFader();
};
struct NuSoundEffectPitchRamp {
    struct FinishState {};
    void AttachVoice(NuSoundVoice *);
    NuSoundEffectPitchRamp();
    void Process(float);
    void ProcessVoice(NuSoundVoice *, float);
    void SetParameters(float, float, NuSoundEffectPitchRamp::FinishState);
    virtual ~NuSoundEffectPitchRamp();
};
struct NuSoundHandle {
    void AddEffect(NuSoundEffect *);
    void GetEffect(NuSoundEffect::EffectType);
    void GetFalloffType() const;
    void GetFar() const;
    void GetLastAttenuation() const;
    void GetLastAttenuationListener() const;
    void GetLastDistanceAttenuation() const;
    void GetLastListenerDistance() const;
    void GetLastPositionalListener() const;
    void GetNear() const;
    void GetPitch() const;
    void GetPlaybackPositionSamples();
    void GetPlaybackPositionSeconds() const;
    void GetPosition() const;
    void GetState() const;
    void GetSurroundMode() const;
    void GetTotalLengthSamples() const;
    void GetTotalLengthSeconds() const;
    void GetVelocity() const;
    void GetVoice() const;
    void GetVolume() const;
    void InvalidateVoice();
    void IsLooping() const;
    NuSoundHandle();
    NuSoundHandle(NuSoundHandle &);
    void Pause();
    void Play();
    void RemoveEffect(NuSoundEffect *);
    void ResetFrameCount();
    void Resume();
    void SetFalloff(float, float, NuSoundSystem::FalloffType);
    void SetPitch(float);
    void SetPosition(VuVec *);
    void SetVelocity(VuVec const &);
    void SetVoice(NuSoundVoice *);
    void SetVolume(float);
    void Stop();
    void operator=(NuSoundHandle &);
    void operator==(NuSoundHandle const &);
    ~NuSoundHandle();
};
struct NuSoundListener {
    void Disable();
    void DisableFocusPosition();
    void Enable();
    void EnableFocusPosition();
    void Get2DScreenPosition() const;
    void GetAttenuationDistance(VuVec const &) const;
    void GetAttenuationPosition(VuVec const &) const;
    void GetFocusPosition() const;
    void GetHeadDistance(VuVec const &) const;
    void GetHeadMatrix() const;
    void GetOutputDevices() const;
    void GetSensitivity() const;
    void GetVelocity() const;
    void IsEnabled() const;
    void IsFocusPositionEnabled() const;
    NuSoundListener();
    void Set2DScreenPosition(VuVec const *);
    void SetFocusPosition(VuVec const *);
    void SetHeadMatrix(VuMtx const *);
    void SetOutputDevices(i32);
    void SetSensitivity(float);
    void SetVelocity(VuVec const &);
    ~NuSoundListener();
};
struct NuSoundLoader {
    void CloseStream();
    void Deinterleave(char *, i32, char **, i32, NuSoundSystem::ChannelConfig);
    void GetChannelAddress(NuSoundBuffer *, NuSoundStreamDesc *, NuSoundSystem::AudioChannel);
    void ReleaseHeader(NuSoundStreamDesc *);
};
struct NuSoundLoaderOGG {
    struct OGGFileCallbacks {
        void Close();
        void GetFile() const;
        void GetPosition() const;
        void Seek(i32, u32);
    };
    void Close();
    void OggCallbackClose(void *);
    void OggCallbackSeek(void *, i64, i32);
    void OggCallbackTell(void *);
    void OpenFileForStreaming(char const *, bool);
    void SeekRawData(u64);
};
struct NuSoundLoaderWAV {
    struct ChunkInfo {};
    struct ChunkReadRequest {};
    void CreateHeader();
    void EndianFlipWAVHeader(FileHeaderWAV *);
    void FindChunk(i32, u32, NuSoundLoaderWAV::ChunkInfo &);
    void FindChunks(i32, NuSoundStreamDesc *, NuSoundLoaderWAV::ChunkReadRequest *, u32);
    void MakeFourCC(char *);
    NuSoundLoaderWAV();
    void ReadDataChunk(i32, NuSoundStreamDesc *, NuSoundLoaderWAV::ChunkInfo const &, NuSoundLoaderWAV *);
    void ReadHeader(NuSoundStreamDesc *);
    void ReadRIFFHeaderChunk(i32, NuSoundStreamDesc *, NuSoundLoaderWAV::ChunkInfo const &, NuSoundLoaderWAV *);
    void SeekPCMSample(u64);
    void SeekTime(double);
    virtual ~NuSoundLoaderWAV();
};
struct NuSoundMixer {
    struct OutputLayout {};
    void GetOutputIndex(i32, i32);
    void Mix(float *, float *);
    NuSoundMixer(NuSoundSystem::ChannelConfig, NuSoundSystem::ChannelConfig, NuSoundMixer::OutputLayout,
                 NuSoundSystem::DownmixType, NuSoundRoutingTable *);
    ~NuSoundMixer();
};
struct NuSoundRoutingTable {
    void GetConfig(i32);
    void GetIndex(NuSoundSystem::ChannelConfig);
    void GetMatrix(NuSoundSystem::ChannelConfig, NuSoundSystem::ChannelConfig) const;
    void GetName() const;
    NuSoundRoutingTable(char const *);
    NuSoundRoutingTable(char const *, NuSoundRoutingTable const *);
    void SetMatrix(NuSoundSystem::ChannelConfig, NuSoundSystem::ChannelConfig, NuSoundMixMatrix *);
};
struct NuSoundSample {
    struct ErrorState {};
    struct LoadState {};
    void GetSourceBuffer();
    void IsLocked() const;
    void Lock();
    void RequestBuffer(bool, NuSoundWeakPtr<NuSoundBufferCallback>);
    void SetLastErrorState(NuSoundSample::ErrorState);
    void SetLoadState(NuSoundSample::LoadState);
    void Unload();
    void Unlock();
    virtual ~NuSoundSample();
};
struct NuSoundStreamDesc {
    struct DataFormat {};
};
struct NuSoundStreamer {
    void RequestClose(NuSoundStreamingSample *);
    void RequestFill(NuSoundStreamingSample *, NuSoundBuffer *, bool, NuSoundWeakPtr<NuSoundBufferCallback>);
    void RequestReCue(NuSoundStreamingSample *, bool, float);
    void ShutdownAll();
    void ShutdownThread();
    ~NuSoundStreamer();
};
struct NuSoundStreamingSample {
    void Close();
    void IsLocked() const;
    void Lock();
    void ReCue(float, bool);
    void RequestBuffer(bool, NuSoundWeakPtr<NuSoundBufferCallback>);
    void Unlock();
    virtual ~NuSoundStreamingSample();
};
struct NuSoundVoice {
    struct PlayState {};
    NuSoundVoice(NuSoundSource *, bool);
    void AddEffect(NuSoundEffect *);
    void AreStopEffectsRunning() const;
    void BeginStopEffects();
    void CalculateEffectAttenuation();
    void CalculateEffectPitchScale();
    void CalculateFalloffAttenuation(float);
    void CalculateFieldAngle(float);
    void CalculatePositionalCoefficients(float *, VuVec const &, VuMtx const &, float, float);
    void CalculatePositionalMix();
    void CheckStopEffects();
    void GetAutoDelete() const;
    void GetControllerBits() const;
    void GetDirection() const;
    void GetDownmixerType() const;
    void GetEffect(NuSoundEffect::EffectType);
    void GetFalloffType() const;
    void GetFar() const;
    void GetLowFrequencyMix() const;
    void GetNear() const;
    void GetNumEffects() const;
    void GetOutputBus() const;
    void GetPenetration() const;
    void GetPitch() const;
    void GetPlaybackPositionSeconds();
    void GetPosition() const;
    void GetReverbWetMix() const;
    void GetRoutingTable() const;
    void GetSpeakerBleedAngle() const;
    void GetSpeakerBleedFar() const;
    void GetSpeakerBleedNear() const;
    void GetSpeakerFieldAngleMax() const;
    void GetSpeakerFieldAngleMin() const;
    void GetStartOffset() const;
    void GetState() const;
    void GetSurroundMode() const;
    void GetVelocity() const;
    void GetVolume() const;
    void IsLooping() const;
    void Pause();
    void Play();
    void RegisterHandle(NuSoundHandle *);
    void RemoveEffect(NuSoundEffect *);
    void Resume();
    void SetAutoDelete(bool);
    void SetControllerBits(i32);
    void SetCustomSurroundMix(float *);
    void SetDirection(VuVec *);
    void SetDownmixerType(NuSoundSystem::DownmixType);
    void SetFalloff(float, float, NuSoundSystem::FalloffType);
    void SetLowFrequencyMix(float);
    void SetMixUpdate(bool);
    void SetOutputBus(NuSoundBus *);
    void SetOutputDevices(i32);
    void SetPenetration(float);
    void SetPitch(float);
    void SetPosition(VuVec *);
    void SetReverbWetMix(float);
    void SetRoutingTable(NuSoundRoutingTable *);
    void SetSpeakerBleedAngle(float);
    void SetSpeakerBleedFar(float);
    void SetSpeakerBleedNear(float);
    void SetSpeakerFieldAngle(float, float);
    void SetStartOffset(float);
    void SetListeners(NuEList<NuSoundListener, DefaultElist> const *);
    void SetState(NuSoundVoice::PlayState);
    void SetSurroundMode(NuSoundSystem::SurroundMode);
    void SetVelocity(VuVec const &);
    void SetVolume(float);
    void Stop(bool);
    void UnregisterHandle(NuSoundHandle *);
    void Update(float);
    void UpdateEffects(float, NuSoundEffect::EffectProcessStage);
    void UpdateMix(float);
    virtual ~NuSoundVoice();
};
struct NuSoundVoiceFactoryAndroid_PCM {
    void CreateVoice(NuSoundSource *, bool);
};
struct NuSoundVoiceFactoryList {
    void GetFactory(NuSoundStreamDesc::DataFormat);
    NuSoundVoiceFactoryList();
    void RegisterFactory(NuSoundVoiceFactory *, NuSoundStreamDesc::DataFormat);
};

#endif // NU2API_NUSOUND_TYPES_H