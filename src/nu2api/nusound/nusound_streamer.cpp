// NuSoundStreamingSample / NuSoundStreamer — decompiled from libTTapp.so
// (nu2api.2013/nusound/nusound_streaming_sample.cpp, nusound_streamer.cpp).

#include "nu2api/nusound/nusound_streamer.hpp"
#include "nu2api/nusound/nusound_voice.hpp"

#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nusound/nusound_loader.hpp"

#include <new>

NuList<NuSoundStreamer *> NuSoundStreamer::sStreamers;
i32 NuSoundStreamer::sThreadPriority = 2;
i32 NuSoundStreamer::sThreadStackSize = 0x8000;
NUTHREAD_CORE NuSoundStreamer::sThreadCoreId = {.value = 0};

// ---------------------------------------------------------------------------
// NuSoundStreamer
// ---------------------------------------------------------------------------

NuSoundStreamer::NuSoundStreamer()
    : queue1_semaphore(32), queue2_semaphore(32), semaphore(32), queue1_length(0), queue1_index(0), queue2_length(0),
      queue2_index(0), running(true) {
    thread =
        NuCore::m_threadManager->CreateThread(ThreadFunc, this, sThreadPriority, "NuSoundStreamThread",
                                              sThreadStackSize, sThreadCoreId.cafe_core, sThreadCoreId.xbox360_core);

    NuListNode<NuSoundStreamer *> *node = NU_ALLOC_T(NuListNode<NuSoundStreamer *>, 1, "", NUMEMORY_CATEGORY_NONE);
    if (node != NULL) {
        new (node) NuListNode<NuSoundStreamer *>(NULL, NULL, this);
    }

    sStreamers.Append(node);
}

NuSoundStreamer::~NuSoundStreamer() {
}

void NuSoundStreamer::RequestCue(NuSoundStreamingSample *streaming_sample, bool loop, f32 start_offset,
                                 bool weak_flag) {
    streaming_sample->AddedToThreadQueue();
    streaming_sample->streamer = this;

    QueueElement *element = &this->queue1[this->queue1_length % 32];
    element->message = QueueElement::Message::OPEN_SAMPLE;
    element->sample = streaming_sample;
    element->loop = loop;
    element->start_offset = start_offset;
    element->buffer = NULL;

    NuSoundWeakPtrListNode::sPtrListLock.Lock();
    if (element->weak_ptr.obj != NULL) {
        element->weak_ptr.obj->Unlink(&element->weak_ptr);
        element->weak_ptr.obj = NULL;
    }
    NuSoundWeakPtrListNode::sPtrListLock.Unlock();

    element->weak_ptr.bool_value = false;

    __sync_fetch_and_add(&this->queue1_length, 1);

    this->semaphore.Signal();
}

void NuSoundStreamer::RequestFill(NuSoundStreamingSample *sample, NuSoundBuffer *buffer, bool loop,
                                  NuSoundWeakPtr<NuSoundBufferCallback> callback) {
    // Fill requests go to the priority queue so the streamer always services
    // them before any pending control request.
    QueueElement *element = &this->queue2[this->queue2_length % 32];
    element->message = QueueElement::Message::FILL_STREAM_BUFFER;
    element->sample = sample;
    element->loop = loop;
    element->start_offset = 0.0f;
    element->buffer = buffer;

    NuSoundWeakPtrListNode::sPtrListLock.Lock();
    NuSoundWeakPtr<NuSoundBufferCallback> local;
    local.obj = callback.obj;
    local.bool_value = false;
    if (element->weak_ptr.obj != local.obj) {
        if (element->weak_ptr.obj != NULL) {
            element->weak_ptr.obj->Unlink(&element->weak_ptr);
        }
        if (local.obj != NULL) {
            local.obj->Link(&element->weak_ptr);
        }
        element->weak_ptr.obj = local.obj;
    }
    NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    element->weak_ptr.bool_value = false;

    __sync_fetch_and_add(&this->queue2_length, 1);

    this->semaphore.Signal();
}

void NuSoundStreamer::RequestClose(NuSoundStreamingSample *sample) {
    sample->AddedToThreadQueue();

    QueueElement *element = &this->queue1[this->queue1_length % 32];
    element->message = QueueElement::Message::CLOSE_SAMPLE;
    element->sample = sample;
    element->loop = false;
    element->start_offset = 0.0f;
    element->buffer = NULL;

    NuSoundWeakPtrListNode::sPtrListLock.Lock();
    if (element->weak_ptr.obj != NULL) {
        element->weak_ptr.obj->Unlink(&element->weak_ptr);
        element->weak_ptr.obj = NULL;
    }
    NuSoundWeakPtrListNode::sPtrListLock.Unlock();

    element->weak_ptr.bool_value = false;

    __sync_fetch_and_add(&this->queue1_length, 1);

    this->semaphore.Signal();
}

