#pragma once

#include "nu2api/nucore/android/NuThread_android.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nusound/nusound_source.hpp"
#include "nu2api/nusound/nusound_weakptr.hpp"

class NuSoundBufferCallback;

class NuSoundSource;

class NuSoundBuffer;

// The decoder IS a sound source: it wraps the original source, decodes its
// encoded data into ring buffers and feeds voices from the decode thread.
class NuSoundDecoder : public NuSoundSource {
  public:
    NuSoundDecoder(char const *, NuSoundSource *);
    virtual ~NuSoundDecoder();

    void CloseStream();
    void Initialise();
    bool IsLocked() const;
    bool IsStreamOpen() const override;
    void Lock();
    void OpenStream(bool);
    void Shutdown();
    void Unlock();
    void VoiceReference() override;
    void VoiceRelease() override;
    u32 GetNumInitialBuffers() const override;
    unsigned int GetNumRingBuffers() const;
    void RequestBuffer(bool, NuSoundWeakPtr<NuSoundBufferCallback>) override;
};

class NuSoundDecodeThread {
    NuThreadSemaphore semaphore;
    NuSoundWeakPtr<NuSoundBufferCallback> loaders[128];
    NuThread *thread;
    i32 field1026_0xe04;
    i32 some_atomic_count;

    static i32 sThreadPriority;

  public:
    NuSoundDecodeThread();
    ~NuSoundDecodeThread();

    static void ThreadFunc(void *self_);
    void Shutdown();
    void RequestDecode(NuSoundDecoder &, NuSoundBuffer &, NuSoundWeakPtr<NuSoundBufferCallback>, bool);
};
