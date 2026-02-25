#include "nu2api/nusound/nusound_streamer.hpp"

#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nusound/nusound_loader.hpp"

void NuSoundStreamer::ThreadFunc(void *self_) {
    NuSoundStreamer *self = (NuSoundStreamer *)self_;

    LOG_DEBUG("NuSoundStreamer::ThreadFunc");

    while (self->running) {
        self->semaphore.Wait();

        QueueElement element;
        if (self->queue2_index == self->queue2_length) {
            QueueElement element1 = self->queue1[self->queue1_index % 32];
            element = element1;
            __sync_fetch_and_add(&self->queue1_index, 1);
        } else {
            QueueElement element2 = self->queue2[self->queue2_index % 32];
            element = element2;
            __sync_fetch_and_add(&self->queue2_index, 1);
        }

        switch (element.type) {
            case QueueElement::Type::OPEN_SAMPLE:
                LOG_INFO("NuSoundStreamer::ThreadFunc: OPEN_SAMPLE");
                element.sample->Open(element.start_offset, element.loop, element.weak_ptr.bool_value);
                element.sample->RemovedFromThreadQueue();
                break;
            default:
                LOG_WARN("NuSoundStreamer::ThreadFunc: Unhandled queue element type %d", (u32)element.type);
                UNIMPLEMENTED();
                break;
        }
    }
}
