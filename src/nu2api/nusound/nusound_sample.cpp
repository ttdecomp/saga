#include "nu2api/nusound/nusound_sample.hpp"
#include "nu2api/nusound/nusound_voice.hpp"

#include "nu2api/nusound/nusound_loader.hpp"

pthread_mutex_t NuSoundSample::sCriticalSection = PTHREAD_MUTEX_INITIALIZER;

NuSoundSample::NuSoundSample(const char *path, FeedType feed_type)
    : NuSoundSource(path, SourceType::ZERO, feed_type), buffer{} {
    this->field2_0x24 = 0;
    this->field1_0x20 = 0;
    this->file_type = NuSoundSystem::DetermineFileType(path);
    this->load_state = LoadState::NOT_LOADED;
    this->last_error = ErrorState::NONE;
    this->next = NULL;
    this->thread_queue_count = 0;
    this->ref_count = 0;
}

NuSoundSample::LoadState NuSoundSample::GetLoadState() const {
    pthread_mutex_lock(&sCriticalSection);
    LoadState ls = this->load_state;
    pthread_mutex_unlock(&sCriticalSection);

    return ls;
}

NuSoundSample::ErrorState NuSoundSample::GetLastErrorState() const {
    pthread_mutex_lock(&sCriticalSection);
    ErrorState es = this->last_error;
    pthread_mutex_unlock(&sCriticalSection);

    return es;
}

i32 NuSoundSample::GetThreadQueueCount() const {
    return this->thread_queue_count;
}

void NuSoundSample::Reference() {
    __sync_fetch_and_add(&ref_count, 1);
}

void NuSoundSample::Release() {
    __sync_fetch_and_sub(&ref_count, 1);
}

NuSoundSample::ErrorState NuSoundSample::Load(void *param_1, i32 param_2, NuSoundOutOfMemCallback *oomCallback) {
    (void)param_1;
    (void)param_2;

    ErrorState error = ErrorState::NONE;
    if (GetLoadState() != LoadState::LOADED) {
        NuSoundLoader *loader = NuSoundSystem::CreateFileLoader(this->file_type);
        NuSoundStreamDesc *desc = loader->CreateHeader();

        if (desc == NULL) {
            error = ErrorState::UNSUPPORTED;
            NuSoundSystem::ReleaseFileLoader(loader);
        } else {
            SetStreamDesc(desc);
            NuSoundBuffer *source_buffer = static_cast<NuSoundBuffer *>(GetSourceBuffer());
            i32 result = loader->LoadFromFile(this->name, desc, source_buffer, oomCallback);
            NuSoundSystem::ReleaseFileLoader(loader);

            LoadState load_state = LoadState::LOADED;
            if (result != 1) {
                error = ErrorState::FILE_NOT_FOUND;
                if (result >= 2 && result <= 5) {
                    static const ErrorState errors[4] = {ErrorState::FILE_NOT_FOUND, ErrorState::OUT_OF_MEMORY,
                                                         ErrorState::OUT_OF_MEMORY, ErrorState::UNSUPPORTED};
                    error = errors[result - 2];
                }
                NuSoundSystem::FreeMemory(NuSoundSystem::MemoryDiscipline::SCRATCH, reinterpret_cast<usize>(desc), 0);
                SetStreamDesc(NULL);
                load_state = LoadState::NOT_LOADED;
            }

            SetLoadState(load_state);
            SetLastErrorState(error);
        }
    }

    return error;
}

u32 NuSoundSample::GetResourceCount() {
    return this->ref_count;
}

void NuSoundSample::AddedToThreadQueue() {
    __sync_fetch_and_add(&thread_queue_count, 1);
}

void NuSoundSample::RemovedFromThreadQueue() {
    __sync_fetch_and_sub(&thread_queue_count, 1);
}

void NuSoundSample::SetLoadState(LoadState state) {
    pthread_mutex_lock(&sCriticalSection);
    this->load_state = state;
    pthread_mutex_unlock(&sCriticalSection);
}

NuSoundSample::~NuSoundSample() {
}

void NuSoundSample::SetLastErrorState(ErrorState state) {
    pthread_mutex_lock(&sCriticalSection);
    this->last_error = state;
    pthread_mutex_unlock(&sCriticalSection);
}

void *NuSoundSample::GetSourceBuffer() {
    return &this->buffer;
}

bool NuSoundSample::IsLocked() const {
    return this->buffer.IsLocked();
}

void NuSoundSample::Lock() {
    this->buffer.Lock();
}

void NuSoundSample::Unlock() {
    this->buffer.Unlock();
}

i32 NuSoundSample::Unload() {
    if (this->buffer.IsAllocated()) {
        this->buffer.Free();
    }

    if (this->stream_desc != NULL) {
        this->stream_desc->~NuSoundStreamDesc();
        NuSoundSystem::FreeMemory(NuSoundSystem::MemoryDiscipline::SCRATCH, reinterpret_cast<usize>(this->stream_desc),
                                  0);
        SetStreamDesc(NULL);
    }

    SetLoadState(LoadState::NOT_LOADED);
    SetLastErrorState(ErrorState::NONE);
    return 1;
}

void NuSoundSample::RequestBuffer(bool loop, NuSoundWeakPtr<NuSoundBufferCallback> callback) {
    // In-memory samples hand their whole buffer to the requesting voice
    // immediately; the streaming sample overrides this with the double-buffer
    // pump.
    pthread_mutex_lock(&NuSoundWeakPtrListNode::sPtrAccessLock.mutex);
    if (callback.obj != NULL) {
        ((NuSoundBufferCallback *)callback.obj)->SubmitBuffer(&this->buffer);
    }
    pthread_mutex_unlock(&NuSoundWeakPtrListNode::sPtrAccessLock.mutex);
}

bool NuSoundSample::IsStreamOpen() const {
    return GetLoadState() == LoadState::LOADED;
}
