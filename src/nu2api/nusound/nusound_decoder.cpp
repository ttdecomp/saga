#include "nusound_decoder.hpp"

#include "nu2api/nucore/nucore.hpp"

i32 NuSoundDecodeThread::sThreadPriority = 2;

NuSoundDecodeThread::NuSoundDecodeThread() : semaphore(128), loaders() {
    this->field1026_0xe04 = 0;
    this->some_atomic_count = 0;
    this->thread = NuCore::m_threadManager->CreateThread(ThreadFunc, this, sThreadPriority, "NuSoundDecodeThread", 0,
                                                         NUTHREADCAFECORE_UNKNOWN_2, NUTHREADXBOX360CORE_UNKNOWN_2);
}

void NuSoundDecodeThread::ThreadFunc(void *self_) {
    NuSoundDecodeThread *self = (NuSoundDecodeThread *)self_;
}

NuSoundDecodeThread::~NuSoundDecodeThread() {
}

void NuSoundDecodeThread::Shutdown() {
}

void NuSoundDecodeThread::RequestDecode(NuSoundDecoder &, NuSoundBuffer &, NuSoundWeakPtr<NuSoundBufferCallback>, bool) {
}

NuSoundDecoder::NuSoundDecoder(char const *, NuSoundSource *) {
}

NuSoundDecoder::~NuSoundDecoder() {
}

void NuSoundDecoder::CloseStream() {
}

void NuSoundDecoder::Initialise() {
}

bool NuSoundDecoder::IsLocked() const {
    return false;
}

bool NuSoundDecoder::IsStreamOpen() const {
    return false;
}

void NuSoundDecoder::Lock() {
}

void NuSoundDecoder::OpenStream(bool) {
}

void NuSoundDecoder::Shutdown() {
}

void NuSoundDecoder::Unlock() {
}

void NuSoundDecoder::VoiceReference() {
}

void NuSoundDecoder::VoiceRelease() {
}

u32 NuSoundDecoder::GetNumInitialBuffers() const {
    return 0;
}

u32 NuSoundDecoder::GetNumRingBuffers() const {
    return 0;
}

void NuSoundDecoder::RequestBuffer(bool, NuSoundWeakPtr<NuSoundBufferCallback>) {
}
