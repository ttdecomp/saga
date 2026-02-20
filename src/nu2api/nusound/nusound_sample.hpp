#pragma once

#include "nu2api/nusound/nusound_buffer.hpp"
#include "nu2api/nusound/nusound_source.hpp"

#include <pthread.h>

class NuSoundSample : public NuSoundSource {
  protected:
    NuSoundBuffer buffer;
    NuSoundSystem::FileType file_type;

    i32 field1_0x20;
    i32 field2_0x24;

  public:
    NuSoundSample *next;

  private:
    LoadState load_state;
    ErrorState last_error;
    i32 thread_queue_count;
    i32 ref_count;

    static pthread_mutex_t sCriticalSection;

  public:
    NuSoundSample(const char *path, FeedType feed_type);

    void AddedToThreadQueue();
    void RemovedFromThreadQueue();

    LoadState GetLoadState() const;
    ErrorState GetLastErrorState() const;
    i32 GetThreadQueueCount() const;
    u32 GetResourceCount();

    void Reference();
    void Release();

    ErrorState Load(void *param_1, int param_2, NuSoundOutOfMemCallback *oomCallback);
};
