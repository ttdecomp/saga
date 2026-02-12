#pragma once

#include "decomp.h"
#include "nu2api.saga/nufile/nufile.h"

#include "nu2api.saga/nusound/nusound_buffer.hpp"
#include "nu2api.saga/nusound/nusound_system.hpp"

#include <pthread.h>

class NuSoundLoadTrigger {
  public:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    volatile bool a;
    volatile bool b;

    NuSoundLoadTrigger() {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
        a = false;
        b = false;
    }
    ~NuSoundLoadTrigger() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }
};

class NuSoundLoader {
  private:
    NUFILE file;
    i32 field2_0x8;
    i32 field3_0xc;
    NuSoundStreamDesc *desc;
    NuSoundOutOfMemCallback *oom;
    const char *path;

  public:
    NuSoundLoader();

    i32 CloseStream();
    u64 Deinterleave(byte *data, int length, char **dest, int count, NuSoundSystem::ChannelConfig config);
    i32 LoadFromFile(const char *name, NuSoundStreamDesc *desc, NuSoundBuffer *buffer, NuSoundOutOfMemCallback *oom);
    i32 Load(NuSoundStreamDesc *desc, NuSoundBuffer *buffer);

    // virtual methods
    virtual ~NuSoundLoader();

    virtual NuSoundStreamDesc *CreateHeader() = 0;

    virtual i32 OpenForStreaming(const char *path, f64 param2, NuSoundStreamDesc *desc, bool param4);
    virtual NuSoundBuffer *FillStreamBuffer(NuSoundBuffer *buffer, bool param2);

    virtual bool SeekRawData(u64 position);
    virtual bool SeekPCMSample(u64 sampleIndex) = 0;
    virtual bool SeekTime(f64 seconds) = 0;

    virtual bool OpenFileForStreaming(const char *path, bool unused);
    virtual void Close();

    virtual void *ReadHeader(NuSoundStreamDesc *desc) = 0;
    virtual u64 ReadData(void *buffer, u64 size);
};
