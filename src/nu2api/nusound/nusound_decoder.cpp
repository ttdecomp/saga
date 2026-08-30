#include "nusound_decoder.hpp"

#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nusound/nusound_buffer.hpp"
#include "nu2api/nusound/nusound_sample.hpp"
#include "nu2api/nusound/nusound_voice.hpp"
#include "nu2api/nusound/nusound_system.hpp"

#include <new>

// libTTapp.so NuSoundDecoder::sDecodeThread @0x11e90d0.
NuSoundDecodeThread *NuSoundDecoder::sDecodeThread = NULL;

// libTTapp.so 0x31eed0: the decoder wraps the source, carries two ring
// buffer objects and starts closed. The source's stream descriptor (the
// loaded NuSoundHeaderOGG) is shared with the decoder so the decode path can
// ov_read() through the handle the loader opened.
NuSoundDecoder::NuSoundDecoder(char const *name, NuSoundSource *wrapped)
    : NuSoundSource(NULL, SourceType::STREAMING, NuSoundSource::FeedType::STREAMING) {
    (void)name;
    this->source = wrapped;
    this->buffers[0] = new NuSoundBuffer();
    this->buffers[1] = new NuSoundBuffer();

    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->cond, NULL);

    // The decode-sync pair the decode thread raises per completed request
    // (device offsets +0xdc/+0xe0/+0xe4).
    pthread_mutex_init(&this->decode_mutex, NULL);
    pthread_cond_init(&this->decode_cond, NULL);
    this->decode_done = false;

    this->consumed_pos = 0;
    this->ring_count = 0;
    this->decode_pos = 0;
    this->buffers_started = 0;
    this->buffer_size = 0x2000;
    this->stream_open = false;
    this->field_0xd4 = 0;
    this->locked_flag = false;
    this->loop_flag = false;
    this->field_0xc0 = 0;
    this->field_0xc4 = 0;
    this->field_0xc8 = 0;
    this->field_0xcc = 0;
    this->total_decoded_bytes = 0;
    this->decoded_bytes = 0;

    this->SetStreamDesc(wrapped->GetStreamDesc());
}

// libTTapp.so 0x31ede0 / 0x31ee70.
NuSoundDecoder::~NuSoundDecoder() {
    pthread_mutex_destroy(&this->mutex);
    pthread_cond_destroy(&this->cond);
    pthread_mutex_destroy(&this->decode_mutex);
    pthread_cond_destroy(&this->decode_cond);
    delete this->buffers[0];
    delete this->buffers[1];
}

// libTTapp.so 0x31ec90: fill the ring buffers upfront. Each buffer is
// allocated at the decoder's buffer size and decoded via the virtual Decode;
// the loop stops after the first two buffers or once the whole stream has
// been decoded (the remaining chunks decode on demand through the decode
// thread as the voice consumes the ring).
bool NuSoundDecoder::OpenStream(bool loop) {
    u64 total = 0;
    u64 decoded = 0;
    this->source->OpenStream(loop);

    NuSoundStreamDesc *desc = this->source->GetStreamDesc();
    this->ring_count = 0;
    this->decode_pos = 0;
    this->consumed_pos = 0;

    if (desc != NULL) {
        total = desc->GetDecodedLengthBytes();
    }

    for (u32 i = 0; decoded < total; i++) {
        NuSoundBuffer &buffer = *this->buffers[i];

        if (buffer.Allocate(this->buffer_size, NuSoundSystem::MemoryDiscipline::DECODER) != 1) {
            this->source->CloseStream();
            return false;
        }

        u64 got = this->Decode(*this->source, buffer, loop);
        decoded += got;
        this->ring_count++;
        this->decode_pos++;
        this->buffers_started++;

        if (i >= 1) {
            break;
        }
    }

    this->stream_open = true;
    return true;
}

// libTTapp.so 0x31e9c0.
bool NuSoundDecoder::IsStreamOpen() const {
    return this->stream_open;
}

// libTTapp.so 0x31ebb0.
void NuSoundDecoder::CloseStream() {
    this->stream_open = false;
    this->ring_count = 0;
    this->decode_pos = 0;
    this->consumed_pos = 0;
}

