#pragma once

#include "nu2api/nusound/nusound_buffer.hpp"
#include "nu2api/nusound/nusound_loader.hpp"
#include "nu2api/nusound/nusound_source.hpp"
#include "nu2api/nusound/nusound_weakptr.hpp"

#include <pthread.h>

class NuSoundBufferCallback;

class NuSoundSample : public NuSoundSource {
  public:
    enum class LoadState {
        NOT_LOADED = 0,
        LOADED = 1,
        TWO = 2,
    };
    enum class ErrorState {
        NONE = 0,
    };

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

    void SetLoadState(LoadState state);
    void SetLastErrorState(ErrorState state);

    void *GetSourceBuffer();
    virtual bool IsLocked() const;
    void Lock();
    void Unlock();
    void Unload();

    // NuSoundSource overrides (the original dispatched through the source
    // vtable; Play() calls RequestBuffer to obtain its initial buffers).
    bool IsStreamOpen() const override;
    u32 GetMaxBufferSize() const override {
        return 0;
    }
    void RequestBuffer(bool loop, NuSoundWeakPtr<NuSoundBufferCallback> callback) override;

    ~NuSoundSample();

    ErrorState Load(void *param_1, int param_2, NuSoundOutOfMemCallback *oomCallback);
};
