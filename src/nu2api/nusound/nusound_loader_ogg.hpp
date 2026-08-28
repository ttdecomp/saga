#pragma once

#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nusound/nusound_loader.hpp"

class NuSoundHeaderOGG : public NuSoundStreamDesc {
  public:
    NuSoundHeaderOGG() = default;

    DataFormat GetDecodedDataFormat() const;
    u64 GetEncodedLengthBytes() const;
    u64 GetLengthSamples() const;
    double GetLengthSeconds() const;
    u64 GetDataOffset() const;
    u16 GetNumChannels() const;
    u32 GetSampleRate() const;
    u16 GetBitsPerChannel() const;
    u16 GetBlockSize() const;
    DataFormat GetEncodedDataFormat() const;
    u64 GetDecodedLengthBytes() const;
    u16 GetInterleaveSize() const;
    u16 GetFormatID() const;
    u16 GetExtendedDataSize() const;
    void *GetExtendedData() const;
};

class NuSoundLoaderOGG : public NuSoundLoader {
  public:
    class OGGFileCallbacks {
      private:
        NUFILE file;

      public:
        i32 Read(void *dest, u32 size);
        void Seek(i32 origin, u32 offset);
        void Close();
        void SetFile(NUFILE file);
        NUFILE GetFile() const;
        i32 GetPosition() const;
    };

    NuSoundLoaderOGG();

    NuSoundStreamDesc::DataFormat GetDecodedDataFormat();
    NuSoundStreamDesc *CreateHeader();

    bool SeekPCMSample(u64 index);
    bool SeekTime(f64 seconds);
    virtual i32 ReadHeader(NuSoundStreamDesc *header);
    i32 OpenFileForStreaming(const char *path, bool flag);
    bool SeekRawData(u64 position);
    void Close();

    static i32 OggCallbackClose(void *callbacks);
    static i32 OggCallbackSeek(void *callbacks, i64 offset, i32 origin);
    static i32 OggCallbackTell(void *callbacks);

  private:
    OGGFileCallbacks file_callbacks;
    void *buffer;

    void FreeMemoryBuffer();

    static usize OggCallbackRead(void *dest, usize count, usize size, void *callbacks);
};
