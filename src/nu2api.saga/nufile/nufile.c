#include "nu2api.saga/nufile/nufile.h"

#include "nu2api.saga/nustr/nustr.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include <stddef.h>
#include <string.h>

#include "decomp.h"

NuFileHandle NuFileOpen(const char *path, NuFileOpenMode mode) {
    return NuFileOpenDF(path, mode, curr_dat);
}

NuFileHandle NuFileOpenDF(const char *path, NuFileOpenMode mode, nudathdr_s *header) {
    UNIMPLEMENTED();
}

NuFileDevice *NuFileGetDeviceFromPath(const char *path) {
    int i;

    for (i = 0; i < 8 && (path[i] != ':'); i++) {
    }

    if (i < 8) {
        for (int index = 0; index < _numdevices; index++) {
            // if (NuStrNICmp(path, index * 0x234 + 0x6c93b4, devices[index].length) == 0) {
            // return &devices[index];
            //}
            UNIMPLEMENTED();
        }
    }

    return NULL;
}

int32_t NameToHash(const char *name) {
    int32_t hash = 0x811c9dc5;

    for (; *name != 0; name = name + 1) {
        hash = (hash ^ (int32_t)*name) * 0x199933;
    }

    return hash;
}

size_t BinarySearch(int32_t element, int32_t *array, size_t length) {
    size_t end = length - 1;
    size_t start = 0;

    while (1) {
        if (end < start) {
            return -1;
        }

        size_t index = (end + start) / 2;
        if (array[index] == element) {
            return index;
        } else if (array[index] < element) {
            start = index + 1;
        } else {
            end = index - 1;
        }
    }
}

int32_t NuDatFileGetFreeInfo(void) {
    NuThreadCriticalSectionBegin(file_criticalsection);

    int32_t found = -1;
    for (int32_t i = 0; i < 20; i++) {
        if (dat_file_infos[i].used == 0) {
            dat_file_infos[i].used = 1;
            found = i;
            break;
        }
    }

    NuThreadCriticalSectionEnd(file_criticalsection);
    return found;
}

size_t NuPSFileRead(int32_t index, void *dest, size_t len) {
    return fread(dest, 1, len, g_fileHandles[index]);
}

size_t NuMemFileRead(int32_t file, char *dest, size_t size) {
    if (NUFILE_IS_MEM(file)) {
        int32_t i = NUFILE_INDEX_MEM(file);
        ptrdiff_t remaining = memfiles[i].end - memfiles[i].ptr + 1;

        if (remaining <= size) {
            size = remaining;
        }

        if (size != 0) {
            memcpy(dest, memfiles[i].ptr, size);
            memfiles[i].ptr = ((char *)memfiles[i].ptr + size);
        }

        return size;
    } else {
        return NuDatFileRead(file, dest, size);
    }
}

NuFileHandle NuDatFileOpen(nudathdr_s *header, const char *name, int32_t mode) {
    UNIMPLEMENTED();
}

size_t NuDatFileRead(NuFileHandle file, void *dest, size_t size) {
    UNIMPLEMENTED();
}

size_t NuFileRead(NuFileHandle index, void *dest, size_t length) {
    if (NUFILE_IS_PS(index)) {
        return NuPSFileRead(index, dest, length);
    } else if (NUFILE_IS_MEM(index) || NUFILE_IS_DAT(index)) {
        return NuMemFileRead(index, dest, length);
    } else if (NUFILE_IS_MC(index)) {
        UNIMPLEMENTED("MC file read not implemented");
    } else if (NUFILE_IS_NATIVE(index)) {
        UNIMPLEMENTED("Native file read not implemented");
    }
}

int32_t NuFileSeek(NuFileHandle file, int64_t offset, int32_t seekMode) {
    UNIMPLEMENTED();
}

nudathdr_s *NuDatOpen(char *name, void **bufferBase, int32_t zero) {
    return NuDatOpenEx(name, bufferBase, zero, 0);
}

nudathdr_s *NuDatOpenEx(char *name, void **bufferBase, int zero, short mode) {
    NuFileHandle file = NuFileOpenDF(name, mode, (nudathdr_s *)0x0);

    if (file == NUFILE_INVALID) {
        return NULL;
    }

    size_t size = NuFileOpenSize(file);

    int32_t buffer[2];
    NuFileRead(file, (char *)buffer, 8);

    int64_t offset = ((int64_t)buffer[0] << 32) | buffer[1];

    NuFileSeek(file, offset, 0);

    nudathdr_s *header = BUFFER_ALLOC(bufferBase, nudathdr_s);
    memset(header, 0, sizeof(nudathdr_s));

    header->field336_0x16c = 1;
    header->path = buffer_alloc(bufferBase, NuStrLen(name) + 1, 1);
    NuStrCpy(header->path, name);

    NuFileRead(file, header, 4);
    NuFileRead(file, &header->hashesCount, 4);

    header->field5_0x8 = buffer_alloc(bufferBase, 0x10 * header->hashesCount, 1);
    NuFileRead(file, header->field5_0x8, header->hashesCount * 0x10);

    NuFileRead(file, &header->field6_0xc, 4);

    UNIMPLEMENTED();
}

size_t NuFileOpenSize(NuFileHandle file) {
    UNIMPLEMENTED();
}