#include "nu2api_nusound_types.h"

void SetSoundFadeDist(WORLDINFO_s *, OPTIONSSAVE_s *) {
}

void edanimSoundPlace(i32, nuvec_s *) {
}

void edanimSoundCreate(nuvec_s *) {
}

void GameSetSoundVolume(OPTIONSSAVE_s *) {
}

NuSoundListener *NuSoundSystem::GetNearestRealListener(NuEList<NuSoundListener, DefaultElist> const &, VuVec const &) {
    return NULL;
}

NuSoundListener *NuSoundSystem::GetNearestFocusListener(NuEList<NuSoundListener, DefaultElist> const &, VuVec const &,
                                                        float &) {
    return NULL;
}

void NuSoundLoaderOGG::OGGFileCallbacks::Close() {
}

void NuSoundLoaderOGG::OGGFileCallbacks::GetFile() const {
}

void NuSoundLoaderOGG::OGGFileCallbacks::GetPosition() const {
}

void NuSoundLoaderOGG::OGGFileCallbacks::Seek(i32, u32) {
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::Close() {
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::GetPosition() const {
}

NuSoundDecoderOGG::OGGReadCallbacksDecoder::OGGReadCallbacksDecoder() {
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::Read(void *, u32) {
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::Seek(i32, u32) {
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::SetDecoder(NuSoundDecoderOGG *) {
}
