#pragma once

// NuSoundVoice / NuVoiceAndroid — decompiled from libTTapp.so
// (nu2api.2013/nusound/nusound.cpp, nusound/android/nusound_android.cpp).
//
// NuSoundVoice is the engine-side voice: the play state machine (1 stopped,
// 2 paused, 3 playing), volume/pitch, the eight output-channel gains fed from
// the positional mix, and the per-frame Update. NuVoiceAndroid is the platform
// voice on top of it: one OpenSL ES AudioPlayer per voice (play interface,
// ANDROIDSIMPLEBUFFERQUEUE, volume interface) that receives the mixed PCM.

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuelist.hpp"
#include "nu2api/nusound/nusound_buffer.hpp"
#include "nu2api/nusound/nusound_system.hpp"
#include "nu2api/nusound/nusound_weakptr.hpp"

#include <pthread.h>

struct SLPlayItf_;

class NuSoundSource;
class NuSoundBus;
class NuSoundEffect;
class NuSoundHandle;
class NuSoundListener;
class NuSoundRoutingTable;
struct nuvec_s;
struct VuMtx;
struct VuVec;

// The voice doubles as the NuSoundBufferCallback the sample hands filled
// buffers to (the original dispatched through the callback vtable slot +8,
// which is NuVoiceAndroid::SubmitBuffer).
class NuSoundBufferCallback {
  public:
    virtual ~NuSoundBufferCallback() {
    }
    virtual void SubmitBuffer(NuSoundBuffer *buffer) = 0;
};

class NuSoundVoice : public NuSoundBufferCallback {
  public:
    enum PlayState {
        PLAYSTATE_STOPPED = 1,
        PLAYSTATE_PAUSED = 2,
        PLAYSTATE_PLAYING = 3,
    };

    // Weak-reference bookkeeping (the original embedded NuSoundWeakPtrObj at
    // the start of the voice). queued_buffers counts buffers handed to the
    // hardware but not yet consumed; Play() refuses to double-start while it
    // is non-zero.
    NuSoundWeakPtrObj<NuSoundVoice> weak_ptr_obj;
    u32 queued_buffers;

    // System intrusive list links.
    NuSoundVoice *field_0x24;
    NuSoundVoice *field_0x28;

    // +0x2c sound source; +0x30/0x31 the flags/flags2 bytes.
    NuSoundSource *sound_source;
    u8 flags;  // low nybble: pause counter; bit3: request loop; bit4: mix update
    u8 flags2; // bit0: auto delete; bit1: last buffer queued; bit2: stop effects
               // running; bit3: looping (from the CreateVoice loop argument)

    NuSoundRoutingTable *field15_0x38;
    void *field16_0x3c; // default routing table

    // Effects list (elist nodes at +0x40..+0x48).
    NuEListNode<NuSoundEffect> *effects_start;
    NuEListNode<NuSoundEffect> *effects_end;
    NuEListNode<NuSoundEffect> *effects_tail;

    u32 field20_0x4c;
    u32 field23_0x58;

    // +0x5c..0x7c: the eight output channel gains (the positional mix).
    f32 mix_gains[8];

    void *field56_0x7c;

    void *field57_0x80;
    void *field58_0x84;
    void *field59_0x88;

    void *field60_0x8c;
    void *field61_0x90;
    void *field62_0x94;

    f32 field63_0x98;
    f32 field64_0x9c;
    f32 field65_0xa0; // falloff attenuation
    f32 field66_0xa4;
    f32 field67_0xa8; // final mix scalar fed to the hardware volume
    f32 field68_0xac; // pitch scale

    f32 falloff_a;    // 1.0
    f32 falloff_b;    // 6.0
    f32 field69_0xb0; // 20.0
    f32 field70_0xb4; // 180.0
    f32 field71_0xb8; // 70.0
    f32 field72_0xbc;
    f32 field73_0xc0;
    f32 field74_0xc4; // 1.0

    f32 volume; // +0xfc (SetVolume clamps to 0..1)
    f32 pitch;  // +0x100 (SetPitch clamps to >= 0)
    u32 field_0x104;
    u32 field_0x108;

    f32 field113_0x10c; // LFE gain
    u32 field114_0x110;
    u32 field115_0x114; // 1
    u32 field116_0x118;

    NuSoundBus *output_bus; // +0x11c, defaults to NuSoundSystem::sMasterBus

    void *field121_0x120;
    void *field122_0x124;
    void *field123_0x128;
    void *field124_0x12c;
    void *field125_0x130;
    void *field126_0x134;
    u32 field127_0x138;

    PlayState state; // +0x140, guarded by sStateCriticalSection
    f32 field130_0x144;
    i32 field131_0x148; // -1

    static pthread_mutex_t sStateCriticalSection;

  public:
    NuSoundVoice(NuSoundSource *sound_source, bool loop);
    virtual ~NuSoundVoice();

    // NuSoundBufferCallback: implemented by NuVoiceAndroid (the device write).
    void SubmitBuffer(NuSoundBuffer *buffer) override = 0;

    // Play state.
    PlayState GetState() const;
    void SetState(PlayState state);
    bool GetAutoDelete() const;
    void SetAutoDelete(bool auto_delete);
    void SetMixUpdate(bool mix_update);
    void SetVolume(f32 volume);
    void SetPitch(f32 pitch);

