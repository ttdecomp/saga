#pragma once

#include "nusound_decoder.hpp"
#include "nusound_loader_ogg.hpp"
#include "nusound_voice.hpp"

#include <vorbis/vorbisfile.h>

// The streaming OGG decoder. Transcribed from libTTapp.so 0x32e630 (ctor),
// 0x32e730 (Decode), 0x32e310 (DecodeOggChunk). The decode path ov_read()s
// through the still-open OggVorbis_File of the loaded NuSoundHeaderOGG and
// fills the requested buffer with decoded PCM, swapping channel pairs for
// 3- and 6-channel material exactly like the original.
class NuSoundDecoderOGG : public NuSoundDecoder, public NuSoundBufferCallback {
  public:
    // Streaming datasource the decoder reads the encoded stream through.
    class OGGReadCallbacksDecoder {
      public:
        OGGReadCallbacksDecoder();

        void SetDecoder(NuSoundDecoderOGG *decoder);
        virtual int Read(void *dest, unsigned int size);
        virtual void Seek(int origin, unsigned int offset);
        virtual void Close();
        virtual int GetPosition() const;

      private:
        NuSoundDecoderOGG *decoder;
        unsigned int position;
    };

    NuSoundDecoderOGG(char const *name, NuSoundSource *source);
    ~NuSoundDecoderOGG();

    void Reset();
    void SubmitBuffer(NuSoundBuffer *buffer) override;
    u64 Decode(NuSoundSource &source, NuSoundBuffer &buffer, bool loop) override;

    u32 DecodeOggChunk(char *dest, unsigned int size);

    u32 field_0xec;
    void *field_0xf0;
    void *field_0xf4;
    u32 field_0xf8;
    void *field_0xfc;
    void *field_0x100;
    u32 field_0x104;
    u32 field_0x108;
    OGGReadCallbacksDecoder read_callbacks;
    u32 field_0x118;
    NuSoundBuffer *encoded_buffers[4];
    u32 ring_read_pos;            // 0x12c
    u32 ring_write_pos;           // 0x130
    NuSoundBuffer *locked_buffer; // 0x134
    bool ogg_loop;                // 0x138
};
