#include "nusound_decoder_ogg.hpp"

#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nucore/numemory.h"
#include "nu2api/nusound/nusound_buffer.hpp"
#include "nu2api/nusound/nusound_system.hpp"

#include <new>

#include <string.h>

// libTTapp.so 0x32e630: the OGG decoder extends the base decoder with the
// streaming reader subobject and a 0x2b000-byte ring buffer size.
NuSoundDecoderOGG::NuSoundDecoderOGG(char const *name, NuSoundSource *wrapped) : NuSoundDecoder(name, wrapped) {
    this->decoder_self_list_0xe8 = NULL;
    this->field_0xf8 = 0;
    this->field_0x104 = 0;
    this->field_0x108 = 0;

    new (&this->read_callbacks) OGGReadCallbacksDecoder();

    this->ring_read_pos = 0;
    this->ring_write_pos = 0;
    this->locked_buffer = NULL;
    this->ogg_file = NULL;

    this->read_callbacks.SetDecoder(this);

    this->buffer_size = 0x2b000;
}

NuSoundDecoderOGG::~NuSoundDecoderOGG() {
}

// libTTapp.so 0x32e1a0.
void NuSoundDecoderOGG::Reset() {
}

// libTTapp.so 0x32e1e0.
void NuSoundDecoderOGG::SubmitBuffer(NuSoundBuffer *buffer) {
    (void)buffer;
}

