#pragma once

// NuSoundStreamDesc — decompiled from libTTapp.so
// (nu2api.2013/nusound/nusound_stream_desc.h). Common header description for
// a loadable / streamable sound file: format, rates, lengths and the encoded
// data position. Concrete loaders (WAV, OGG) subclass it.

#include "nu2api/nucore/common.h"

class NuSoundStreamDesc {
  public:
    enum class DataFormat {
        ZERO = 0,
        THREE = 3,
    };

  public:
    virtual ~NuSoundStreamDesc() {
    }
    virtual DataFormat GetDecodedDataFormat() const = 0;
    virtual u64 GetEncodedLengthBytes() const = 0;
    virtual u64 GetLengthSamples() const = 0;
    virtual f32 GetLengthSeconds() const = 0;
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
