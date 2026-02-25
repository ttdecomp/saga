#pragma once

#include "nu2api/nucore/android/NuThread_android.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nusound/nulist.hpp"
#include "nu2api/nusound/nusound_sample.hpp"
#include "nu2api/nusound/nusound_weakptr.hpp"

#include "nu2api/nucore/nuvuvec.hpp"

class NuSoundStreamer;
class NuSoundBufferCallback;

class NuSoundStreamingSample : public NuSoundSample {
  public:
    NuSoundBuffer *buffer1;
    NuSoundBuffer *buffer2;

    NuSoundStreamer *streamer;
    NuSoundLoader *loader;

    NuSoundStreamingSample(const char *file);

    i32 Open(float param_1, bool param_2, bool param_3);
};

class NuSoundStreamer {
  public:
    struct QueueElement {
        enum class Type : u32 {
            OPEN_SAMPLE = 0,
        } type;

        NuSoundStreamingSample *sample;
        bool loop;
        float start_offset;
        int field7_0x10;

        NuSoundWeakPtr<NuSoundBufferCallback> weak_ptr;

        QueueElement() = default;

        QueueElement(NuSoundStreamingSample *sample, bool loop, float start_offset)
            : type(Type::OPEN_SAMPLE), sample(sample), loop(loop), start_offset(start_offset) {
        }

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

    QueueElement queue1[32];
    i32 queue1_length;
    i32 queue1_index;
    NuThreadSemaphore queue1_semaphore;

    QueueElement queue2[32];
    i32 queue2_length;
    i32 queue2_index;
    NuThreadSemaphore queue2_semaphore;

    NuThreadSemaphore semaphore;

  public:
    NuSoundStreamer();

    void RequestCue(NuSoundStreamingSample *streaming_sample, bool loop, float start_offset, bool always_false);

  private:
    static void ThreadFunc(void *self);
};
