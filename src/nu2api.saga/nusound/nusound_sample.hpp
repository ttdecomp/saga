#pragma once

#include "nu2api.saga/nusound/nusound_system.hpp"

#include <pthread.h>

class NuSoundSample {
  private:
    LoadState load_state;
    ErrorState last_error;
    NuSoundSystem::FileType type;
    i32 thread_queue_count;
    i32 ref_count;

    static pthread_mutex_t sCriticalSection;

  protected:
    void AddedToThreadQueue();

  public:
    LoadState GetLoadState() const;
    ErrorState GetLastErrorState() const;
    i32 GetThreadQueueCount() const;
    u32 GetResourceCount();

    void Reference();
    void Release();

    ErrorState Load(void *param_1, int param_2, NuSoundOutOfMemCallback *oomCallback);
};
