#pragma once

#include "nu2api.saga/nucore/nuthread.h"
#include "nu2api.saga/nusound/nulist.hpp"
#include "nu2api.saga/nusound/nusound_sample.hpp"

#include "nu2api.saga/nucore/nuvuvec.hpp"

class NuSoundStreamingSample : public NuSoundSample {
  public:
    NuSoundBuffer *buffer1;
    NuSoundBuffer *buffer2;

    NuSoundLoader *loader;

    NuSoundStreamingSample(const char *file);
};

class NuSoundStreamer {
  public:
    static NuList<NuSoundStreamer *> sStreamers;

    static i32 sThreadPriority;
    static i32 sThreadStackSize;
    static NUTHREAD_CORE sThreadCoreId;

  private:
    NuThread *thread;

    u8 buffer1[0x500];
    u8 buffer2[0x500];

    NuThreadSemaphore semaphore1;
    NuThreadSemaphore semaphore2;
    NuThreadSemaphore semaphore3;

  public:
    NuSoundStreamer();

    void RequestCue(NuSoundStreamingSample *streaming_sample, bool loop, float start_offset, bool always_false);

  private:
    static void ThreadFunc(void *self);
};