// libTTapp.so 0x31f0e0: allocate the singleton decode thread.
void NuSoundDecoder::Initialise() {
    // 0xe1c is the device-side object size; the host NuThreadSemaphore and
    // weakptr types differ in size, so size the block from the real object.
    NuSoundDecodeThread *thread = (NuSoundDecodeThread *)NuSoundSystem::_AllocMemory(
        NuSoundSystem::MemoryDiscipline::SCRATCH, (u32)sizeof(NuSoundDecodeThread), 4,
        "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/android/nusound_decoder.cpp:69");

    if (thread != NULL) {
        new (thread) NuSoundDecodeThread();
    }

    NuSoundDecoder::sDecodeThread = thread;
}

// libTTapp.so 0x31fe00.
void NuSoundDecoder::Shutdown() {
}

bool NuSoundDecoder::IsLocked() const {
    return this->locked_flag;
}

// libTTapp.so 0x31eb50: the decoder lock is what keeps the ring buffers'
// device addresses valid. The voice holds it for its whole lifetime (the
// NuSoundVoice ctor calls the source's Lock slot), so every ring buffer
// stays locked at count >= 1 and SubmitBuffer always hands the device a
// live address while the voice exists.
void NuSoundDecoder::Lock() {
    pthread_mutex_lock(&this->mutex);
    this->locked_flag = true;

    for (u32 i = 0; i < this->ring_count; i++) {
        this->buffers[i]->Lock();
    }
}

// libTTapp.so 0x31eaf0.
void NuSoundDecoder::Unlock() {
    for (u32 i = 0; i < this->ring_count; i++) {
        this->buffers[i]->Unlock();
    }

    pthread_mutex_unlock(&this->mutex);
    this->locked_flag = false;
}

// libTTapp.so 0x31eab0 / 0x31ea40: voice bookkeeping on the wrapped source.
void NuSoundDecoder::VoiceReference() {
    if (this->source != NULL) {
        this->source->VoiceReference();
    }
}

void NuSoundDecoder::VoiceRelease() {
    if (this->source != NULL) {
        this->source->VoiceRelease();
    }
}

// libTTapp.so 0x31ea10: min(ring buffers, sNumInitialBuffersByType[type]).
// Every source's type field is 1 (the original's NuSoundSource ctor hardcodes
// it), so the table's second entry, 2, is the cap.
u32 NuSoundDecoder::GetNumInitialBuffers() const {
    static const u32 sNumInitialBuffersByType[] = {1, 2};

    const u32 type = (u32)this->source_type;
    const u32 cap = sNumInitialBuffersByType[type];
    return this->ring_count < cap ? this->ring_count : cap;
}

// libTTapp.so 0x31f000.
unsigned int NuSoundDecoder::GetNumRingBuffers() const {
    return this->ring_count;
}

// libTTapp.so 0x31fe80.
u32 NuSoundDecoder::GetMaxBufferSize() {
    return this->buffer_size;
}

// libTTapp.so 0x31fa90: hand the voice the next ring buffer. When a decoded
// buffer is ahead of the consume cursor and still holds data, it is handed to
// the caller's SubmitBuffer callback under the sample critical section and
// the consume cursor advances. Otherwise the request goes to the decode
// thread (RequestDecode) and returns immediately; the thread decodes the
// buffer and performs the same SubmitBuffer hand-off on its own stack.
void NuSoundDecoder::RequestBuffer(bool loop, NuSoundWeakPtr<NuSoundBufferCallback> callback) {
    if (this->decode_pos > this->consumed_pos && this->ring_count > 0) {
        NuSoundBuffer &ready = *this->buffers[this->consumed_pos % this->ring_count];
        NuSoundBuffer::Context &context = ready.GetCurrentContext();

        if ((context.read_size | context.size2) != 0) {
            NuSoundBufferCallback *cb = (NuSoundBufferCallback *)callback.obj;
            if (cb != NULL) {
                pthread_mutex_lock(&NuSoundSample::sCriticalSection);
                cb->SubmitBuffer(&ready);
                pthread_mutex_unlock(&NuSoundSample::sCriticalSection);
            }
        }

        this->consumed_pos++;
        return;
    }

    if (this->source == NULL || !this->stream_open) {
        return;
    }

    // libTTapp.so 0x31fb08: queue buffers[decode_pos % ring_count] on the
    // decode thread and return; the thread performs the decode and the
    // SubmitBuffer hand-off. The ring slot was already allocated by the
    // OpenStream prefill, and the consume cursor advances on the caller side.
    if (NuSoundDecoder::sDecodeThread != NULL && this->ring_count > 0) {
        NuSoundBuffer &buffer = *this->buffers[this->decode_pos % this->ring_count];
        NuSoundWeakPtr<NuSoundBufferCallback> request;
        request.Set((NuSoundBufferCallback *)callback.obj);
        NuSoundDecoder::sDecodeThread->RequestDecode(*this, buffer, request, (loop & 1) != 0);
        this->decode_pos++;
    }

    this->consumed_pos++;
}

