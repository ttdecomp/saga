#include "nu2api/nusound/nusound_streamer.hpp"

#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nusound/nusound_loader.hpp"

#include <new>

NuList<NuSoundStreamer *> NuSoundStreamer::sStreamers;
i32 NuSoundStreamer::sThreadPriority = 2;
i32 NuSoundStreamer::sThreadStackSize = 0x8000;
NUTHREAD_CORE NuSoundStreamer::sThreadCoreId = {.value = 0};

void NuSoundStreamer::RequestCue(NuSoundStreamingSample *streaming_sample, bool loop, float start_offset,
                                 bool always_false) {

    streaming_sample->AddedToThreadQueue();
    streaming_sample->streamer = this;

    NuSoundWeakPtrListNode *element = &queue[queue_length];
    LOG_DEBUG("element=%p", element);
    element->streaming_sample = streaming_sample;

    LOG_WARN("missing lots of stuff here");

    __sync_fetch_and_add(&this->queue_length, 1);
    this->semaphore1.Signal();
}

NuSoundStreamer::NuSoundStreamer() : semaphore1(0x20), semaphore2(0x20), semaphore3(0x20), queue{}, buffer2{0} {
    queue_length = 0;

    thread =
        NuCore::m_threadManager->CreateThread(ThreadFunc, this, sThreadPriority, "NuSoundStreamThread",
                                              sThreadStackSize, sThreadCoreId.cafe_core, sThreadCoreId.xbox360_core);

    NuListNode<NuSoundStreamer *> *node = NU_ALLOC_T(NuListNode<NuSoundStreamer *>, 1, "", NUMEMORY_CATEGORY_NONE);
    if (node != NULL) {
        new (node) NuListNode<NuSoundStreamer *>(NULL, NULL, this);
    }

    sStreamers.Append(node);
}

void NuSoundStreamer::ThreadFunc(void *self_) {
    NuSoundStreamer *self = (NuSoundStreamer *)self_;

    LOG_WARN("NuSoundStreamer::ThreadFunc");
    while (true) {
        self->semaphore1.Wait();
        LOG_INFO("ThreadFunc: semaphore1 signaled, queue_length=%d", self->queue_length);

        NuSoundWeakPtrListNode *element = &self->queue[self->index];

        element->streaming_sample->Open(0.0f, false, false);

        element->streaming_sample->RemovedFromThreadQueue();

        __sync_fetch_and_add(&self->index, 1);
    }
}

NuSoundStreamingSample::NuSoundStreamingSample(const char *file)
    : NuSoundSample(file, NuSoundSource::FeedType::STREAMING) {

    // this->some_count = 0;
    // this->field8_0x90 = 0;
    //(this->parent).parent.vtable = &NuSoundSourceVtable_00679f68;
    // this->sound_streamer = NULL;
    // this->soundLoader = NULL;
    // this->field_0x88 = 0;
    //(this->parent).parent.field9_0x1c = 1;
}

i32 NuSoundStreamingSample::Open(float param_1, bool param_2, bool param_3) {
    if (GetResourceCount() == 0 || GetLoadState() == LoadState::TWO) {
        return 0;
    }

    if (this->buffer1 == NULL) {
        buffer1 = NU_ALLOC_T(NuSoundBuffer, 1, "", NUMEMORY_CATEGORY_NUSOUND);
        if (buffer1 != NULL) {
            new (buffer1) NuSoundBuffer();
        }

        if (buffer1->Allocate(NuSoundSystem::GetStreamBufferSize(), NuSoundSystem::MemoryDiscipline::SAMPLE) != 1) {
            goto alloc_error;
        }
    }

    if (this->buffer2 == NULL) {
        buffer2 = NU_ALLOC_T(NuSoundBuffer, 1, "", NUMEMORY_CATEGORY_NUSOUND);
        if (buffer2 != NULL) {
            new (buffer2) NuSoundBuffer();
        }

        if (buffer2->Allocate(NuSoundSystem::GetStreamBufferSize(), NuSoundSystem::MemoryDiscipline::SAMPLE) != 1) {
            goto alloc_error;
        }
    }

    loader = NuSoundSystem::CreateFileLoader(file_type);
    {
        NuSoundStreamDesc *desc = loader->CreateHeader();

        if (desc == NULL) {
            NuSoundSystem::ReleaseFileLoader(loader);
            loader = NULL;
            return 3;
        }
    }

    return 0;

alloc_error:
    if (buffer1 != NULL) {
        if (buffer1->IsAllocated()) {
            buffer1->Free();
        }

        buffer1->~NuSoundBuffer();

        NU_FREE(buffer1);

        buffer1 = NULL;
    }

    if (buffer2 != NULL) {
        if (buffer2->IsAllocated()) {
            buffer2->Free();
        }

        buffer2->~NuSoundBuffer();

        NU_FREE(buffer2);

        buffer2 = NULL;
    }

    return 3;
}
