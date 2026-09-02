#pragma once

// NuSoundStreamingSample / NuSoundStreamer — decompiled from libTTapp.so
// (nu2api.2013/nusound/nusound_streaming_sample.cpp, nusound_streamer.cpp).
//
// The streamer owns one worker thread. Requests are queued as fixed-size
// elements: message 0 = open, 1 = fill (the priority queue), 2 = close,
// 3 = re-cue, 4 = shutdown. The thread waits on a semaphore, always drains
// the fill queue first, then the control queue.

#include "nu2api/nucore/android/NuThread_android.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nusound/nulist.hpp"
#include "nu2api/nusound/nusound_sample.hpp"
#include "nu2api/nusound/nusound_weakptr.hpp"

#include "nu2api/nucore/nuvuvec.hpp"

class NuSoundStreamer;
class NuSoundBufferCallback;
class NuSoundLoader;

// Streaming variant of NuSoundSample (0x9c bytes in the original): the two
// ring buffers live in the system's stream buffer pool and are handed to the
// voice round-robin while the loader keeps them filled on the streamer thread.
class NuSoundStreamingSample : public NuSoundSample {
  public:
    NuSoundBuffer *sound_buffer1;
    NuSoundBuffer *sound_buffer2;

    u8 field_0x88; // set once this sample allocated its own buffers

    // Number of buffers that hold valid data (incremented by Open/ReCue and
    // by every completed fill).
    i32 some_count;
    // Read cursor: how many of the filled buffers have been handed out.
    i32 field8_0x90;

    NuSoundStreamer *streamer;
    NuSoundLoader *file_loader;

    NuSoundStreamingSample(const char *file);

    i32 Open(f32 start_offset, bool loop, bool weak_flag);
    void Close();
    i32 ReCue(f32 start_offset, bool loop);

    bool IsLocked() const override;
    void Lock();
    void Unlock();

    void RequestBuffer(bool loop, NuSoundWeakPtr<NuSoundBufferCallback> callback) override;

    ~NuSoundStreamingSample();
};

class NuSoundStreamer {
  public:
    struct QueueElement {
        enum class Message : u32 {
            OPEN_SAMPLE = 0,
            FILL_STREAM_BUFFER = 1,
            CLOSE_SAMPLE = 2,
            RECUE_SAMPLE = 3,
            SHUTDOWN = 4,
        } message;

        NuSoundStreamingSample *sample;
        bool loop;
        f32 start_offset;
        NuSoundBuffer *buffer;

        NuSoundWeakPtr<NuSoundBufferCallback> weak_ptr;
        bool weak_flag;

        QueueElement() = default;

        ~QueueElement() {
            NuSoundWeakPtrListNode::sPtrListLock.Lock();
            if (this->weak_ptr.obj != NULL) {
                this->weak_ptr.obj->Unlink(&this->weak_ptr);
                this->weak_ptr.obj = NULL;
            }
            NuSoundWeakPtrListNode::sPtrListLock.Unlock();
        }
    };

  public:
    static NuList<NuSoundStreamer *> sStreamers;

    static i32 sThreadPriority;
    static i32 sThreadStackSize;
    static NUTHREAD_CORE sThreadCoreId;

  private:
    NuThread *thread;
    bool running;

    QueueElement queue1[32]; // control queue (open / close / recue / shutdown)
    i32 queue1_length;
    i32 queue1_index;
    NuThreadSemaphore queue1_semaphore;

    QueueElement queue2[32]; // fill queue — always drained first
    i32 queue2_length;
    i32 queue2_index;
    NuThreadSemaphore queue2_semaphore;

    NuThreadSemaphore semaphore;

  public:
    NuSoundStreamer();

    void RequestCue(NuSoundStreamingSample *streaming_sample, bool loop, f32 start_offset, bool weak_flag);
    void RequestFill(NuSoundStreamingSample *sample, NuSoundBuffer *buffer, bool loop,
                     NuSoundWeakPtr<NuSoundBufferCallback> callback);
    void RequestClose(NuSoundStreamingSample *sample);
    void RequestReCue(NuSoundStreamingSample *sample, bool loop, f32 start_offset);
    void ShutdownAll();
    void ShutdownThread();

    ~NuSoundStreamer();

  private:
    static void ThreadFunc(void *self);
};

DECOMP_ASSERT(sizeof(NuSoundStreamer::QueueElement) == 0x28, "NuSoundStreamer queue element size");
