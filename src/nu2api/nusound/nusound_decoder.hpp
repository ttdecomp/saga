#pragma once

#include "nu2api/nucore/android/NuThread_android.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nusound/nusound_weakptr.hpp"

class NuSoundBufferCallback;

class NuSoundDecodeThread {
    NuThreadSemaphore semaphore;
    NuSoundWeakPtr<NuSoundBufferCallback> loaders[128];
    NuThread *thread;
    i32 field1026_0xe04;
    i32 some_atomic_count;

    static i32 sThreadPriority;

  public:
    NuSoundDecodeThread();

    static void ThreadFunc(void *self_);
};
