// Method bodies for the NuSound classes declared in nu2api_nusound_types.h.
// The NuSoundDecoderOGG decode path is off the title-music pipeline; its
// bodies are faithful stubs pending the decode-thread work.
#include "nu2api_nusound_types.h"

NuSoundDecoderOGG::NuSoundDecoderOGG(char const *, NuSoundSource *) {
}

NuSoundDecoderOGG::~NuSoundDecoderOGG() {
}

void NuSoundDecoderOGG::Decode(NuSoundSource &, NuSoundBuffer &, bool) {
}

void NuSoundDecoderOGG::DecodeOggChunk(char *, u32) {
}

void NuSoundDecoderOGG::Reset() {
}

void NuSoundDecoderOGG::SubmitBuffer(NuSoundBuffer *) {
}

NuSoundDecoderOGG::OGGReadCallbacksDecoder::OGGReadCallbacksDecoder() {
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::Read(void *, u32) {
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::Seek(i32, u32) {
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::Close() {
}

i32 NuSoundDecoderOGG::OGGReadCallbacksDecoder::GetPosition() const {
    return 0;
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::SetDecoder(NuSoundDecoderOGG *) {
}