// ---------------------------------------------------------------------------
// the decode thread
// ---------------------------------------------------------------------------

// libTTapp.so 0x31f010: 128 zeroed loader slots behind a 128-signal
// semaphore, then the worker thread (priority 2, cafe/xbox core 2).
NuSoundDecodeThread::NuSoundDecodeThread() : semaphore(128), loaders(), thread(NULL), tail_index(0), head_index(0) {
    NuSoundDecodeThread *self = this;
    (void)self;
    this->thread = NuCore::m_threadManager->CreateThread(NuSoundDecodeThread::ThreadFunc, this, 2, "NuSoundDecode", 0,
                                                         NUTHREADCAFECORE_UNKNOWN_1, NUTHREADXBOX360CORE_UNKNOWN_1);
}

NuSoundDecodeThread::~NuSoundDecodeThread() {
    this->Shutdown();
}

// libTTapp.so 0x31f670: bump the decoder's pending-request count, park
// {decoder, buffer, callback, loop} in the next loader slot and signal the
// worker. The callback weakptr is copied, which re-links its list node into
// the callback's weak list so the worker can re-validate the callback.
void NuSoundDecodeThread::RequestDecode(NuSoundDecoder &decoder, NuSoundBuffer &buffer,
                                        NuSoundWeakPtr<NuSoundBufferCallback> callback, bool loop) {
    decoder.field_0xd4++;

    Loader &entry = this->loaders[this->tail_index % 128];
    entry.decoder = &decoder;
    entry.buffer = &buffer;
    entry.callback.Set((NuSoundBufferCallback *)callback.obj);
    entry.loop = loop;
    this->tail_index++;

    this->semaphore.Signal();
}

// libTTapp.so 0x31f190: pop one loader entry, re-validate the callback
// weakptr, decode the buffer through the decoder's virtual Decode and hand
// it to the callback's SubmitBuffer under the sample critical section.
void NuSoundDecodeThread::ThreadFunc(void *self_) {
    NuSoundDecodeThread *self = (NuSoundDecodeThread *)self_;

    while (true) {
        self->semaphore.Wait();

        Loader &entry = self->loaders[self->head_index % 128];
        self->head_index++;

        NuSoundDecoder *decoder = entry.decoder;
        NuSoundBuffer *buffer = entry.buffer;
        NuSoundBufferCallback *callback = (NuSoundBufferCallback *)entry.callback.obj;
        bool loop = entry.loop;

        // libTTapp.so 0x31f4d3: a closed decoder drops the request
        // (pending count--) without decoding.
        if (decoder == NULL || !decoder->IsStreamOpen()) {
            if (decoder != NULL) {
                decoder->field_0xd4--;
            }
            continue;
        }

        // The worker takes the buffer's data lock for the decode and keeps
        // it held after the SubmitBuffer hand-off (libTTapp.so 0x31f190 has
        // no Unlock on this path; the ring slot stays allocated so the next
        // RequestBuffer reuses the same pool block).
        buffer->Lock();
        decoder->Decode(*decoder, *buffer, loop);
        decoder->buffers_started++;

        if (callback != NULL) {
            pthread_mutex_lock(&NuSoundSample::sCriticalSection);
            callback->SubmitBuffer(buffer);
            pthread_mutex_unlock(&NuSoundSample::sCriticalSection);
        }

        // libTTapp.so 0x31f1c8: the request is complete; drop the pending
        // count (a locked atomic sub on device) and raise the decode-done
        // flag through the decoder's decode-sync pair (mutex +0xdc,
        // cond +0xe0, done flag +0xe4). This never touches the decoder's
        // lifetime lock mutex, which the owning voice holds.
        decoder->field_0xd4--;
        pthread_mutex_lock(&decoder->decode_mutex);
        if (decoder->decode_done == false) {
            decoder->decode_done = true;
            pthread_cond_signal(&decoder->decode_cond);
        }
        pthread_mutex_unlock(&decoder->decode_mutex);
    }
}

// libTTapp.so 0x31fc90.
void NuSoundDecodeThread::Shutdown() {
}
