#include "nu2api/nusound/nusound_loader.hpp"

#include "nu2api/numath/nufloat.h"

#include <string.h>

NuSoundLoader::NuSoundLoader() {
    this->file = 0;
    this->field2_0x8 = 0;
    this->field3_0xc = 0;
    this->desc = NULL;
    this->oom = NULL;
}

NuSoundLoader::~NuSoundLoader() {
}

i32 NuSoundLoader::LoadFromFile(const char *name, NuSoundStreamDesc *desc, NuSoundBuffer *buffer,
                                NuSoundOutOfMemCallback *oom) {
    this->oom = oom;
    this->path = name;

    NUFILE file = NuFileOpen(const_cast<char *>(name), NUFILE_READ);
    this->file = file;

    if (file == 0) {
        return 2;
    }

    return Load(desc, buffer);
}

i32 NuSoundLoader::Load(NuSoundStreamDesc *desc, NuSoundBuffer *buffer) {
    UNIMPLEMENTED();
}

i32 NuSoundLoader::OpenForStreaming(const char *path, f64 length, NuSoundStreamDesc *desc, bool param4) {
    i32 ret = OpenFileForStreaming(path, param4);
    if (ret != 1) {
        return ret;
    }

    ret = ReadHeader(desc);
    if (ret != 1) {
        Close();
        return ret;
    }

    this->desc = desc;

    if (SeekTime(NuFmod(length, desc->GetLengthSeconds())) == 0) {
        SeekTime(0);
    }

    return 1;
}

void NuSoundLoader::FillStreamBuffer(NuSoundBuffer *buffer, bool param3) {
    NuSoundBuffer::Context context;

    if (this->file == 0) {
        return;
    }

    buffer->Lock();
    u8 *data = (u8 *)buffer->GetAddress();
    u64 buffer_size = buffer->GetBufferSize();

    u64 uVar3iVar2 = context.size2;

    u64 read_size, size;

    do {
        do {
            if (buffer_size <= uVar3iVar2) {
            LAB_0033fce8:
                buffer->SetCurrentContext(context);
                buffer->Unlock();
                return;
            }
            read_size = buffer_size - uVar3iVar2;
            u64 size = ReadData(data, read_size);

            context.read_size += size;

            data += size;

            context.size2 += size;

        } while (read_size == size);

        SeekRawData(0);

        context.size3 = context.read_size;

        u32 uVar5 = buffer->GetBufferSize();

        u32 uVar6 = this->desc->GetEncodedLengthBytes();

        if ((uVar6 <= uVar5) || (!param3)) {
            context.flags |= 2;
            goto LAB_0033fce8;
        }

        uVar3iVar2 = context.size2;

    } while (true);
}

bool NuSoundLoader::SeekRawData(u64 position) {
    if (this->desc == NULL || this->desc->GetEncodedLengthBytes() < position || this->file == 0) {
        return 0;
    } else {
        u64 data_offset = this->desc->GetDataOffset();
        NuFileSeek(this->file, data_offset + position, NUFILE_SEEK_START);
        return 1;
    }
}

i32 NuSoundLoader::OpenFileForStreaming(char const *name, bool) {
    this->file = NuFileOpen(const_cast<char *>(name), NUFILE_READ);

    return this->file == 0 ? 2 : 1;
}

void NuSoundLoader::Close() {
    UNIMPLEMENTED();
}

u64 NuSoundLoader::ReadData(void *dest, u64 size) {
    LOG_DEBUG("NuSoundLoader::ReadData: dest=%p, size=%lu", dest, size);

    u64 read = 0;

    if (this->file != 0) {
        read = NuFileRead(this->file, dest, size);
    }

    return read;
}
