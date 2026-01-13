#include "nu2api.saga/nufile/nufile.h"

#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include "implode/implode.h"

#include "decomp.h"

#include <stdlib.h>
#include <string.h>

static char decode_buffer[0x40000] = {0};
static int32_t decode_buffer_pos = 0;
static int32_t decode_buffer_left = 0;
static char read_buffer[0x40000] = {0};
static int32_t read_buffer_size = 0;
static int32_t read_buffer_decoded_size = 0;

static int32_t NameToHash(char *name) {
    int32_t hash = 0x811c9dc5;
    int32_t prime = 0x199933;

    for (char *ptr = name; *ptr != '\0'; ptr++) {
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

static int32_t OpenDatFileBase(nudathdr_s *header, int32_t someIndex) {
    nudatopenfileinfo_s *fileInfo = header->openFiles + someIndex;

    if (fileInfo->file == 0) {
        fileInfo->file = NuFileOpenDF(header->path, (NUFILEMODE)header->mode, NULL, 0);
        header->openFiles[someIndex].position.l = 0;
    }

    return fileInfo->file;
}

int32_t datsys_offset = 0;

int32_t NuDatFileFindTree(nudathdr_s *header, char *name) {
    LOG("header=%p name=%s", header, name);

    int32_t index;
    char buf[256];
    int32_t i;
    char *backslash;

    if (*name == '@') {
        name = name + 4;
    }
    NuStrCpy(buf, name);
    NuFileUpCase(0, buf);
    name = buf;

    if (header->filetree == NULL) {
        index = NuDatFileFindHash(header, name);
    } else {
        i = (int)header->filetree->field0_0x0;
        backslash = strchr(name, L'\\');
        if (backslash != (char *)0x0) {
            *backslash = '\0';
        }
        do {
            index = NuStrICmp(header->filetree[i].name, name);
            if (index == 0) {
                if (backslash == (char *)0x0) {
                    if (header->filetree[i].field0_0x0 < 1) {
                        return -(int)header->filetree[i].field0_0x0;
                    }
                    return -1;
                }
                i = (int)header->filetree[i].field0_0x0;
                name = backslash + 1;
                backslash = strchr(name, L'\\');
                if (backslash != (char *)0x0) {
                    *backslash = '\0';
                }
            } else {
                i = (int)header->filetree[i].field1_0x2;
            }
        } while (i != 0);
        index = -1;
    }

    return index;
}

NUFILE NuDatFileOpen(NUDATHDR *header, char *name, NUFILEMODE mode) {
    LOG("header=%p name=%s mode=%d", header, name, mode);

    int iVar1;
    int index;
    int fileIndex;
    longlong pos;
    longlong lVar2;
    int highPos;

    if ((((mode == NUFILE_MODE_READ) && (iVar1 = NuDatFileFindTree(header, name), -1 < iVar1)) &&
         (header->finfo[iVar1].field1_0x4 != 0)) &&
        (index = NuDatFileGetFreeInfo(), index != -1)) {
        fileIndex = NuDatFileGetFreeHandleIX(header, index);
        if (fileIndex != -1) {
            NuThreadCriticalSectionBegin(file_criticalsection);
            OpenDatFileBase(header, fileIndex);
            dat_file_infos[index].ptr = header;
            pos = (longlong)NuDatCalcPos(header, header->finfo[iVar1].field0_0x0);
            dat_file_infos[index].pos.l = pos;
            highPos = dat_file_infos[index].pos.i[1];
            dat_file_infos[index].pos2.i[0] = dat_file_infos[index].pos.i[0];
            dat_file_infos[index].pos2.i[1] = highPos;
            dat_file_infos[index].field3_0x14 = header->finfo[iVar1].field1_0x4;
            dat_file_infos[index].field4_0x18 = header->finfo[iVar1].size;
            dat_file_infos[index].compressionMode = header->finfo[iVar1].compressionMode;
            dat_file_infos[index].index = fileIndex;
            iVar1 = dat_file_infos[index].pos2.i[1];
            header->openFiles[fileIndex].position.i[0] = dat_file_infos[index].pos2.i[0];
            header->openFiles[fileIndex].position.i[1] = iVar1;
            do {
                lVar2 = NuFileSeek(header->openFiles[fileIndex].file, dat_file_infos[index].pos.l, NUFILE_SEEK_START);
            } while (lVar2 < 0);
            if ((dat_file_infos[index].compressionMode == 2) || (dat_file_infos[index].compressionMode == 3)) {
                // NuDatFileDecodeNewFile(dat_file_infos + index, header->openFiles + fileIndex);
                UNIMPLEMENTED();
            }
            NuThreadCriticalSectionEnd(file_criticalsection);
            return index + 0x800;
        }
        dat_file_infos[index].used = 0;
    }
    return 0;
}

void NuDatFileClose(NUFILE file) {
    int32_t index = NUFILE_INDEX_DAT(file);
    nudatfileinfo_s *info = &dat_file_infos[index];

    if (info->index >= 0) {
        info->ptr->openFiles[info->index].field1_0x4 = -1;
    }

    info->used = 0;
}

int64_t NuDatFileSeek(NUFILE file, int64_t seek, NUFILESEEK whence) {
    int32_t index = NUFILE_INDEX_DAT(file);

    nudathdr_s *hdr = dat_file_infos[index].ptr;
    int ofile = dat_file_infos[index].index;

    int64_t offset;

    if (whence == NUFILE_SEEK_CURRENT) {
        offset = seek + dat_file_infos[index].pos2.l;
    } else if (whence == NUFILE_SEEK_END) {
        int64_t end = (int64_t)dat_file_infos[index].field3_0x14 + dat_file_infos[index].pos.l;

        offset = end - seek;
    } else {
        offset = seek + dat_file_infos[index].pos.l;
    }

    int64_t pos = NuFileSeek(hdr->openFiles[ofile].file, offset, NUFILE_SEEK_START);

    dat_file_infos[index].pos2.l = pos;
    hdr->openFiles[ofile].position.l = pos;

    return pos;
}

uint64_t NuDatFilePos(NUFILE file) {
    int32_t index = NUFILE_INDEX_DAT(file);
    return dat_file_infos[index].pos2.l;
}

int32_t NuDatFileLoadBuffer(nudathdr_s *dat, char *name, void *dest, int32_t maxSize) {
    LOG("dat=%p name=%s dest=%p maxSize=%d", dat, name, dest, maxSize);

    nufile_lasterror = 0;

    NUFILE file = NuDatFileOpen(dat, name, NUFILE_MODE_READ);

    if (file == 0) {
        return 0;
    }

    if (dat->mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    int32_t size;

    if (dat_file_infos[file + -0x800].compressionMode == 0) {
        size = dat_file_infos[file + -0x800].field3_0x14;
    } else {
        size = dat_file_infos[file + -0x800].field4_0x18;
    }

    if (size <= maxSize && size != 0) {
        LOG("Loading %d bytes from dat file", size);

        while (NuDatFileRead(file, dest, size) < 0) {
            while (NuDatFileSeek(file, 0, NUFILE_SEEK_START) < 0) {
            }
        }

        if (dat->mode == 3) {
            while (NuFileStatus(file) != 0) {
            }
        }

        NuFileClose(file);

        return size;
    }

    if (size != 0) {
        nufile_lasterror = -1;
    }

    NuFileClose(file);

    return 0;
}

int32_t NuDatFileRead(NUFILE file, void *dest, int32_t size) {
    nudathdr_s *pnVar1;
    uint uVar2;
    size_t n;
    uint uVar3;
    uint length;
    int iVar4;
    uint pos;
    int fileIndex;
    int index;

    fileIndex = file + -0x800;
    pnVar1 = dat_file_infos[fileIndex].ptr;
    index = dat_file_infos[fileIndex].index;
    if (dat_file_infos[fileIndex].compressionMode == 0) {
        if (pnVar1->openFiles[index].position.i[0] != dat_file_infos[fileIndex].pos2.i[0] ||
            dat_file_infos[fileIndex].pos2.i[1] != pnVar1->openFiles[index].position.i[1]) {
            NuFileSeek(pnVar1->openFiles[index].file, dat_file_infos[fileIndex].pos2.l, NUFILE_SEEK_START);
            iVar4 = dat_file_infos[fileIndex].pos2.i[1];
            pnVar1->openFiles[index].position.i[0] = dat_file_infos[fileIndex].pos2.i[0];
            pnVar1->openFiles[index].position.i[1] = iVar4;
        }
        uVar2 = dat_file_infos[fileIndex].field3_0x14;
        uVar3 = dat_file_infos[fileIndex].pos.u[0] + uVar2;
        length = uVar3 - dat_file_infos[fileIndex].pos2.u[0];
        iVar4 = ((dat_file_infos[fileIndex].pos.i[1] + ((int)uVar2 >> 0x1f) +
                  (uint)CARRY4(dat_file_infos[fileIndex].pos.u[0], uVar2)) -
                 dat_file_infos[fileIndex].pos2.i[1]) -
                (uint)(uVar3 < dat_file_infos[fileIndex].pos2.u[0]);
        if (iVar4 < 0) {
            length = 0;
            iVar4 = 0;
        }
        if ((size >> 0x1f <= iVar4) && ((size >> 0x1f < iVar4 || ((uint)size < length)))) {
            length = size;
        }
        if (length == 0) {
            pos = 0;
        } else {
            pos = NuFileRead(pnVar1->openFiles[index].file, dest, length);
            if (-1 < (int)pos) {
                uVar2 = dat_file_infos[fileIndex].pos2.u[0];
                iVar4 = dat_file_infos[fileIndex].pos2.i[1];
                dat_file_infos[fileIndex].pos2.i[0] = pos + uVar2;
                dat_file_infos[fileIndex].pos2.i[1] = ((int)pos >> 0x1f) + iVar4 + (uint)CARRY4(pos, uVar2);
                iVar4 = dat_file_infos[fileIndex].pos2.i[1];
                pnVar1->openFiles[index].position.i[0] = dat_file_infos[fileIndex].pos2.i[0];
                pnVar1->openFiles[index].position.i[1] = iVar4;
            }
        }
    } else {
        pos = 0;
        for (; size != 0; size = size - n) {
            if (decode_buffer_left == 0) {
                // NuDatFileDecodeNext();
                UNIMPLEMENTED();

                if (read_buffer_size == read_buffer_decoded_size) {
                    memcpy(decode_buffer, read_buffer, read_buffer_size);
                } else if (dat_file_infos[fileIndex].compressionMode == 2) {
                    ExplodeBufferNoHeader(read_buffer, decode_buffer, read_buffer_size, read_buffer_decoded_size);
                } else if (dat_file_infos[fileIndex].compressionMode == 3) {
                    InflateBuffer(decode_buffer, read_buffer_decoded_size, read_buffer, read_buffer_size);
                }

                decode_buffer_pos = 0;
                decode_buffer_left = read_buffer_decoded_size;
            }
            n = decode_buffer_left;
            if (size < decode_buffer_left) {
                n = size;
            }
            memcpy(dest, decode_buffer + decode_buffer_pos, n);
            dest = (void *)((int)dest + n);
            decode_buffer_pos = n + decode_buffer_pos;
            decode_buffer_left = decode_buffer_left - n;
            pos = pos + n;
        }
    }
    return pos;
}

int64_t NuDatCalcPos(nudathdr_s *header, int32_t index) {
    int64_t pos = index;

    if (header->version < -1) {
        pos <<= 8;
    }

    return pos + datsys_offset;
}

int32_t NuDatFileOpenSize(NUFILE file) {
    int32_t index = NUFILE_INDEX_DAT(file);
    if (dat_file_infos[index].compressionMode == 0) {
        return dat_file_infos[index].field3_0x14;
    } else {
        return dat_file_infos[index].field4_0x18;
    }
}