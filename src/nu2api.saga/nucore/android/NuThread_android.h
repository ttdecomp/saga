#pragma once

#include <pthread.h>

#include "nu2api.saga/nucore/common.h"

class NuThreadSemaphore {
  public:
    NuThreadSemaphore(i32 max_signals);
    ~NuThreadSemaphore();

  public:
    void Signal();
    i32 TryWait();
    void Wait();

  private:
    pthread_mutex_t mutex;
    pthread_cond_t condition;

    i32 signaled_count;
    i32 max_signals;
};
