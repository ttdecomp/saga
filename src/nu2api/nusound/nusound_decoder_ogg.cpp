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
    this->field_0xec = 0;
    this->field_0xf0 = &this->field_0xf0;
    this->field_0xf4 = (u8 *)this + 0xe8;
    this->field_0xf8 = 0;
    this->field_0xfc = (u8 *)this + 0xe8;
    this->field_0x100 = &this->field_0xf0;
    this->field_0x104 = 0;
    this->field_0x108 = 0;

    new (&this->read_callbacks) OGGReadCallbacksDecoder();

    this->ring_read_pos = 0;
    this->ring_write_pos = 0;
    for (u32 i = 0; i < 4; i++) {
        this->encoded_buffers[i] = NULL;
    }
    this->locked_buffer = NULL;
    this->ogg_loop = false;

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
    this->encoded_buffers[this->ring_write_pos % 4] = buffer;
    __sync_fetch_and_add(&this->ring_write_pos, 1);
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
    memset(dest, 0, size);
    if (this->decoder == NULL || size == 0) {
        return 0;
    }

    u8 *out = (u8 *)dest;
    u32 remaining = size;
    u32 copied = 0;

    while (remaining != 0 && this->decoder->locked_buffer != NULL) {
        NuSoundBuffer *buffer = this->decoder->locked_buffer;
        buffer->Lock();
        NuSoundBuffer::Context &context = buffer->GetCurrentContext();

        u64 boundary = context.size3 != 0 ? context.size3 : context.read_size;
        u32 available = boundary > this->position ? (u32)(boundary - this->position) : 0;
        u32 take = remaining < available ? remaining : available;
        if (take != 0) {
            memmove(out, (u8 *)buffer->GetAddress() + this->position, take);
            this->position += take;
            out += take;
            remaining -= take;
            copied += take;
        }

        bool at_end = (context.flags & 2) != 0;
        buffer->Unlock();

        if (remaining == 0 || (at_end && !this->decoder->ogg_loop)) {
            break;
        }

        if (available == take) {
            // Decode() keeps the current encoded buffer locked while
            // vorbisfile reads it.  At a buffer boundary the original drops
            // that persistent lock before moving to the next ring entry.
            buffer->Unlock();

            if (this->decoder->ring_read_pos == this->decoder->ring_write_pos) {
                // A refill is still in flight.  The original re-cues the
                // current buffer from its start and restores the persistent
                // lock, returning whatever was copied this call rather than
                // reporting a false end-of-stream to vorbisfile.
                buffer->Lock();
                this->position = 0;
                return (int)copied;
            }
            this->decoder->locked_buffer = this->decoder->encoded_buffers[this->decoder->ring_read_pos % 4];
            __sync_fetch_and_add(&this->decoder->ring_read_pos, 1);

            NuSoundWeakPtr<NuSoundBufferCallback> callback;
            callback.Set(this->decoder);
            this->decoder->source->RequestBuffer(this->decoder->ogg_loop, callback);

            this->decoder->locked_buffer->Lock();
            this->position = 0;
        }
    }

    return (int)copied;
}

// libTTapp.so 0x32e730: decode the next chunk of the stream into the given
// buffer. Returns the number of decoded bytes written (the buffer context's
// read_size). Encoded buffers arrive through the four-entry ring at +0x11c;
// the callback reader at +0x10c exposes that ring to vorbisfile.
u64 NuSoundDecoderOGG::Decode(NuSoundSource &source, NuSoundBuffer &buffer, bool loop) {
    this->ogg_loop = loop;

    if (this->locked_buffer == NULL) {
        u32 initial = source.GetNumInitialBuffers();
        for (u32 i = 0; i < initial; i++) {
            NuSoundWeakPtr<NuSoundBufferCallback> callback;
            callback.Set(this);
            source.RequestBuffer(loop, callback);
        }
        if (this->ring_read_pos != this->ring_write_pos) {
            this->locked_buffer = this->encoded_buffers[this->ring_read_pos % 4];
            this->ring_read_pos++;
        }
    }

    NuSoundStreamDesc *desc = source.GetStreamDesc();
    if (this->locked_buffer == NULL || desc == NULL) {
        return 0;
    }
    NuSoundBuffer::Context &context = buffer.GetCurrentContext();

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
// channel permutation passes for 3- and 6-channel material. As in the
// original, the header's vorbis handle temporarily uses the decoder callback
// object as its datasource while this chunk is decoded.
u32 NuSoundDecoderOGG::DecodeOggChunk(char *dest, unsigned int size) {
    NuSoundStreamDesc *desc = this->GetStreamDesc();

    u32 bits_per_sample = desc->GetBitsPerChannel();

    NuSoundHeaderOGG *header = (NuSoundHeaderOGG *)desc;
    OggVorbis_File *ogg = header != NULL ? &header->ogg_file : NULL;
    void *saved_datasource = ogg != NULL ? ogg->datasource : NULL;
    if (ogg != NULL) {
        ogg->datasource = &this->read_callbacks;
    }
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
                if (this->ogg_loop) {
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

    if (ogg != NULL) {
        ogg->datasource = saved_datasource;
    }
    return decoded;
}
