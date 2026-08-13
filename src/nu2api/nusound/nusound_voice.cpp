#include "nu2api_nusound_types.h"

NuSoundVoice::NuSoundVoice(NuSoundSource *sound_source, bool param_2) {
}

NuSoundVoice::~NuSoundVoice() {
}

void NuSoundVoice::AddEffect(NuSoundEffect *) {
}

void NuSoundVoice::AreStopEffectsRunning() const {
}

void NuSoundVoice::BeginStopEffects() {
}

void NuSoundVoice::CalculateEffectAttenuation() {
}

void NuSoundVoice::CalculateEffectPitchScale() {
}

void NuSoundVoice::CalculateFalloffAttenuation(float) {
}

void NuSoundVoice::CalculateFieldAngle(float) {
}

void NuSoundVoice::CalculatePositionalCoefficients(float *, VuVec const &, VuMtx const &, float, float) {
}

void NuSoundVoice::CalculatePositionalMix() {
}

void NuSoundVoice::CheckStopEffects() {
}

void NuSoundVoice::GetAutoDelete() const {
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

void NuSoundVoice::GetState() const {
}

void NuSoundVoice::GetSurroundMode() const {
}

void NuSoundVoice::GetVelocity() const {
}

void NuSoundVoice::GetVolume() const {
}

void NuSoundVoice::IsLooping() const {
}

void NuSoundVoice::Pause() {
}

void NuSoundVoice::Play() {
}

void NuSoundVoice::RegisterHandle(NuSoundHandle *) {
}

void NuSoundVoice::RemoveEffect(NuSoundEffect *) {
}

void NuSoundVoice::Resume() {
}

void NuSoundVoice::SetAutoDelete(bool) {
}

void NuSoundVoice::SetControllerBits(i32) {
}

void NuSoundVoice::SetCustomSurroundMix(float *) {
}

void NuSoundVoice::SetDirection(VuVec *) {
}

void NuSoundVoice::SetDownmixerType(NuSoundSystem::DownmixType) {
}

void NuSoundVoice::SetFalloff(float, float, NuSoundSystem::FalloffType) {
}

void NuSoundVoice::SetLowFrequencyMix(float) {
}

void NuSoundVoice::SetMixUpdate(bool) {
}

void NuSoundVoice::SetOutputBus(NuSoundBus *) {
}

void NuSoundVoice::SetOutputDevices(i32) {
}

void NuSoundVoice::SetPenetration(float) {
}

void NuSoundVoice::SetPitch(float) {
}

void NuSoundVoice::SetPosition(VuVec *) {
}

void NuSoundVoice::SetReverbWetMix(float) {
}

void NuSoundVoice::SetRoutingTable(NuSoundRoutingTable *) {
}

void NuSoundVoice::SetSpeakerBleedAngle(float) {
}

void NuSoundVoice::SetSpeakerBleedFar(float) {
}

void NuSoundVoice::SetSpeakerBleedNear(float) {
}

void NuSoundVoice::SetSpeakerFieldAngle(float, float) {
}

void NuSoundVoice::SetStartOffset(float) {
}

void NuSoundVoice::SetListeners(NuEList<NuSoundListener, DefaultElist> const *) {
}

void NuSoundVoice::SetState(NuSoundVoice::PlayState) {
}

void NuSoundVoice::SetSurroundMode(NuSoundSystem::SurroundMode) {
}

void NuSoundVoice::SetVelocity(VuVec const &) {
}

void NuSoundVoice::SetVolume(float) {
}

void NuSoundVoice::Stop(bool) {
}

void NuSoundVoice::UnregisterHandle(NuSoundHandle *) {
}

void NuSoundVoice::Update(float) {
}

void NuSoundVoice::UpdateEffects(float, NuSoundEffect::EffectProcessStage) {
}

void NuSoundVoice::UpdateMix(float) {
}
