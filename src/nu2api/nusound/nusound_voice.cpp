// NuSoundVoice — decompiled from libTTapp.so (nu2api.2013/nusound/nusound.cpp).
// Engine-side voice: play state machine, volume/pitch, the eight output
// gains fed from the positional mix, and the per-frame Update that pushes the
// mix down into the platform voice (NuVoiceAndroid).

#include "nu2api_nusound_types.h"

#include "decomp.h"

#include "nu2api/nucore/nuthread.h"
#include "nu2api/nusound/nusound_bus.hpp"
#include "nu2api/nusound/nusound_source.hpp"

#include <string.h>

pthread_mutex_t NuSoundVoice::sStateCriticalSection = PTHREAD_MUTEX_INITIALIZER;

// ---------------------------------------------------------------------------
// construction / destruction
// ---------------------------------------------------------------------------

NuSoundVoice::NuSoundVoice(NuSoundSource *sound_source, bool loop) {
    this->field_0x24 = NULL;
    this->field_0x28 = NULL;
    this->effects_start = NULL;
    this->effects_end = NULL;
    this->effects_tail = NULL;
    this->field20_0x4c = 0;
    this->field23_0x58 = 0;
    this->field57_0x80 = NULL;
    this->field58_0x84 = NULL;
    this->field59_0x88 = NULL;
    this->field60_0x8c = NULL;
    this->field61_0x90 = NULL;
    this->field62_0x94 = NULL;
    this->field121_0x120 = NULL;
    this->field122_0x124 = NULL;
    this->field123_0x128 = NULL;
    this->field124_0x12c = NULL;
    this->field125_0x130 = NULL;
    this->field126_0x134 = NULL;
    this->field127_0x138 = 0;
    this->weak_ptr_obj.head = NULL;
    this->weak_ptr_obj.tail = NULL;
    this->weak_ptr_obj.weak_count = 0;
    this->queued_buffers = 0;

    // The source is locked for the lifetime of the voice and keeps the stream
    // open until the voice releases it.
    sound_source->IsStreamOpen();
    sound_source->Lock();
    sound_source->VoiceReference();
    this->sound_source = sound_source;

    memset(this->mix_gains, 0, sizeof(this->mix_gains));

    this->field63_0x98 = 0.0f;
    this->field64_0x9c = 0.0f;
    this->field65_0xa0 = 1.0f; // falloff attenuation
    this->field66_0xa4 = 0.0f;
    this->field67_0xa8 = 1.0f; // final mix scalar
    this->field68_0xac = 1.0f; // pitch scale
    this->volume = 1.0f;
    this->pitch = 1.0f;
    this->falloff_a = 1.0f;
    this->falloff_b = 6.0f;
    this->field69_0xb0 = 20.0f;
    this->field70_0xb4 = 180.0f;
    this->field71_0xb8 = 70.0f;
    this->field72_0xbc = 0.0f;
    this->field73_0xc0 = 0.0f;
    this->field74_0xc4 = 1.0f;
    this->field_0x104 = 0;
    this->field_0x108 = 0;
    this->field113_0x10c = 1.0f; // LFE gain
    this->field114_0x110 = 0;
    this->field115_0x114 = 1;
    this->field116_0x118 = 0;
    this->output_bus = NuSoundSystem::sMasterBus;
    this->field15_0x38 = NULL;
    this->field16_0x3c = NuSoundSystem::sDefaultRoutingTable;
    this->surround_mode = 2; // 2D omni
    this->falloff_type = 0;
    this->field130_0x144 = 1.0f;
    this->field131_0x148 = -1;

    this->flags2 = (u8)(this->flags2 & 0xf6 | loop << 3);
    this->flags2 &= 0xf9;
    this->flags = (u8)(this->flags & 0xf0 | 0x10); // mix update on the first Update

    this->SetState(PLAYSTATE_STOPPED); // libTTapp.so ctor tail (0x3275b9)
}

NuSoundVoice::~NuSoundVoice() {
    this->sound_source->VoiceRelease();
    this->sound_source->Unlock();
}

// ---------------------------------------------------------------------------
// play state
// ---------------------------------------------------------------------------

NuSoundVoice::PlayState NuSoundVoice::GetState() const {
    NuSoundVoice::PlayState state;

    pthread_mutex_lock(&sStateCriticalSection);
    state = this->state;
    pthread_mutex_unlock(&sStateCriticalSection);
    return state;
}

void NuSoundVoice::SetState(PlayState state) {
    pthread_mutex_lock(&sStateCriticalSection);
    this->state = state;
    pthread_mutex_unlock(&sStateCriticalSection);
}

bool NuSoundVoice::GetAutoDelete() const {
    return (this->flags2 & 1) != 0;
}

void NuSoundVoice::SetAutoDelete(bool auto_delete) {
    this->flags2 = (u8)(this->flags2 & 0xfe | auto_delete);
}