    // Control.
    void Play();
    void Pause();
    void Resume();
    void Stop(bool with_effects);
    void RegisterHandle(NuSoundHandle *handle);
    void UnregisterHandle(NuSoundHandle *handle);

    // Per-frame processing (NuSoundSystem::Update calls this on every voice).
    void Update(f32 frametime);
    void UpdateMix(f32 frametime);
    void CalculatePositionalMix();
    bool BeginStopEffects();
    bool CheckStopEffects();
    bool AreStopEffectsRunning() const;
    f32 CalculateEffectAttenuation();
    f32 CalculateEffectPitchScale();
    void UpdateEffects(f32 frametime, NuSoundEffect::EffectProcessStage stage);
    void CheckStarvedBuffers();

    // Platform half, dispatched through the object vtable in the original.
    virtual void StartHardwareVoice() = 0;               // vtable +0x20
    virtual void StopHardwareVoice() = 0;                // vtable +0x24
    virtual void PauseHardwareVoice() = 0;               // vtable +0x28
    virtual void ResumeHardwareVoice() = 0;              // vtable +0x2c
    virtual void UpdateHardwareVoice(f32 frametime) = 0; // vtable +0x30
    virtual void ApplyHardwareVoiceMix() = 0;            // vtable +0x34
    virtual u64 GetPlaybackPositionSamples() = 0;

    // Remaining original surface (off the title music path; kept as stubs).
    void AddEffect(NuSoundEffect *effect);
    void CalculateFalloffAttenuation(f32 distance);
    void CalculateFieldAngle(f32 angle);
    void CalculatePositionalCoefficients(f32 *gains, VuVec const &position, VuMtx const &mtx, f32 falloff_a,
                                         f32 falloff_b);
    void GetControllerBits() const;
    void GetDirection() const;
    void GetDownmixerType() const;
    void GetEffect(NuSoundEffect::EffectType type);
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
    void GetSurroundMode() const;
    void GetVelocity() const;
    void GetVolume() const;
    void IsLooping() const;
    void RemoveEffect(NuSoundEffect *effect);
    void SetControllerBits(i32 bits);
    void SetCustomSurroundMix(f32 *mix);
    void SetDirection(VuVec *direction);
    void SetDownmixerType(NuSoundSystem::DownmixType type);
    void SetFalloff(f32 near, f32 far, NuSoundSystem::FalloffType type);
    void SetLowFrequencyMix(f32 mix);
    void SetOutputBus(NuSoundBus *bus);
    void SetOutputDevices(i32 devices);
    void SetPenetration(f32 penetration);
    void SetPosition(VuVec *position);
    void SetReverbWetMix(f32 mix);
    void SetRoutingTable(NuSoundRoutingTable *table);
    void SetSpeakerBleedAngle(f32 angle);
    void SetSpeakerBleedFar(f32 far);
    void SetSpeakerBleedNear(f32 near);
    void SetSpeakerFieldAngle(f32 min, f32 max);
    void SetStartOffset(f32 offset);
    void SetListeners(NuEList<NuSoundListener, DefaultElist> const *listeners);
    void SetSurroundMode(NuSoundSystem::SurroundMode mode);
    void SetVelocity(VuVec const &velocity);

  protected:
    // The 3D positional state; only the surround_mode == 2 (2D omni) path is
    // exercised by the title music.
    u32 surround_mode;
    u8 falloff_type;
    NuEList<NuSoundListener, DefaultElist> const *listeners;
};

class NuVoiceAndroid : public NuSoundVoice {
  public:
    // OpenSL ES handles (opaque pointers exactly like the original; a host
    // override only has to provide what these point at).
    void *player_object;   // +0x14c SLObjectItf of the audio player
    void *play_interface;  // +0x150 SLPlayItf
    void *queue_interface; // +0x154 SLAndroidSimpleBufferQueueItf
    void *field4_0x158;
    void *volume_interface; // +0x15c SLVolumeItf

    pthread_mutex_t mutex; // +0x160

    u32 field7_0x164;  // playback block position (low)
    u32 field8_0x168;  // playback block position (high)
    u32 field9_0x16c;  // wrap counter (low)
    u32 field10_0x170; // wrap counter (high)
    i32 field11_0x174; // playback position in samples (low)
    i32 field12_0x178; // playback position in samples (high)

    i16 last_volume_level; // +0x17c centibels cache (-32768 = mute)
    u8 hardware_flags;     // +0x17e bit0: start; bit1: stop; bit2: request buffer

  public:
    NuVoiceAndroid(NuSoundSource *sound_source, bool loop);
    virtual ~NuVoiceAndroid();

    // NuSoundBufferCallback: the device write (Enqueue).
    void SubmitBuffer(NuSoundBuffer *buffer) override;

    void CreateHardwareVoice();
    bool RealiseObject();
    bool GetInterfaces();
    void StartHardwareVoice() override;
    void StopHardwareVoice() override;
    void PauseHardwareVoice() override;
    void ResumeHardwareVoice() override;
    void UpdateHardwareVoice(f32 frametime) override;
    void ApplyHardwareVoiceMix() override;
    void UpdateQueue();
    bool UpdateState();
    void UpdateSamplePlaybackCount();
    u64 GetPlaybackPositionSamples() override;
    void DestroyHardwareVoice();
    void OnPlayerEvent(u32 event);

    // The SL play-interface callback (static; registered by GetInterfaces).
    static void PlayerCallback(const SLPlayItf_ *const *player, void *context, u32 event);
};
