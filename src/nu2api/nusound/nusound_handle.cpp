#include "nu2api_nusound_types.h"

void NuSoundHandle::AddEffect(NuSoundEffect *) {
}

void NuSoundHandle::GetEffect(NuSoundEffect::EffectType) {
}

void NuSoundHandle::GetFalloffType() const {
}

void NuSoundHandle::GetFar() const {
}

void NuSoundHandle::GetLastAttenuation() const {
}

void NuSoundHandle::GetLastAttenuationListener() const {
}

void NuSoundHandle::GetLastDistanceAttenuation() const {
}

void NuSoundHandle::GetLastListenerDistance() const {
}

void NuSoundHandle::GetLastPositionalListener() const {
}

void NuSoundHandle::GetNear() const {
}

void NuSoundHandle::GetPitch() const {
}

void NuSoundHandle::GetPlaybackPositionSamples() {
}

void NuSoundHandle::GetPlaybackPositionSeconds() const {
}

void NuSoundHandle::GetPosition() const {
}

void NuSoundHandle::GetState() const {
}

void NuSoundHandle::GetSurroundMode() const {
}

void NuSoundHandle::GetTotalLengthSamples() const {
}

void NuSoundHandle::GetTotalLengthSeconds() const {
}

void NuSoundHandle::GetVelocity() const {
}

void NuSoundHandle::GetVoice() const {
}

void NuSoundHandle::GetVolume() const {
}

void NuSoundHandle::InvalidateVoice() {
}

void NuSoundHandle::IsLooping() const {
}

NuSoundHandle::NuSoundHandle() {
}

NuSoundHandle::NuSoundHandle(NuSoundHandle &) {
}

void NuSoundHandle::Pause() {
}

void NuSoundHandle::Play() {
}

void NuSoundHandle::RemoveEffect(NuSoundEffect *) {
}

void NuSoundHandle::ResetFrameCount() {
}

void NuSoundHandle::Resume() {
}

void NuSoundHandle::SetFalloff(float, float, NuSoundSystem::FalloffType) {
}

void NuSoundHandle::SetPitch(float) {
}

void NuSoundHandle::SetPosition(VuVec *) {
}

void NuSoundHandle::SetVelocity(VuVec const &) {
}

void NuSoundHandle::SetVoice(NuSoundVoice *) {
}

void NuSoundHandle::SetVolume(float) {
}

void NuSoundHandle::Stop() {
}

void NuSoundHandle::operator=(NuSoundHandle &) {
}

void NuSoundHandle::operator==(NuSoundHandle const &) {
}

NuSoundHandle::~NuSoundHandle() {
}
