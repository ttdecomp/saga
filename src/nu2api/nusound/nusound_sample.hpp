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
        STREAM_READY = 2,
    };
    enum class ErrorState {
        NONE = 0,
        FILE_NOT_FOUND = 1,
        OUT_OF_MEMORY = 2,
        UNSUPPORTED = 3,
    };

  protected:
    i32 field1_0x20;
    i32 field2_0x24;
    NuSoundBuffer buffer;
    NuSoundSystem::FileType file_type;

  public:
    NuSoundSample *next;

  private:
    LoadState load_state;
    ErrorState last_error;
    i32 thread_queue_count;
    i32 ref_count;

  public:
    // The original's global sample lock: the decoder's RequestBuffer hands
    // ring buffers to voice callbacks under it (libTTapp.so -0x18e4).
    static pthread_mutex_t sCriticalSection;

  public:
    NuSoundSample(const char *path, FeedType feed_type);

    void AddedToThreadQueue();
    void RemovedFromThreadQueue();

    LoadState GetLoadState() const;
    ErrorState GetLastErrorState() const;
    i32 GetThreadQueueCount() const;
    i32 GetResourceCount();

    void Reference();
    void Release();

    void SetLoadState(LoadState state);
    void SetLastErrorState(ErrorState state);

    void *GetSourceBuffer();
    virtual bool IsLocked() const;
    void Lock();
    void Unlock();
    i32 Unload();

    // NuSoundSource overrides (the original dispatched through the source
    // vtable; Play() calls RequestBuffer to obtain its initial buffers).
    bool IsStreamOpen() const override;
    u32 GetMaxBufferSize() override {
        return 0;
    }
    void RequestBuffer(bool loop, NuSoundWeakPtr<NuSoundBufferCallback> callback) override;

    ~NuSoundSample();

    ErrorState Load(void *param_1, int param_2, NuSoundOutOfMemCallback *oomCallback);
};