void NuSoundStreamer::RequestReCue(NuSoundStreamingSample *sample, bool loop, f32 start_offset) {
    sample->AddedToThreadQueue();

    QueueElement *element = &this->queue1[this->queue1_length % 32];
    element->message = QueueElement::Message::RECUE_SAMPLE;
    element->sample = sample;
    element->loop = loop;
    element->start_offset = start_offset;
    element->buffer = NULL;

    NuSoundWeakPtrListNode::sPtrListLock.Lock();
    if (element->weak_ptr.obj != NULL) {
        element->weak_ptr.obj->Unlink(&element->weak_ptr);
        element->weak_ptr.obj = NULL;
    }
    NuSoundWeakPtrListNode::sPtrListLock.Unlock();

    element->weak_ptr.bool_value = false;

    __sync_fetch_and_add(&this->queue1_length, 1);

    this->semaphore.Signal();
}

void NuSoundStreamer::ShutdownThread() {
    QueueElement *element = &this->queue1[this->queue1_length % 32];
    element->message = QueueElement::Message::SHUTDOWN;
    element->sample = NULL;
    element->loop = false;
    element->start_offset = 0.0f;
    element->buffer = NULL;
    element->weak_ptr.obj = NULL;
    element->weak_ptr.bool_value = false;

    __sync_fetch_and_add(&this->queue1_length, 1);

    this->semaphore.Signal();
}

void NuSoundStreamer::ShutdownAll() {
    for (NuListNodeBase *node = sStreamers.Head(); node != sStreamers.Tail(); node = node->GetNext()) {
        ((NuListNode<NuSoundStreamer *> *)node)->value->ShutdownThread();
    }
}

void NuSoundStreamer::ThreadFunc(void *self) {
    LOG_INFO("NuSoundStreamer::ThreadFunc(self=%p)", self);

    NuSoundStreamer *streamer = (NuSoundStreamer *)self;
    if (streamer->running == false) {
        return;
    }

    do {
        streamer->semaphore.Wait();

        QueueElement *slot;
        bool is_fill;
        if (streamer->queue2_index == streamer->queue2_length) {
            slot = &streamer->queue1[streamer->queue1_index % 32];
            is_fill = false;
        } else {
            slot = &streamer->queue2[streamer->queue2_index % 32];
            is_fill = true;
        }

        // Take a copy of the element; the queue slot releases its weak
        // reference as soon as the element is taken.
        QueueElement element;
        element.message = slot->message;
        element.sample = slot->sample;
        element.loop = slot->loop;
        element.start_offset = slot->start_offset;
        element.buffer = slot->buffer;
        element.weak_ptr.obj = slot->weak_ptr.obj;
        element.weak_ptr.bool_value = slot->weak_ptr.bool_value;
        slot->weak_ptr.obj = NULL;
        slot->weak_ptr.bool_value = false;

        LOG_INFO("NuSoundStreamer::ThreadFunc: processing element %p (message=%d, sample=%p, loop=%d, "
                 "start_offset=%f, buffer=%p, weak_ptr.obj=%p)",
                 &element, (u32)element.message, element.sample, element.loop, element.start_offset, element.buffer,
                 element.weak_ptr.obj);

        switch (element.message) {
            case QueueElement::Message::OPEN_SAMPLE:
                element.sample->Open(element.start_offset, element.loop, element.weak_ptr.bool_value);
                element.sample->RemovedFromThreadQueue();
                break;

            case QueueElement::Message::FILL_STREAM_BUFFER: {
                NuSoundLoader *loader = element.sample->file_loader;
                loader->FillStreamBuffer(element.buffer, element.loop);

                NuSoundBuffer::Context &context = element.buffer->GetCurrentContext();
                if (context.size2 != 0) {
                    NuSoundWeakPtrListNode::sPtrAccessLock.Lock();
                    if (element.weak_ptr.obj != NULL) {
                        ((NuSoundBufferCallback *)element.weak_ptr.obj)->SubmitBuffer(element.buffer);
                    }
                    NuSoundWeakPtrListNode::sPtrAccessLock.Unlock();
                }
                break;
            }

            case QueueElement::Message::CLOSE_SAMPLE:
                element.sample->Close();
                element.sample->RemovedFromThreadQueue();
                break;

            case QueueElement::Message::RECUE_SAMPLE:
                element.sample->ReCue(element.start_offset, element.loop);
                element.sample->RemovedFromThreadQueue();
                break;

            case QueueElement::Message::SHUTDOWN:
                streamer->running = false;
                break;
        }

        if (element.weak_ptr.obj != NULL) {
            NuSoundWeakPtrListNode::sPtrListLock.Lock();
            element.weak_ptr.obj->Unlink(&element.weak_ptr);
            element.weak_ptr.obj = NULL;
            NuSoundWeakPtrListNode::sPtrListLock.Unlock();
        }

        if (is_fill) {
            __sync_fetch_and_add(&streamer->queue2_index, 1);
        } else {
            __sync_fetch_and_add(&streamer->queue1_index, 1);
        }
    } while (streamer->running != false);
}