void NuSoundVoice::SetMixUpdate(bool mix_update) {
    this->flags = (u8)(this->flags & 0xef | mix_update << 4);
}

void NuSoundVoice::SetVolume(f32 volume) {
    // The original rejects out-of-range values instead of clamping.
    if (0.0f <= volume && volume <= 1.0f) {
        this->volume = volume;
    }
}

void NuSoundVoice::SetPitch(f32 pitch) {
    if (0.0f <= pitch) {
        this->pitch = pitch;
    }
}

// ---------------------------------------------------------------------------
// play control
// ---------------------------------------------------------------------------

void NuSoundVoice::Play() {
    NuSoundVoice::PlayState state = this->GetState();
    if (state == PLAYSTATE_PLAYING) {
        return;
    }
    state = this->GetState();
    if (state == PLAYSTATE_PAUSED) {
        this->Resume();
        return;
    }

    if (this->queued_buffers == 0) {
        // Ask the source for the initial buffers; the streamer (or the sample
        // itself) hands them back through SubmitBuffer.
        u32 num_buffers = this->sound_source->GetNumInitialBuffers();
        for (u32 i = 0; i < num_buffers; i++) {
            if ((this->flags2 & 8) == 0 && (this->flags2 & 2) != 0) {
                break;
            }
            NuSoundWeakPtr<NuSoundBufferCallback> callback;
            callback.Set(this);
            this->sound_source->RequestBuffer((this->flags2 >> 3) & 1, callback);
        }
    }

    this->Update(0.0f);         // prime the mix and the hardware state
    this->StartHardwareVoice(); // flags the device start (applied in UpdateHardwareVoice)
    this->SetState(PLAYSTATE_PLAYING);
}

void NuSoundVoice::Pause() {
    if (this->GetState() == PLAYSTATE_PLAYING) {
        this->PauseHardwareVoice();
        this->SetState(PLAYSTATE_PAUSED);
        this->flags = (u8)(this->flags & 0xf0 | (this->flags + 1) & 0xf);
    }
}

void NuSoundVoice::Resume() {
    u8 flags = this->flags;
    if ((flags & 0xf) != 0) {
        flags = (u8)(flags & 0xf0 | (flags & 0xf) + 0xf & 0xf);
        this->flags = flags;
    }
    if ((flags & 0xf) != 0) {
        return;
    }

    if (this->GetState() == PLAYSTATE_PAUSED) {
        this->Update(0.0f);
        this->ResumeHardwareVoice();
        this->SetState(PLAYSTATE_PLAYING);
    }

    this->flags &= 0xf0;
}

void NuSoundVoice::Stop(bool with_effects) {
    if (this->GetState() == PLAYSTATE_STOPPED) {
        return;
    }

    if (with_effects) {
        if (this->BeginStopEffects()) {
            // Stop effects were started; the voice stops once they finished.
            if (!this->CheckStopEffects()) {
                return;
            }
        }
    }

    this->StopHardwareVoice();
    this->SetState(PLAYSTATE_STOPPED);
}

// ---------------------------------------------------------------------------
// per-frame processing
// ---------------------------------------------------------------------------

void NuSoundVoice::Update(f32 frametime) {
    if ((this->flags2 & 4) != 0 && this->CheckStopEffects()) {
        this->Stop(true);
    }

    this->UpdateEffects(frametime, NuSoundEffect::EffectProcessStage::ZERO);

    if ((this->flags & 0x10) != 0 || this->GetState() == PLAYSTATE_STOPPED) {
        this->UpdateMix(frametime);
    }

    this->UpdateEffects(frametime, NuSoundEffect::EffectProcessStage::ONE);

    this->UpdateHardwareVoice(frametime);
    this->ApplyHardwareVoiceMix();
}

void NuSoundVoice::UpdateMix(f32 frametime) {
    f32 bus_gains[8] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    this->CalculatePositionalMix();

    if (this->output_bus != NULL) {
        this->output_bus->ApplyFinalMix(bus_gains);
    }

    f32 attenuation = this->CalculateEffectAttenuation();
    f32 pitch_scale = this->CalculateEffectPitchScale();
    f32 volume = this->volume;

    for (u32 i = 0; i < 8; i++) {
        this->mix_gains[i] *= bus_gains[i] * attenuation * volume;
    }

    this->field67_0xa8 = bus_gains[2] * attenuation * this->field67_0xa8;
    this->field68_0xac = pitch_scale;
}

void NuSoundVoice::CalculatePositionalMix() {
    if (this->surround_mode == 2) {
        // 2D omni: every channel at full gain except the LFE channel, which
        // keeps the voice's low-frequency mix.
        this->mix_gains[0] = 1.0f;
        this->mix_gains[1] = 1.0f;
        this->mix_gains[2] = 1.0f;
        this->mix_gains[3] = (f32)this->field113_0x10c;
        this->mix_gains[4] = 1.0f;
        this->mix_gains[5] = 1.0f;
        this->mix_gains[6] = 1.0f;
        this->mix_gains[7] = 1.0f;
        this->field67_0xa8 = 1.0f;
        return;
    }

    // The 3D positional paths (surround modes 0/1/3/4 with listener matrices)
    // are not exercised by the title music; left unimplemented on purpose.
}

