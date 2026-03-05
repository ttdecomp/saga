#include "nusound_loader_ogg.hpp"

#include <new>

#include <vorbis/vorbisfile.h>

#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/numemory.h"

i32 NuSoundLoaderOGG::OGGFileCallbacks::Read(void *dest, u32 size) {
    return NuFileRead(file, dest, size);
}

void NuSoundLoaderOGG::OGGFileCallbacks::SetFile(NUFILE file) {
    this->file = file;
}

NuSoundLoaderOGG::NuSoundLoaderOGG() {
}

NuSoundStreamDesc *NuSoundLoaderOGG::CreateHeader() {
    NuSoundHeaderOGG *header = (NuSoundHeaderOGG *)NuSoundSystem::_AllocMemory(
        NuSoundSystem::MemoryDiscipline::SCRATCH, 0x308, 4,
        "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound_loader_ogg.cpp:280");

    if (header != NULL) {
        new (header) NuSoundHeaderOGG();
    }

    return header;
}

bool NuSoundLoaderOGG::SeekPCMSample(u64 index) {
    NuIOS_IsLowEndDevice();

    i32 ret = ov_pcm_seek(&desc->ogg_file, index);
    return ret == 0;
}

bool NuSoundLoaderOGG::SeekTime(double seconds) {
    NuIOS_IsLowEndDevice();
    i32 iVar2 = ov_time_seek(&desc->ogg_file, seconds);
    return iVar2 == 0;
}

usize NuSoundLoaderOGG::OggCallbackRead(void *dest, usize count, usize size, void *callbacks_) {
    OGGFileCallbacks *callbacks = (OGGFileCallbacks *)callbacks_;
    return callbacks->Read(dest, count * size);
}

i32 NuSoundLoaderOGG::ReadHeader(NuSoundStreamDesc *desc) {
    NuIOS_IsLowEndDevice();

    OggVorbis_File *ogg_file = &desc->ogg_file;

    file_callbacks.SetFile(file);

    u32 channels = ov_open_callbacks( //
        &file_callbacks,              //
        ogg_file,                     //
        NULL,                         //
        0,                            //
        (ov_callbacks){
            .read_func = OggCallbackRead,
        } //
    );

    if (channels >= 0) {
        vorbis_info *info = ov_info(ogg_file, 0);
        if (info != NULL) {
            channels = info->channels;
            u16 rate = info->rate;
            desc->sample_rate = rate;
            desc->bits_per_channel = 16;
            desc->format_id = -2;
            desc->num_channels = channels;
            desc->block_size = channels * 2;
            desc->samples_per_second = rate * channels * 2;
            desc->extended_data_size = 0x16;

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

            desc->file_size = NuFileOpenSize(file);

            u32 total = ov_pcm_total(ogg_file, -1);
            desc->decoded_length_bytes = desc->block_size * total;

            u32 pcm_total = ov_pcm_total(ogg_file, -1);
            desc->length_samples = pcm_total;

            double time_total = ov_time_total(ogg_file, -1);
            desc->length_seconds = (float)time_total;

            return 1;
        }

        NuFileClose(file);

        FreeMemoryBuffer();
    }
}

void NuSoundLoaderOGG::FreeMemoryBuffer() {
    NU_FREE(buffer);
}

NuSoundStreamDesc::DataFormat NuSoundHeaderOGG::GetDecodedDataFormat() {
    return DataFormat::ZERO;
}

u64 NuSoundHeaderOGG::GetEncodedLengthBytes() {
    return encoded_length_bytes;
}

u64 NuSoundHeaderOGG::GetLengthSamples() {
    return length_samples;
}

double NuSoundHeaderOGG::GetLengthSeconds() {
    return length_seconds;
}

u64 NuSoundHeaderOGG::GetDataOffset() {
    return 0;
}

u16 NuSoundHeaderOGG::GetNumChannels() {
    return num_channels;
}

u32 NuSoundHeaderOGG::GetSampleRate() {
    return sample_rate;
}

u16 NuSoundHeaderOGG::GetBitsPerChannel() {
    return bits_per_channel;
}

u16 NuSoundHeaderOGG::GetBlockSize() {
    return block_size;
}

NuSoundStreamDesc::DataFormat NuSoundHeaderOGG::GetEncodedDataFormat() {
    return DataFormat::THREE;
}

u64 NuSoundHeaderOGG::GetDecodedLengthBytes() {
    return decoded_length_bytes;
}

u16 NuSoundHeaderOGG::GetInterleaveSize() {
    return bits_per_channel / 8;
}

u16 NuSoundHeaderOGG::GetFormatId() {
    return format_id;
}

u16 NuSoundHeaderOGG::GetExtendedDataSize() {
    return extended_data_size;
}

void *NuSoundHeaderOGG::GetExtendedData() {
    return &extended_data;
}
