#include "nusound_loader_ogg.hpp"

#include <new>

#include <vorbis/vorbisfile.h>

#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/numemory.h"

namespace {
    struct OGGCallbacksVTable {
        i32 (*read)(void *, void *, u32);
        void (*seek)(void *, i32, u32);
        void (*close)(void *);
        i32 (*get_position)(const void *);
    };
} // namespace

i32 NuSoundLoaderOGG::OGGFileCallbacks::Read(void *dest, u32 size) {
    return NuFileRead(file, dest, size);
}

void NuSoundLoaderOGG::OGGFileCallbacks::SetFile(NUFILE file) {
    this->file = file;
}

NuSoundLoaderOGG::NuSoundLoaderOGG() : buffer(NULL) {
}

NuSoundStreamDesc *NuSoundLoaderOGG::CreateHeader() {
    NuSoundHeaderOGG *header = (NuSoundHeaderOGG *)NuSoundSystem::_AllocMemory(
        NuSoundSystem::MemoryDiscipline::SCRATCH, sizeof(NuSoundHeaderOGG), 4,
        "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound_loader_ogg.cpp:280");

    if (header != NULL) {
        new (header) NuSoundHeaderOGG;
    }

    return header;
}

bool NuSoundLoaderOGG::SeekPCMSample(u64 index) {
    NuIOS_IsLowEndDevice();

    NuSoundHeaderOGG *header = (NuSoundHeaderOGG *)desc;
    i32 ret = ov_pcm_seek(&header->ogg_file, index);
    return ret == 0;
}

bool NuSoundLoaderOGG::SeekTime(f64 seconds) {
    NuIOS_IsLowEndDevice();
    NuSoundHeaderOGG *header = (NuSoundHeaderOGG *)desc;
    i32 iVar2 = ov_time_seek(&header->ogg_file, seconds);
    return iVar2 == 0;
}

void NuSoundLoaderOGG::OGGFileCallbacks::Seek(i32 origin, u32 offset) {
    NuFileSeek(file, offset, (NUFILESEEK)origin);
}

void NuSoundLoaderOGG::OGGFileCallbacks::Close() {
    NuFileClose(file);
}

NUFILE NuSoundLoaderOGG::OGGFileCallbacks::GetFile() const {
    return file;
}

i32 NuSoundLoaderOGG::OGGFileCallbacks::GetPosition() const {
    return NuFilePos(file);
}

int NuSoundLoaderOGG::OggCallbackClose(void *callbacks) {
    (*(OGGCallbacksVTable **)callbacks)->close(callbacks);
    return 0;
}

int NuSoundLoaderOGG::OggCallbackSeek(void *callbacks, i64 offset, i32 origin) { // NOLINT(google-runtime-int)
    (*(OGGCallbacksVTable **)callbacks)->seek(callbacks, origin, (u32)offset);
    return 0;
}

long NuSoundLoaderOGG::OggCallbackTell(void *callbacks) { // NOLINT(google-runtime-int)
    return (*(OGGCallbacksVTable **)callbacks)->get_position(callbacks);
}

bool NuSoundLoaderOGG::SeekRawData(u64 position) {
    u64 data_offset = this->desc->GetDataOffset();
    return NuFileSeek(file, data_offset + position, NUFILE_SEEK_START) != 0;
}

i32 NuSoundLoaderOGG::OpenFileForStreaming(const char *path, bool flag) {
    if (!flag) {
        return NuSoundLoader::OpenFileForStreaming(path, flag);
    }
    this->file = NuFileOpen((char *)path, NUFILE_READ);
    return this->file == 0 ? 2 : 1;
}

void NuSoundLoaderOGG::Close() {
    if (this->desc != NULL) {
        NuIOS_IsLowEndDevice();
        NuSoundHeaderOGG *header = (NuSoundHeaderOGG *)this->desc;
        ov_clear(&header->ogg_file);
    }
    if (this->file != 0) {
        NuFileClose(this->file);
    }
    FreeMemoryBuffer();
    this->file = 0;
}

usize NuSoundLoaderOGG::OggCallbackRead(void *dest, usize count, usize size, void *callbacks_) {
    return (*(OGGCallbacksVTable **)callbacks_)->read(callbacks_, dest, count * size);
}

