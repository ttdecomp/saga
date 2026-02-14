#include "nu2api.saga/nusound/nusound_streamer.hpp"

#include "nu2api.saga/nucore/nucore.hpp"

#include <new>

NuList<NuSoundStreamer *> NuSoundStreamer::sStreamers;
i32 NuSoundStreamer::sThreadPriority = 2;
i32 NuSoundStreamer::sThreadStackSize = 0x8000;
NUTHREAD_CORE NuSoundStreamer::sThreadCoreId = {.value = 0};

void NuSoundStreamer::RequestCue(NuSoundStreamingSample *streaming_sample, bool loop, float start_offset,
                                 bool always_false) {

    streaming_sample->AddedToThreadQueue();
    // TODO
    UNIMPLEMENTED();
}

NuSoundStreamer::NuSoundStreamer() : semaphore1(0x20), semaphore2(0x20), semaphore3(0x20), buffer1{0}, buffer2{0} {
    thread =
        NuCore::m_threadManager->CreateThread(ThreadFunc, this, sThreadPriority, "NuSoundStreamThread",
                                              sThreadStackSize, sThreadCoreId.cafe_core, sThreadCoreId.xbox360_core);

    NuListNode<NuSoundStreamer *> *node = NU_ALLOC_T(NuListNode<NuSoundStreamer *>, 1, "", NUMEMORY_CATEGORY_NONE);
    if (node != NULL) {
        new (node) NuListNode<NuSoundStreamer *>(NULL, NULL, this);
    }

    sStreamers.Append(node);
}

void NuSoundStreamer::ThreadFunc(void *self) {
    UNIMPLEMENTED("NuSoundStreamer::ThreadFunc");
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
