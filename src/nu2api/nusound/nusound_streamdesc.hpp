#pragma once

// NuSoundStreamDesc — decompiled from libTTapp.so
// (nu2api.2013/nusound/nusound_stream_desc.h). Common header description for
// a loadable / streamable sound file: format, rates, lengths and the encoded
// data position. Concrete loaders (WAV, OGG) subclass it.

#include "nu2api/nucore/common.h"

#include <vorbis/vorbisfile.h>

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