i32 NuSoundLoaderOGG::ReadHeader(NuSoundStreamDesc *desc) {
    NuIOS_IsLowEndDevice();

    NuSoundHeaderOGG *header = (NuSoundHeaderOGG *)desc;
    OggVorbis_File *ogg_file = &header->ogg_file;

    file_callbacks.SetFile(file);

    u32 channels = ov_open_callbacks( //
        &file_callbacks,              //
        ogg_file,                     //
        NULL,                         //
        0,                            //
        (ov_callbacks){
            .read_func = OggCallbackRead,
            .seek_func = OggCallbackSeek,
            .close_func = OggCallbackClose,
            .tell_func = OggCallbackTell,
        } //
    );

    if (channels >= 0) {
        vorbis_info *info = ov_info(ogg_file, 0);
        if (info != NULL) {
            channels = info->channels;
            u16 rate = info->rate;
            header->sample_rate = rate;
            header->bits_per_channel = 16;
            header->format_id = -2;
            header->num_channels = channels;
            header->block_size = channels * 2;
            header->samples_per_second = rate * channels * 2;
            header->extended_data_size = 0x16;
            *(u16 *)&header->extended_data[0] = 0x10;
            if (channels > 0) {
                u32 channel_mask = 0;
                for (u32 channel = 0; channel < (u32)channels; channel++) {
                    channel_mask |= 1 << channel;
                }
                *(u32 *)&header->extended_data[2] = channel_mask;
            }

            // header->extended_data[0] = 0x10;
            // if (channels > 0) {
            //     u32 uVar2 = *(u32 *)(header->extended_data + 1);
            //     rate = 0;
            //     do {
            //         bVar3 = (byte)rate;
            //         rate = rate + 1;
            //         uVar2 = uVar2 | 1 << (bVar3 & 0x1f);
            //     } while (rate != channels);
            //     *(u32 *)((header->parent).extended_data + 1) = uVar2;
            // }

            header->encoded_length_bytes = NuFileOpenSize(file);

            u32 total = ov_pcm_total(ogg_file, -1);
            header->decoded_length_bytes = header->block_size * total;

            u32 pcm_total = ov_pcm_total(ogg_file, -1);
            header->length_samples = pcm_total;

            double time_total = ov_time_total(ogg_file, -1);
            header->length_seconds = (float)time_total;

            return 1;
        }

        NuFileClose(file);

        FreeMemoryBuffer();
    }

    return 3;
}

void NuSoundLoaderOGG::FreeMemoryBuffer() {
    if (buffer != NULL) {
        NU_FREE(buffer);
        buffer = NULL;
    }
}

NuSoundStreamDesc::DataFormat NuSoundHeaderOGG::GetDecodedDataFormat() const {
    return DataFormat::ZERO;
}

u64 NuSoundHeaderOGG::GetEncodedLengthBytes() const {
    return encoded_length_bytes;
}

u64 NuSoundHeaderOGG::GetLengthSamples() const {
    return length_samples;
}

f32 NuSoundHeaderOGG::GetLengthSeconds() const {
    return length_seconds;
}

u64 NuSoundHeaderOGG::GetDataOffset() const {
    return 0;
}

u16 NuSoundHeaderOGG::GetNumChannels() const {
    return num_channels;
}

u32 NuSoundHeaderOGG::GetSampleRate() const {
    return sample_rate;
}

u16 NuSoundHeaderOGG::GetBitsPerChannel() const {
    return bits_per_channel;
}

u16 NuSoundHeaderOGG::GetBlockSize() const {
    return block_size;
}

NuSoundStreamDesc::DataFormat NuSoundHeaderOGG::GetEncodedDataFormat() const {
    return DataFormat::THREE;
}

u64 NuSoundHeaderOGG::GetDecodedLengthBytes() const {
    return decoded_length_bytes;
}

u16 NuSoundHeaderOGG::GetInterleaveSize() const {
    return bits_per_channel / 8;
}

u16 NuSoundHeaderOGG::GetFormatID() const {
    return format_id;
}

u16 NuSoundHeaderOGG::GetExtendedDataSize() const {
    return extended_data_size;
}

void *NuSoundHeaderOGG::GetExtendedData() const {
    return (void *)extended_data;
}
