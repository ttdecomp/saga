#pragma once

#include "decomp.h"

#include <pthread.h>

#include "nu2api/nufile/nufile.h"
#include "nu2api/nusound/nusound_buffer.hpp"

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
  protected:
    NUFILE file;
    i32 field2_0x8;
    i32 field3_0xc;
    NuSoundStreamDesc *desc;
    NuSoundOutOfMemCallback *oom;
    const char *path;
    u32 flags;

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
    virtual void FillStreamBuffer(NuSoundBuffer *buffer, bool param2);

    virtual bool SeekRawData(u64 position);
    virtual bool SeekPCMSample(u64 index) = 0;
    virtual bool SeekTime(f64 seconds) = 0;

    virtual i32 OpenFileForStreaming(const char *path, bool unused);
    virtual void Close();

    virtual i32 ReadHeader(NuSoundStreamDesc *desc) = 0;
    virtual u64 ReadData(void *buffer, u64 size);
};

class NuSoundStreamDesc {
  public:
    enum class DataFormat {
        ZERO = 0,
        THREE = 3,
    };

  public:
    u16 format_id;
    u16 num_channels;
    u32 sample_rate;
    u32 samples_per_second;
    u16 bits_per_channel;
    u16 block_size;
    u64 decoded_length_bytes;
    u16 extended_data_size;
    void *extended_data;

    OggVorbis_File ogg_file;

    u64 file_size;
    u64 encoded_length_bytes;
    u64 length_samples;
    double length_seconds;

  public:
    virtual ~NuSoundStreamDesc() {
    }

    virtual DataFormat GetDecodedDataFormat() = 0;
    virtual u64 GetEncodedLengthBytes() = 0;
    virtual u64 GetLengthSamples() = 0;
    virtual double GetLengthSeconds() = 0;
    virtual u64 GetDataOffset() = 0;
    virtual u16 GetNumChannels() = 0;
    virtual u32 GetSampleRate() = 0;
    virtual u16 GetBitsPerChannel() = 0;
    virtual u16 GetBlockSize() = 0;
    virtual DataFormat GetEncodedDataFormat() = 0;
    virtual u64 GetDecodedLengthBytes() = 0;
    virtual i32 DecodeStreamOnOpen();
    virtual i32 GetLoopStart();
    virtual i32 GetLoopEnd();
    virtual u16 GetInterleaveSize() = 0;
    virtual u16 GetFormatId() = 0;
    virtual u16 GetExtendedDataSize() = 0;
    virtual void *GetExtendedData() = 0;
};

enum class LoadState {
    NOT_LOADED = 0,
    LOADED = 1,
    TWO = 2,
};
enum class ErrorState {
    NONE = 0,
};
