// Method bodies for the Ghidra-reconstructed NuSound class model declared in
// nu2api_nusound_types.h. The NuSoundLoaderOGG and NuSoundDecoderOGG classes
// defined there differ from the hand-written real headers (their virtual
// signatures do not line up), so their empty placeholder bodies live here.
#include "nu2api_nusound_types.h"

void NuSoundLoaderOGG::Close() {
}

void NuSoundLoaderOGG::OggCallbackClose(void *) {
}

void NuSoundLoaderOGG::OggCallbackSeek(void *, i64, i32) {
}

void NuSoundLoaderOGG::OggCallbackTell(void *) {
}

void NuSoundLoaderOGG::OpenFileForStreaming(char const *, bool) {
}

void NuSoundLoaderOGG::SeekRawData(u64) {
}

void NuSoundDecoderOGG::Decode(NuSoundSource &, NuSoundBuffer &, bool) {
}

void NuSoundDecoderOGG::DecodeOggChunk(char *, u32) {
}

NuSoundDecoderOGG::NuSoundDecoderOGG(char const *, NuSoundSource *) {
}

void NuSoundDecoderOGG::Reset() {
}

void NuSoundDecoderOGG::SubmitBuffer(NuSoundBuffer *) {
}

NuSoundDecoderOGG::~NuSoundDecoderOGG() {
}