// ---------------------------------------------------------------------------
// NuSoundStreamingSample
// ---------------------------------------------------------------------------

NuSoundStreamingSample::NuSoundStreamingSample(const char *file)
    : NuSoundSample(file, NuSoundSource::FeedType::STREAMING) {
    this->sound_buffer1 = NULL;
    this->sound_buffer2 = NULL;
    this->streamer = NULL;
    this->file_loader = NULL;
    this->some_count = 0;
    this->field8_0x90 = 0;
    this->field_0x88 = 0;
}

NuSoundStreamingSample::~NuSoundStreamingSample() {
}

i32 NuSoundStreamingSample::Open(f32 start_offset, bool loop, bool weak_flag) {
    if (GetResourceCount() == 0 || GetLoadState() == LoadState::TWO) {
        return 0;
    }

    NuSoundStreamDesc *desc = NULL;

    if (this->sound_buffer1 == NULL) {
        u32 stream_buffer_size = NuSoundSystem::GetStreamBufferSize();

        this->sound_buffer1 = NU_ALLOC_T(NuSoundBuffer, 1, "", NUMEMORY_CATEGORY_NUSOUND);
        if (this->sound_buffer1 != NULL) {
            new (this->sound_buffer1) NuSoundBuffer();
        }

        if (this->sound_buffer1->Allocate(stream_buffer_size / 2, NuSoundSystem::MemoryDiscipline::SAMPLE) != 1) {
            goto alloc_error;
        }

        this->sound_buffer2 = NU_ALLOC_T(NuSoundBuffer, 1, "", NUMEMORY_CATEGORY_NUSOUND);
        if (this->sound_buffer2 != NULL) {
            new (this->sound_buffer2) NuSoundBuffer();
        }

        if (this->sound_buffer2->Allocate(stream_buffer_size / 2, NuSoundSystem::MemoryDiscipline::SAMPLE) != 1) {
            goto alloc_error;
        }

        this->field_0x88 = 1;
    }

    this->file_loader = NuSoundSystem::CreateFileLoader(this->file_type);
    desc = this->file_loader->CreateHeader();
    if (desc == NULL) {
        NuSoundSystem::ReleaseFileLoader(this->file_loader);
        this->file_loader = NULL;
        return 3;
    }

    this->SetStreamDesc(desc);

    if (this->file_loader->OpenForStreaming(this->name, start_offset, desc, weak_flag) != 1) {
        NuSoundSystem::ReleaseFileLoader(this->file_loader);
        this->file_loader = NULL;
        NuSoundSystem::FreeMemory(NuSoundSystem::MemoryDiscipline::SAMPLE, (usize)desc, 0);
        this->SetStreamDesc(NULL);
        this->SetLoadState(LoadState::NOT_LOADED);
        this->SetLastErrorState(ErrorState::NONE);
        return 1;
    }

    {
        NuSoundBuffer::Context context;
        context.flags |= 1; // streaming

        this->file_loader->FillStreamBuffer(this->sound_buffer1, loop);
        context = this->sound_buffer1->GetCurrentContext();

        if (context.size2 != 0) {
            this->some_count++;
        }
        this->sound_buffer1->SetCurrentContext(context);
        u32 first_flags = context.flags;

        if ((first_flags & 2) == 0) {
            this->file_loader->FillStreamBuffer(this->sound_buffer2, loop);
            context = this->sound_buffer2->GetCurrentContext();

            if (context.size2 != 0) {
                this->some_count++;
            } else if (this->some_count == 0) {
                // Nothing decoded at all: the stream is unusable.
                this->file_loader->CloseStream();
                this->sound_buffer2->SetCurrentContext(context);
                this->SetLoadState(LoadState::NOT_LOADED);
                this->SetLastErrorState(ErrorState::NONE);
                return 4;
            }
            this->sound_buffer2->SetCurrentContext(context);
        }
    }

    this->SetLoadState(LoadState::TWO);
    this->SetLastErrorState(ErrorState::NONE);
    return 0;

alloc_error:
    if (this->sound_buffer1 != NULL) {
        if (this->sound_buffer1->IsAllocated()) {
            this->sound_buffer1->Free();
        }

        this->sound_buffer1->~NuSoundBuffer();

        NU_FREE(this->sound_buffer1);

        this->sound_buffer1 = NULL;
    }

    if (this->sound_buffer2 != NULL) {
        if (this->sound_buffer2->IsAllocated()) {
            this->sound_buffer2->Free();
        }

        this->sound_buffer2->~NuSoundBuffer();

        NU_FREE(this->sound_buffer2);

        this->sound_buffer2 = NULL;
    }

    return 3;
}

