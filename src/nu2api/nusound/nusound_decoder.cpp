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