bool NuSoundVoice::AreStopEffectsRunning() const {
    return (this->flags2 & 4) != 0;
}

bool NuSoundVoice::CheckStopEffects() {
    // Stop effects only exist while the effects list is non-empty.
    return true;
}

void NuSoundVoice::UpdateEffects(f32 frametime, NuSoundEffect::EffectProcessStage stage) {
    (void)frametime;
    (void)stage;
}

void NuSoundVoice::CheckStarvedBuffers() {
}

// ---------------------------------------------------------------------------
// remaining original surface (off the title music path; kept as stubs)
// ---------------------------------------------------------------------------

void NuSoundVoice::AddEffect(NuSoundEffect *) {
}

bool NuSoundVoice::BeginStopEffects() {
    return false;
}

f32 NuSoundVoice::CalculateEffectAttenuation() {
    return 1.0f;
}

f32 NuSoundVoice::CalculateEffectPitchScale() {
    return 1.0f;
}

void NuSoundVoice::CalculateFalloffAttenuation(f32) {
}

void NuSoundVoice::CalculateFieldAngle(f32) {
}

void NuSoundVoice::CalculatePositionalCoefficients(f32 *, VuVec const &, VuMtx const &, f32, f32) {
}

void NuSoundVoice::GetControllerBits() const {
}

void NuSoundVoice::GetDirection() const {
}

void NuSoundVoice::GetDownmixerType() const {
}

void NuSoundVoice::GetEffect(NuSoundEffect::EffectType) {
}

void NuSoundVoice::GetFalloffType() const {
}

void NuSoundVoice::GetFar() const {
}

void NuSoundVoice::GetLowFrequencyMix() const {
}

void NuSoundVoice::GetNear() const {
}

void NuSoundVoice::GetNumEffects() const {
}

void NuSoundVoice::GetOutputBus() const {
}

void NuSoundVoice::GetPenetration() const {
}

void NuSoundVoice::GetPitch() const {
}

void NuSoundVoice::GetPlaybackPositionSeconds() {
}

void NuSoundVoice::GetPosition() const {
}

void NuSoundVoice::GetReverbWetMix() const {
}

void NuSoundVoice::GetRoutingTable() const {
}

void NuSoundVoice::GetSpeakerBleedAngle() const {
}

void NuSoundVoice::GetSpeakerBleedFar() const {
}

void NuSoundVoice::GetSpeakerBleedNear() const {
}

void NuSoundVoice::GetSpeakerFieldAngleMax() const {
}

void NuSoundVoice::GetSpeakerFieldAngleMin() const {
}

void NuSoundVoice::GetStartOffset() const {
}

void NuSoundVoice::GetSurroundMode() const {
}

void NuSoundVoice::GetVelocity() const {
}

void NuSoundVoice::GetVolume() const {
}

void NuSoundVoice::IsLooping() const {
}

void NuSoundVoice::RegisterHandle(NuSoundHandle *) {
}

void NuSoundVoice::RemoveEffect(NuSoundEffect *) {
}

void NuSoundVoice::SetControllerBits(i32) {
}

void NuSoundVoice::SetCustomSurroundMix(f32 *) {
}

void NuSoundVoice::SetDirection(VuVec *) {
}

void NuSoundVoice::SetDownmixerType(NuSoundSystem::DownmixType) {
}

void NuSoundVoice::SetFalloff(f32, f32, NuSoundSystem::FalloffType) {
}

void NuSoundVoice::SetLowFrequencyMix(f32) {
}

void NuSoundVoice::SetOutputBus(NuSoundBus *) {
}

void NuSoundVoice::SetOutputDevices(i32) {
}

void NuSoundVoice::SetPenetration(f32) {
}

void NuSoundVoice::SetPosition(VuVec *) {
}

void NuSoundVoice::SetReverbWetMix(f32 mix) {
    this->field130_0x144 = mix;
}

void NuSoundVoice::SetRoutingTable(NuSoundRoutingTable *) {
}

void NuSoundVoice::SetSpeakerBleedAngle(f32) {
}

void NuSoundVoice::SetSpeakerBleedFar(f32) {
}

void NuSoundVoice::SetSpeakerBleedNear(f32) {
}

void NuSoundVoice::SetSpeakerFieldAngle(f32, f32) {
}

void NuSoundVoice::SetStartOffset(f32) {
}

void NuSoundVoice::SetListeners(NuEList<NuSoundListener, DefaultElist> const *) {
}

void NuSoundVoice::SetSurroundMode(NuSoundSystem::SurroundMode) {
}

void NuSoundVoice::SetVelocity(VuVec const &) {
}

void NuSoundVoice::UnregisterHandle(NuSoundHandle *) {
}