void NuSoundStreamingSample::Close() {
    if (GetResourceCount() == 0) {
        if (this->file_loader != NULL) {
            this->file_loader->CloseStream();
            NuSoundSystem::ReleaseFileLoader(this->file_loader);
            this->file_loader = NULL;
        }

        NuSoundStreamDesc *desc = this->GetStreamDesc();
        if (desc != NULL) {
            this->file_loader->ReleaseHeader(desc);
            this->SetStreamDesc(NULL);
        }

        this->some_count = 0;
        this->field8_0x90 = 0;
        this->SetLoadState(LoadState::NOT_LOADED);

        if (this->field_0x88 != 0 && this->sound_buffer1 != NULL) {
            this->sound_buffer1->Free();
            this->sound_buffer1->~NuSoundBuffer();
            NU_FREE(this->sound_buffer1);
        }
        this->sound_buffer1 = NULL;

        if (this->field_0x88 != 0 && this->sound_buffer2 != NULL) {
            this->sound_buffer2->Free();
            this->sound_buffer2->~NuSoundBuffer();
            NU_FREE(this->sound_buffer2);
        }
        this->sound_buffer2 = NULL;
    }

    this->SetLastErrorState(ErrorState::NONE);
}

i32 NuSoundStreamingSample::ReCue(f32 start_offset, bool loop) {
    this->some_count = 0;
    this->field8_0x90 = 0;

    this->file_loader->SeekTime(start_offset);

    NuSoundBuffer::Context context;
    context.flags |= 1;

    this->sound_buffer1->Lock();
    this->file_loader->FillStreamBuffer(this->sound_buffer1, loop);
    context = this->sound_buffer1->GetCurrentContext();

    if (context.size2 != 0) {
        this->some_count++;
    }
    this->sound_buffer1->SetCurrentContext(context);
    u32 first_flags = context.flags;
    this->sound_buffer1->Unlock();

    if ((first_flags & 2) == 0) {
        this->sound_buffer2->Lock();
        this->file_loader->FillStreamBuffer(this->sound_buffer2, loop);
        context = this->sound_buffer2->GetCurrentContext();

        if (context.size2 != 0) {
            this->some_count++;
        } else {
            // Ran out of data while re-filling the second buffer.
            this->file_loader->CloseStream();
            this->sound_buffer2->Unlock();
            return 2;
        }
        this->sound_buffer2->SetCurrentContext(context);
        this->sound_buffer2->Unlock();
    }

    return 0;
}

bool NuSoundStreamingSample::IsLocked() const {
    if (this->sound_buffer1 != NULL && this->sound_buffer1->IsLocked()) {
        return true;
    }
    if (this->sound_buffer2 != NULL && this->sound_buffer2->IsLocked()) {
        return true;
    }
    return false;
}

void NuSoundStreamingSample::Lock() {
    this->sound_buffer1->Lock();
    this->sound_buffer2->Lock();
}

void NuSoundStreamingSample::Unlock() {
    this->sound_buffer1->Unlock();
    this->sound_buffer2->Unlock();
}

void NuSoundStreamingSample::RequestBuffer(bool loop, NuSoundWeakPtr<NuSoundBufferCallback> callback) {
    if (this->field8_0x90 < this->some_count) {
        // A buffer already holds decoded data: hand it to the voice directly.
        NuSoundBuffer *buffer = (&this->sound_buffer1)[this->field8_0x90 % 2];
        NuSoundWeakPtrListNode::sPtrAccessLock.Lock();
        if (callback.obj != NULL) {
            ((NuSoundBufferCallback *)callback.obj)->SubmitBuffer(buffer);
        }
        NuSoundWeakPtrListNode::sPtrAccessLock.Unlock();
    } else {
        // The next slot in the ring still has to be filled on the streamer
        // thread; the voice gets it once the fill completes.
        NuSoundBuffer *buffer = (&this->sound_buffer1)[this->some_count % 2];

        NuSoundWeakPtr<NuSoundBufferCallback> local;
        local.obj = callback.obj;
        local.bool_value = callback.bool_value;

        this->streamer->RequestFill(this, buffer, loop, local);
        this->some_count++;
    }

    this->field8_0x90++;
}
