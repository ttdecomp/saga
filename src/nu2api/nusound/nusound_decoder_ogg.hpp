#pragma once

#include "nusound_decoder.hpp"

#include <vorbis/vorbisfile.h>

// The streaming OGG decoder. Transcribed from libTTapp.so 0x32e630 (ctor),
// 0x32e730 (Decode), 0x32e310 (DecodeOggChunk). The decode path ov_read()s
// through the still-open OggVorbis_File of the loaded NuSoundHeaderOGG and
// fills the requested buffer with decoded PCM, swapping channel pairs for
// 3- and 6-channel material exactly like the original.
class NuSoundDecoderOGG : public NuSoundDecoder {
  public:
    // Streaming datasource the decoder reads the encoded stream through.
    class OGGReadCallbacksDecoder {
      public:
        OGGReadCallbacksDecoder();

        void SetDecoder(NuSoundDecoderOGG *decoder);
        virtual void Seek(int origin, unsigned int offset);
        virtual void Close();
        virtual int GetPosition() const;
        virtual int Read(void *dest, unsigned int size);

      private:
        NuSoundDecoderOGG *decoder;
        unsigned int position;
    };

    NuSoundDecoderOGG(char const *name, NuSoundSource *source);
    ~NuSoundDecoderOGG();

    void Reset();
    void SubmitBuffer(NuSoundBuffer *buffer);
    u64 Decode(NuSoundSource &source, NuSoundBuffer &buffer, bool loop) override;

    u32 DecodeOggChunk(char *dest, unsigned int size);

    OGGReadCallbacksDecoder read_callbacks;
    NuSoundBuffer *locked_buffer; // ring buffer the streaming reader is serving

  public:
    OggVorbis_File *ogg_file; // the header's still-open vorbis handle

  private:
  public:
    // Original decoder fields kept under their .so offsets for reference.
    void *decoder_self_list_0xe8;
    u32 field_0xf8;
    u32 field_0x104;
    u32 field_0x108;
    u32 ring_read_pos;  // 0x12c
    u32 ring_write_pos; // 0x130
};
