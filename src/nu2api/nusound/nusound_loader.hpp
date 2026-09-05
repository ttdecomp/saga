#pragma once

#include "decomp.h"

#include <pthread.h>

#include "nu2api/nufile/nufile.h"
#include "nu2api/nusound/nusound_buffer.hpp"
#include "nu2api/nusound/nusound_streamdesc.hpp"

class NuSoundLoaderWAV;

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

  public:
    NuSoundLoader();

    i32 CloseStream();
    u64 Deinterleave(char *data, int length, char **dest, int count, NuSoundSystem::ChannelConfig config);
    void GetChannelAddress(NuSoundBuffer *, NuSoundStreamDesc *, NuSoundSystem::AudioChannel);
    // libTTapp calls this after clearing its loader pointer.  The routine never
    // reads `this`; exposing that original static-style semantic to sanitizer
    // builds avoids manufacturing a non-null object that the target did not use.
    SAGA_HOST_STATIC void ReleaseHeader(NuSoundStreamDesc *);
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

// 18-byte canonical WAV fmt header; overlays the head of the stream desc
// (ReadRIFFHeaderChunk reads it straight into the desc fields).
struct FileHeaderWAV {
    u16 format;
    u16 num_channels;
    u32 sample_rate;
    u32 byte_rate;
    u16 block_size;
    u16 bits_per_channel;
    u16 extended_size;
};

// Stream desc for WAV / MIB files: raw PCM, encoded and decoded format are
// identical, so these sources never need a decoder.
class NuSoundHeaderWAV : public NuSoundStreamDesc {
  public:
    u16 format_id;
    u16 num_channels;
    u32 sample_rate;
    u32 samples_per_second;
    u16 block_size;
    u16 bits_per_channel;
    u16 extended_data_size;
    u16 extended_data;
    u64 encoded_length_bytes;
    u64 data_position; // absolute file offset of the data chunk

    DataFormat GetDecodedDataFormat() const override {
        return DataFormat::ZERO;
    }
    DataFormat GetEncodedDataFormat() const override {
        return DataFormat::ZERO;
    }
    u64 GetEncodedLengthBytes() const override {
        return this->encoded_length_bytes;
    }
    u64 GetDecodedLengthBytes() const override {
        return this->encoded_length_bytes;
    }
    u64 GetLengthSamples() const override {
        return GetEncodedLengthBytes() / ((GetBitsPerChannel() + 7) / 8);
    }
    f32 GetLengthSeconds() const override {
        return (f32)GetLengthSamples() / (f32)GetSampleRate();
    }
    u64 GetDataOffset() const override {
        return this->data_position;
    }
    u16 GetNumChannels() const override {
        return this->num_channels;
    }
    u32 GetSampleRate() const override {
        return this->sample_rate;
    }
    u16 GetBitsPerChannel() const override {
        return this->bits_per_channel;
    }
    u16 GetBlockSize() const override {
        return this->block_size;
    }
    u16 GetInterleaveSize() const override {
        return 0;
    }
    u16 GetFormatID() const override {
        return this->format_id;
    }
    u16 GetExtendedDataSize() const override {
        return this->extended_data_size;
    }
    void *GetExtendedData() const override {
        return (void *)&this->extended_data;
    }
};

class NuSoundLoaderWAV : public NuSoundLoader {
  public:
    // One RIFF chunk header as read from the file.
    struct ChunkInfo {
        u32 id;
        u32 size;
    };

    // Per-request entry for the RIFF chunk walker: the expected chunk ids
    // (fmt / data), the reader callbacks and the found chunk infos + state.
    struct ChunkReadRequest {
        u32 chunk_id;
        ChunkInfo chunk_info;
        u32 state;
        void (*reader)(i32 file, NuSoundStreamDesc *desc, const ChunkInfo &info, NuSoundLoaderWAV *loader);
    };

    NuSoundLoaderWAV();
    ~NuSoundLoaderWAV();

    NuSoundStreamDesc *CreateHeader() override;
    i32 ReadHeader(NuSoundStreamDesc *desc) override;
    bool SeekPCMSample(u64 index) override;
    bool SeekTime(f64 seconds) override;

    static void ReadDataChunk(i32 file, NuSoundStreamDesc *desc, const ChunkInfo &info, NuSoundLoaderWAV *loader);
    static void ReadRIFFHeaderChunk(i32 file, NuSoundStreamDesc *desc, const ChunkInfo &info, NuSoundLoaderWAV *loader);
    u32 FindChunk(i32 file, u32 id, ChunkInfo &info);
    u32 FindChunks(i32 file, NuSoundStreamDesc *desc, ChunkReadRequest *requests, u32 count);
    static u32 MakeFourCC(char *cc);
    static void EndianFlipWAVHeader(FileHeaderWAV *header);
};
