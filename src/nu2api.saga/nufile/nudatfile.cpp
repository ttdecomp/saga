#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include "decomp.h"

static int32_t NameToHash(char *name) {
    int32_t hash = 0x811c9dc5;
    int32_t prime = 0x199933;
    char *ptr = name;

    while (*++ptr != '\0') {
        hash = prime * (hash ^ (int32_t)*ptr);
    }

    return hash;
}

size_t BinarySearch(uint32_t element, uint32_t *array, size_t length) {
    size_t start = 0;
    size_t end = length - 1;

    while (end >= start) {
        size_t index = (start + end) / 2;
        if (array[index] == element) {
            return index;
        } else if (array[index] < element) {
            start = index + 1;
        } else {
            end = index - 1;
        }
    }

    return -1;
}

int32_t NuDatFileGetFreeInfo(void) {
    int32_t found = -1;

    NuThreadCriticalSectionBegin(file_criticalsection);

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

int32_t NuDatFileGetFreeHandleIX(nudathdr_s *header, int32_t freeIndex) {
    int32_t i;
    int32_t local_10;

    local_10 = -1;
    NuThreadCriticalSectionBegin(file_criticalsection);
    i = 0;
    do {
        if (19 < i) {
        LAB_0026cede:
            NuThreadCriticalSectionEnd(file_criticalsection);
            return local_10;
        }
        if (header->openFiles[i].field1_0x4 == -1) {
            header->openFiles[i].field1_0x4 = freeIndex;
            local_10 = i;
            goto LAB_0026cede;
        }
        i = i + 1;
    } while (1);
}

int32_t NuDatFileFindHash(nudathdr_s *header, char *name) {
    LOG("header=%p name=%s", header, name);

    int32_t hash = NameToHash(name);

    int32_t r = BinarySearch(hash, header->hashes, header->filesCount);

    if (r == -1) {
        if (header->count3 != 0) {
            char *ptr = header->arr4;
            for (int32_t i = 0; i < header->count3; i = i + 1) {
                r = NuStrCmp(ptr, name);
                if (r == 0) {
                    r = NuStrLen(ptr);
                    ptr = ptr + r + 1;
                    if (((uint)ptr & 1) != 0) {
                        ptr = ptr + 1;
                    }
                    return (int)*(short *)ptr;
                }
                r = NuStrLen(ptr);
                ptr = ptr + r + 3;
                if (((uint)ptr & 1) != 0) {
                    ptr = ptr + 1;
                }
            }
        }

        r = -1;
    }

    LOG("header=%p name=%s => %d", header, name, r);

    return r;
}

int32_t OpenDatFileBase(nudathdr_s *header, int32_t someIndex) {
    nudatopenfileinfo_s *pnVar1 = header->openFiles + someIndex;

    if (pnVar1->file == 0) {
        pnVar1->file = NuFileOpenDF(header->path, (NuFileOpenMode)header->mode, NULL);
        header->openFiles[someIndex].position.i[0] = 0;
        header->openFiles[someIndex].position.i[1] = 0;
    }

    return pnVar1->file;
}

int32_t datsys_offset = 0;

int64_t NuDatCalcPos(nudathdr_s *header, int32_t index) {
    uint32_t a;
    uint32_t b;
    int64 ret;

    b = index >> 31;
    a = index;
    if (header->version < -1) {
        b = b << 8 | (uint)index >> 24;
        a = index << 8;
    }
    ret.i[1] = b + (datsys_offset >> 0x1f) + (uint)CARRY4(a, datsys_offset);
    ret.i[0] = a + datsys_offset;
    return ret.l;
}
