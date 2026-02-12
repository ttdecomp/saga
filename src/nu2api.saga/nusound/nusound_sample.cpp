#include "nu2api.saga/nusound/nusound_sample.hpp"

#include "nu2api.saga/nusound/nusound_loader.hpp"

pthread_mutex_t NuSoundSample::sCriticalSection = PTHREAD_MUTEX_INITIALIZER;

LoadState NuSoundSample::GetLoadState() const {
    pthread_mutex_lock(&sCriticalSection);
    LoadState ls = this->load_state;
    pthread_mutex_unlock(&sCriticalSection);

    return ls;
}

ErrorState NuSoundSample::GetLastErrorState() const {
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

ErrorState NuSoundSample::Load(void *param_1, i32 param_2, NuSoundOutOfMemCallback *oomCallback) {
    NuSoundLoader *loader;
    NuSoundStreamDesc *desc;
    NuSoundBuffer *buffer;
    LoadState LVar2;

    ErrorState EVar3 = ERRORSTATE_NONE;
    if (GetLoadState() != LOADSTATE_LOADED) {
        loader = NuSoundSystem::CreateFileLoader(this->type);

        // desc = (NuSoundStreamDesc *)(*(code *)loader->vtable[2])(loader);
        desc = loader->CreateHeader();

        /*if (desc == NULL) {
            EVar3 = 3;
            NuSoundSystem::ReleaseFileLoader(loader);
        } else {
            NuSoundSource::SetStreamDesc((NuSoundSource *)this, desc);
            buffer = (NuSoundBuffer *)GetSourceBuffer(this);
            iVar1 = NuSoundLoader::LoadFromFile(loader, this->path, desc, buffer, oomCallback);
            NuSoundSystem::ReleaseFileLoader(loader);
            LVar2 = 1;
            if (iVar1 != 1) {
                EVar3 = 1;
                if (iVar1 - 2U < 4) {
                    EVar3 = *(ErrorState *)(CSWTCH.185 + (iVar1 - 2U) * 4);
                }
                NuSoundSystem::FreeMemory(0, (uint)desc, 0);
                NuSoundSource::SetStreamDesc((NuSoundSource *)this, (NuSoundStreamDesc *)0x0);
                LVar2 = 0;
            }

            SetLoadState(this, LVar2);
            SetLastErrorState(this, EVar3);
        }
            */
    }

    return EVar3;
}

u32 NuSoundSample::GetResourceCount() {
    return this->ref_count;
}

void NuSoundSample::AddedToThreadQueue() {
    __sync_fetch_and_add(&thread_queue_count, 1);
}
