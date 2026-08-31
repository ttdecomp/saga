// NuSoundLoaderWAV — decompiled from libTTapp.so
// (nu2api.2013/nusound/nusound_loader_wav.cpp). RIFF/WAVE chunk walker used
// for WAV and MIB (raw PCM) streams; the title music is streamed through it.

#include "nu2api_nusound_types.h"

#include "decomp.h"

#include "nu2api/nufile/nufile.h"

#include <new>
#include <string.h>

NuSoundLoaderWAV::NuSoundLoaderWAV() {
}

NuSoundLoaderWAV::~NuSoundLoaderWAV() {
}

NuSoundStreamDesc *NuSoundLoaderWAV::CreateHeader() {
    NuSoundHeaderWAV *header = (NuSoundHeaderWAV *)NuSoundSystem::_AllocMemory(
        NuSoundSystem::MemoryDiscipline::SCRATCH, sizeof(NuSoundHeaderWAV), 4,
        "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound_loader_wav.cpp:34");
    if (header != NULL) {
        new (header) NuSoundHeaderWAV;
    }
    return header;
}

u32 NuSoundLoaderWAV::MakeFourCC(char *cc) {
    return ((u32)cc[3] << 0x18) | ((u32)cc[2] << 0x10) | ((u32)cc[1] << 8) | (u32)cc[0];
}

void NuSoundLoaderWAV::EndianFlipWAVHeader(FileHeaderWAV *header) {
    // Big-endian targets only; a no-op on little endian.
    (void)header;
}

void NuSoundLoaderWAV::ReadRIFFHeaderChunk(i32 file, NuSoundStreamDesc *desc, const ChunkInfo &info,
                                           NuSoundLoaderWAV *loader) {
    (void)info;
    (void)loader;

    FileHeaderWAV header;
    memset(&header, 0, sizeof(header));
    if (NuFileRead(file, &header, 0x12) != 0x12) {
        return;
    }
    EndianFlipWAVHeader(&header);

    // The fmt chunk overlays the head of the stream desc.
    NuSoundHeaderWAV *wav = (NuSoundHeaderWAV *)desc;
    wav->format_id = header.format;
    wav->num_channels = header.num_channels;
    wav->sample_rate = header.sample_rate;
    wav->samples_per_second = header.sample_rate;
    wav->block_size = header.block_size;
    wav->bits_per_channel = header.bits_per_channel;
    wav->extended_data_size = header.extended_size;
}

void NuSoundLoaderWAV::ReadDataChunk(i32 file, NuSoundStreamDesc *desc, const ChunkInfo &info,
                                     NuSoundLoaderWAV *loader) {
    (void)loader;

    NuSoundHeaderWAV *header = (NuSoundHeaderWAV *)desc;
    header->encoded_length_bytes = (u64)info.size;
    header->data_position = (u64)NuFilePos(file);
}

u32 NuSoundLoaderWAV::FindChunks(i32 file, NuSoundStreamDesc *desc, ChunkReadRequest *requests, u32 count) {
    if (file == 0 || NuFileSeek(file, 0xc, NUFILE_SEEK_START) == 0) {
        return 0;
    }

    u32 found_mask = 0;
    u32 wanted_mask = (count >= 32) ? 0xffffffffu : (1u << count) - 1;
    u64 file_offset = 0xc;

    for (;;) {
        ChunkInfo read_info;
        if (NuFileRead(file, &read_info, 8) != 8) {
            return found_mask;
        }
        file_offset += (u64)read_info.size + 8;

        for (u32 i = 0; i < count; i++) {
            u32 expected_id = requests[i].chunk_id;
            if (((found_mask & (1u << i)) == 0) && read_info.id == expected_id) {
                requests[i].chunk_info = read_info;

                if (requests[i].reader != NULL) {
                    requests[i].reader(file, desc, requests[i].chunk_info, this);
                }
                requests[i].state = 1;
                found_mask |= 1u << i;
                break;
            }
        }

        if (found_mask == wanted_mask) {
            return found_mask;
        }

        if (NuFileSeek(file, (i64)file_offset, NUFILE_SEEK_START) == 0) {
            return found_mask;
        }
    }
}

u32 NuSoundLoaderWAV::FindChunk(i32 file, u32 id, ChunkInfo &info) {
    ChunkReadRequest request;
    memset(&request, 0, sizeof(request));
    request.chunk_id = id;
    u32 found = this->FindChunks(file, NULL, &request, 1);
    if (found != 0) {
        info = request.chunk_info;
        return request.state;
    }
    return 0;
}

i32 NuSoundLoaderWAV::ReadHeader(NuSoundStreamDesc *desc) {
    static u32 riffId = 0;
    static u32 waveId = 0;
    static u32 formatId = 0;
    static u32 dataId = 0;
    static u32 seekId = 0;

    if (riffId == 0) {
        riffId = MakeFourCC((char *)"RIFF");
    }
    if (waveId == 0) {
        waveId = MakeFourCC((char *)"WAVE");
    }
    if (formatId == 0) {
        formatId = MakeFourCC((char *)"fmt ");
    }
    if (dataId == 0) {
        dataId = MakeFourCC((char *)"data");
    }
    if (seekId == 0) {
        seekId = MakeFourCC((char *)"seek");
    }

    u32 read_id = 0;
    if (NuFileRead(this->file, &read_id, 4) != 4 || read_id != riffId) {
        return 3;
    }

    u32 unused = 0;
    NuFileRead(this->file, &unused, 4);

    if (NuFileRead(this->file, &read_id, 4) != 4 || read_id != waveId) {
        return 3;
    }

    ChunkReadRequest requests[2];
    memset(requests, 0, sizeof(requests));
    requests[0].chunk_id = formatId;
    requests[0].state = 3;
    requests[0].reader = &NuSoundLoaderWAV::ReadRIFFHeaderChunk;
    requests[1].chunk_id = dataId;
    requests[1].state = 4;
    requests[1].reader = &NuSoundLoaderWAV::ReadDataChunk;
    this->FindChunks(this->file, desc, requests, 2);

    return requests[0].state == 1 ? requests[1].state : requests[0].state;
}

bool NuSoundLoaderWAV::SeekPCMSample(u64 index) {
    (void)index;
    return false;
}

bool NuSoundLoaderWAV::SeekTime(f64 seconds) {
    // The original leaves this unimplemented: WAV/MIB streams always start
    // from the raw data offset.
    (void)seconds;
    return false;
}