// libTTapp.so 0x32e5f0 / 0x32e620.
NuSoundDecoderOGG::OGGReadCallbacksDecoder::OGGReadCallbacksDecoder() {
    this->decoder = NULL;
    this->position = 0;
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::SetDecoder(NuSoundDecoderOGG *decoder) {
    this->decoder = decoder;
}

// libTTapp.so 0x32e1b0 / 0x32e1c0 / 0x32e1d0.
void NuSoundDecoderOGG::OGGReadCallbacksDecoder::Seek(int origin, unsigned int offset) {
    (void)origin;
    (void)offset;
}

void NuSoundDecoderOGG::OGGReadCallbacksDecoder::Close() {
}

int NuSoundDecoderOGG::OGGReadCallbacksDecoder::GetPosition() const {
    return (int)this->position;
}

// libTTapp.so 0x32ec10: serves the vorbisfile reader with encoded bytes from
// the decoder's streaming ring, blocking on the decode thread when the ring
// runs dry and looping the stream at EOF when requested.
int NuSoundDecoderOGG::OGGReadCallbacksDecoder::Read(void *dest, unsigned int size) {
    (void)dest;
    (void)size;
    return 0;
}

// libTTapp.so 0x32e730: decode the next chunk of the stream into the given
// buffer. Returns the number of decoded bytes written (the buffer context's
// read_size). The original gates the fast path on its compressed staging
// buffer (decoder+0x134), which its decode thread keeps fed because the
// loader-backed datasource only serves a fixed in-memory snapshot; our
// NuSoundLoaderOGG keeps the file open for streaming, so the same vorbis
// handle reads the identical encoded bytes without the staging hop. The
// staging ring lands with NuSoundDecodeThread::RequestDecode/ThreadFunc.
u64 NuSoundDecoderOGG::Decode(NuSoundSource &source, NuSoundBuffer &buffer, bool loop) {
    this->loop_flag = loop;

    if (this->ogg_file == NULL) {
        return 0;
    }

    NuSoundBuffer::Context &context = buffer.GetCurrentContext();
    NuSoundStreamDesc *desc = source.GetStreamDesc();

    if (this->locked_buffer != NULL) {
        this->locked_buffer->Lock();
    }

    buffer.Lock();

    context.flags &= ~0x3u;
    context.read_size = 0;
    context.size2 = 0;

    memset(buffer.GetAddress(), 0, buffer.GetBufferSize());

    char *dest = (char *)buffer.GetAddress();
    u32 buffer_bytes = (u32)buffer.GetBufferSize();

    if (this->decoded_bytes == 0) {
        context.flags |= 1;
    }

    u32 block_size = desc->GetBlockSize();
    u32 rounded = buffer_bytes - (buffer_bytes % block_size);

    u64 total = desc->GetDecodedLengthBytes();

    u32 chunk = rounded;
    if (loop == false) {
        u64 remaining = total - this->decoded_bytes;
        if (remaining < (u64)chunk) {
            chunk = (u32)remaining;
        }
    }

    u32 got = this->DecodeOggChunk(dest, chunk);

    this->total_decoded_bytes += got;
    this->decoded_bytes += chunk;
    context.read_size += got;

    total = desc->GetDecodedLengthBytes();
    if (this->decoded_bytes == total) {
        context.flags |= 2;
        this->decoded_bytes = 0;
    }

    if (this->locked_buffer != NULL) {
        this->locked_buffer->Unlock();
    }

    buffer.Unlock();

    return context.read_size;
}

// libTTapp.so 0x32e310: ov_read() the encoded stream until the destination
// holds `size` bytes (or the stream ends / loops), then apply the original's
// channel permutation passes for 3- and 6-channel material. The vorbis handle
// is the one the loader opened on the NuSoundHeaderOGG, so this reads the
// same compressed stream the original's streaming reader serves.
u32 NuSoundDecoderOGG::DecodeOggChunk(char *dest, unsigned int size) {
    NuSoundStreamDesc *desc = this->GetStreamDesc();

    u32 bits_per_sample = desc->GetBitsPerChannel();

    OggVorbis_File *ogg = this->ogg_file;
    u32 decoded = 0;

    u32 block_size = desc->GetBlockSize();
    u32 bytes_per_sample = (bits_per_sample + 7) / 8;

    if (size != 0 && block_size < size && ogg != NULL) {
        char *cursor = dest;
        char *end = dest + size;

        while (cursor < end) {
            int bitstream = 0;
            int ret = ov_read(ogg, cursor, (int)(end - cursor), 0, bytes_per_sample, 1, &bitstream);

            if (ret <= 0) {
                if (ret < 0) {
                    // libTTapp.so 0x32e598: negative returns (OV_HOLE and
                    // friends) just loop around; the next ov_read recovers.
                    continue;
                }
                if (this->loop_flag) {
                    // Stream finished and the voice wants a loop: restart it.
                    ov_raw_seek(ogg, 0);
                    continue;
                }
                break;
            }

            decoded += ret;
            cursor += ret;
        }
    }

    // Channel permutation passes (Vorbis orders multichannel material
    // differently from WAVE): 3-channel streams rotate the first samples of
    // each frame, 6-channel streams apply the 5.1 reordering.
    u32 channels = desc->GetNumChannels();
    if (channels == 3) {
        for (u32 i = 0; i < decoded / bytes_per_sample; i++) {
            u16 tmp = *(u16 *)&dest[2 + i * 2];
            *(u16 *)&dest[2 + i * 2] = *(u16 *)&dest[4 + i * 2];
            *(u16 *)&dest[4 + i * 2] = tmp;
        }
    } else if (channels == 6) {
        for (u32 i = 0; i < decoded / bytes_per_sample; i++) {
            u16 tmp = *(u16 *)&dest[2 + i * 2];
            *(u16 *)&dest[2 + i * 2] = *(u16 *)&dest[4 + i * 2];
            *(u16 *)&dest[4 + i * 2] = tmp;

            tmp = *(u16 *)&dest[6 + i * 2];
            *(u16 *)&dest[6 + i * 2] = *(u16 *)&dest[10 + i * 2];
            *(u16 *)&dest[10 + i * 2] = tmp;

            tmp = *(u16 *)&dest[8 + i * 2];
            *(u16 *)&dest[8 + i * 2] = *(u16 *)&dest[10 + i * 2];
            *(u16 *)&dest[10 + i * 2] = tmp;
        }
    }

    return decoded;
}
