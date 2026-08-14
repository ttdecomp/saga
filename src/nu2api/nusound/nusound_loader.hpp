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
    u64 Deinterleave(char *data, int length, char **dest, int count, NuSoundSystem::ChannelConfig config);
    void GetChannelAddress(NuSoundBuffer *, NuSoundStreamDesc *, NuSoundSystem::AudioChannel);
    void ReleaseHeader(NuSoundStreamDesc *);
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
    virtual DataFormat GetDecodedDataFormat() const = 0;
    virtual u64 GetEncodedLengthBytes() const = 0;
    virtual u64 GetLengthSamples() const = 0;
    virtual double GetLengthSeconds() const = 0;
    virtual u64 GetDataOffset() const = 0;
    virtual u16 GetNumChannels() const = 0;
    virtual u32 GetSampleRate() const = 0;
    virtual u16 GetBitsPerChannel() const = 0;
    virtual u16 GetBlockSize() const = 0;
    virtual DataFormat GetEncodedDataFormat() const = 0;
    virtual u64 GetDecodedLengthBytes() const = 0;
    virtual i32 DecodeStreamOnOpen() const;
    virtual i32 GetLoopStart() const;
    virtual i32 GetLoopEnd() const;
    virtual u16 GetInterleaveSize() const = 0;
    virtual u16 GetFormatID() const = 0;
    virtual u16 GetExtendedDataSize() const = 0;
    virtual void *GetExtendedData() const = 0;
};
