#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include "implode/implode.h"

#include <stddef.h>
#include <string.h>

#include "decomp.h"

nudathdr_s *NuDatOpen(char *name, void **bufferBase, int32_t zero) {
    LOG("name=%s bufferBase=%p zero=%d", name, bufferBase, zero);
    return NuDatOpenEx(name, bufferBase, zero, 0);
}

nudathdr_s *NuDatSet(nudathdr_s *header) {
    nudathdr_s *dat = curr_dat;
    curr_dat = header;
    return dat;
}

uint64_t NuDatFilePos(NuFileHandle file) {
    int32_t index = file + -0x800;
    return dat_file_infos[index].pos2.l;
}

void APIEndianSwap(void *data, size_t count, size_t size) {
    return;
}

int32_t DontDoFileUpperCaseHack = 0;

void NuFileUpCase(void *param_1, char *path) {
    char local_5;

    if (param_1 == (void *)0x0) {
        local_5 = '\\';
    } else {
        local_5 = *(char *)((int)param_1 + 0x10);
    }

    if (DontDoFileUpperCaseHack == 0) {
        for (; *path != '\0'; path = path + 1) {
            char cVar1 = *path;
            if (cVar1 < '{') {
                if (cVar1 < 'a') {
                    if ((cVar1 == '/') || (cVar1 == '\\')) {
                        *path = local_5;
                    }
                } else {
                    *path = *path + -0x20;
                }
            }
        }
    } else {
        for (; *path != '\0'; path = path + 1) {
            if ((*path == '\\') || (*path == '/')) {
                *path = local_5;
            }
        }
    }
}

nudathdr_s *NuDatOpenEx(char *name, void **bufferBase, int zero, short mode) {
    LOG("name=%s bufferBase=%p zero=%d mode=%d", name, bufferBase, zero, mode);

    int32_t buffering = nufile_buffering_enabled;

    NuFileHandle file = NuFileOpenDF(name, (int32_t)mode, NULL);
    if (file != 0) {
        int32_t size = NuFileOpenSize(file);

        int32_t buffer[2];
        NuFileRead(file, buffer, 8);
        APIEndianSwap(buffer, 2, 4);

        int64_t offset = (int64_t)buffer[1] << 32 | (uint32_t)buffer[0];
        NuFileSeek(file, offset, 0);

        int32_t len = NuStrLen(name);

        nudathdr_s *header = (nudathdr_s *)*bufferBase;
        *bufferBase = (void *)((int)*bufferBase + 0x178);
        memset(header, 0, 0x178);

        header->field300_0x16c = 1;
        header->path = (char *)*bufferBase;
        *bufferBase = (void *)((int)*bufferBase + (len + 0x10U & 0xfffffff0));
        NuStrCpy(header->path, name);

        NuFileRead(file, header, 4);
        APIEndianSwap(header, 1, 4);

        NuFileRead(file, &header->filesCount, 4);
        APIEndianSwap(&header->filesCount, 1, 4);

        header->finfo = BUFFER_ALLOC_ARRAY(bufferBase, header->filesCount, nudathdr_struct1);
        NuFileRead(file, header->finfo, header->filesCount << 4);

        for (int32_t i = 0; i < header->filesCount; i = i + 1) {
            APIEndianSwap(header->finfo + i, 1, 4);
            APIEndianSwap(&header->finfo[i].field1_0x4, 1, 4);
            APIEndianSwap(&header->finfo[i].size, 1, 4);
        }

        NuFileRead(file, &header->treeCount, 4);
        APIEndianSwap(&header->treeCount, 1, 4);
        header->filetree = (nudathdr_struct2 *)*bufferBase;
        *bufferBase = (void *)((int)*bufferBase + header->treeCount * 0xc);

        if (header->version < -4) {
            NuFileRead(file, header->filetree, header->treeCount * 0xc);
        } else {
            NuFileRead(file, header->filetree, header->treeCount * 8);
            nudathdr_struct2 *struct2 = header->filetree;
            int32_t k = header->treeCount;
            while (k = k + -1, 0 < k) {
                header->filetree[k].field4_0xa = 0;
                header->filetree[k].field3_0x8 = 0;
                header->filetree[k].name = *(char **)((int)struct2 + k * 8 + 4);
                header->filetree[k].field1_0x2 = *(short *)((int)struct2 + k * 8 + 2);
                header->filetree[k].field0_0x0 = *(short *)(k * 8 + (int)struct2);
            }
        }

        for (int32_t k = 0; k < header->treeCount; k = k + 1) {
            APIEndianSwap(header->filetree + k, 1, 2);
            APIEndianSwap(&header->filetree[k].field1_0x2, 1, 2);
            APIEndianSwap(&header->filetree[k].name, 1, 4);
            if (header->version < -4) {
                APIEndianSwap(&header->filetree[k].field3_0x8, 1, 2);
                APIEndianSwap(&header->filetree[k].field4_0xa, 1, 2);
            }
        }

        NuFileRead(file, &header->leafnamesize, 4);
        APIEndianSwap(&header->leafnamesize, 1, 4);

        header->leafnames = (char *)*bufferBase;
        *bufferBase = (void *)((int)*bufferBase + header->leafnamesize);

        NuFileRead(file, header->leafnames, header->leafnamesize);
        for (int32_t k = 0; k < header->treeCount; k = k + 1) {
            header->filetree[k].name += (size_t)header->leafnames;
        }

        header->filetree->name = NULL;
        *bufferBase = (void *)((int)*bufferBase - header->leafnamesize);
        *bufferBase = (void *)((int)*bufferBase + header->treeCount * -0xc);
        header->filetree = NULL;
        header->leafnames = NULL;
        header->hashes = NULL;
        header->count3 = 0;
        header->count4 = 0;
        header->arr4 = NULL;

        if (header->version < -1) {
            header->hashes = (uint *)((int)*bufferBase + 0x1fU & 0xffffffe0);
            *bufferBase = (void *)((int)*bufferBase + 0x1fU & 0xffffffe0);
            *bufferBase = (void *)((int)*bufferBase + header->filesCount * 4);
            NuFileRead(file, header->hashes, header->filesCount << 2);
            for (int32_t k = 0; k < header->filesCount; k = k + 1) {
                APIEndianSwap(header->hashes + k, 1, 4);
            }
            NuFileRead(file, &header->count3, 4);
            APIEndianSwap(&header->count3, 1, 4);
            NuFileRead(file, &header->count4, 4);
            APIEndianSwap(&header->count4, 1, 4);
            header->arr4 = (char *)((int)*bufferBase + 0x1fU & 0xffffffe0);
            *bufferBase = (void *)((int)*bufferBase + 0x1fU & 0xffffffe0);
            *bufferBase = (void *)((int)*bufferBase + header->count4);
            NuFileRead(file, header->arr4, header->count4);
            char *str = header->arr4;
            for (int32_t k = 0; k < header->count3; k = k + 1) {
                len = NuStrLen(str);
                str = str + len + 1;
                if (((uint)str & 1) != 0) {
                    str = str + 1;
                }
                APIEndianSwap(str, 1, 2);
                str = str + 2;
            }
        }

        for (int32_t k = 0; k < 20; k = k + 1) {
            header->openFiles[k].field1_0x4 = -1;
            header->openFiles[k].file = 0;
            header->openFiles[k].position.i[0] = 0;
            header->openFiles[k].position.i[1] = 0;
        }

        header->openFiles[0].file = file;
        header->openFiles[0].position.l = 0;
        header->mode = mode;

        if (-3 < header->version) {
            uint uVar3;
            nudathdr_struct1 *puVar4;
            nudathdr_struct1 *puVar5;
            int uVar1;
            int32_t uVar2 = 0;
            int32_t uVar7 = 0;

            for (int32_t i = 0; i < header->filesCount - 1; i++) {
                int32_t index = i;
                int32_t hash = header->hashes[i];

                for (int32_t j = i; j < header->filesCount; j++) {
                    if (header->hashes[j] <= hash) {
                        hash = header->hashes[j];
                        index = j;
                    }
                }

                if (i != index) {
                    uVar3 = header->hashes[i];
                    header->hashes[i] = header->hashes[index];
                    header->hashes[index] = uVar3;
                    puVar5 = header->finfo + i;
                    len = puVar5->field0_0x0;
                    file = puVar5->field1_0x4;
                    uVar1 = puVar5->size;
                    uVar2 = puVar5->compressionMode;
                    puVar4 = header->finfo + i;
                    puVar5 = header->finfo + index;
                    puVar4->field0_0x0 = puVar5->field0_0x0;
                    puVar4->field1_0x4 = puVar5->field1_0x4;
                    puVar4->size = puVar5->size;
                    puVar4->compressionMode = puVar5->compressionMode;
                    puVar5 = header->finfo + index;
                    puVar5->field0_0x0 = len;
                    puVar5->field1_0x4 = file;
                    puVar5->size = uVar1;
                    puVar5->compressionMode = uVar2;
                }
            }
        }

        return header;
    } else {
        nufile_buffering_enabled = buffering;
        return NULL;
    }
}

char decode_buffer[0x40000] = {0};
int32_t decode_buffer_pos = 0;
int32_t decode_buffer_left = 0;
char read_buffer[0x40000] = {0};
int32_t read_buffer_size = 0;
int32_t read_buffer_decoded_size = 0;

int32_t NuDatFileFindTree(nudathdr_s *header, const char *name) {
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

void NuDatFileClose(NuFileHandle file) {
    file = file + -0x800;
    if (-1 < dat_file_infos[file].index) {
        (dat_file_infos[file].ptr)->openFiles[dat_file_infos[file].index].field1_0x4 = -1;
    }
    dat_file_infos[file].used = 0;
}

int32_t NuDatFileLoadBuffer(nudathdr_s *dat, const char *name, void *dest, int32_t maxSize) {
    LOG("dat=%p name=%s dest=%p maxSize=%d", dat, name, dest, maxSize);

    int32_t file;
    int32_t iVar1;
    int64_t lVar2;
    int32_t size;

    nufile_lasterror = 0;
    file = NuDatFileOpen(dat, name, NUFILE_OPENMODE_READ);
    if (file != 0) {
        if (dat->mode == 3) {
            do {
                iVar1 = NuFileStatus(file);
            } while (iVar1 != 0);
        }
        if (dat_file_infos[file + -0x800].compressionMode == 0) {
            size = dat_file_infos[file + -0x800].field3_0x14;
        } else {
            size = dat_file_infos[file + -0x800].field4_0x18;
        }
        if ((size <= maxSize) && (size != 0)) {
            while (iVar1 = NuDatFileRead(file, dest, size), iVar1 < 0) {
                do {
                    lVar2 = NuDatFileSeek(file, 0, 0, 0);
                } while (lVar2 < 0);
            }
            if (dat->mode == 3) {
                do {
                    iVar1 = NuFileStatus(file);
                } while (iVar1 != 0);
            }
            NuFileClose(file);
            return size;
        }

        if (size != 0) {
            nufile_lasterror = -1;
        }

        NuFileClose(file);
    }
    return 0;
}

NuFileHandle NuDatFileOpen(nudathdr_s *header, const char *name, NuFileOpenMode mode) {
    LOG("header=%p name=%s mode=%d", header, name, mode);

    int iVar1;
    int index;
    int fileIndex;
    longlong pos;
    longlong lVar2;
    int highPos;

    if ((((mode == NUFILE_OPENMODE_READ) && (iVar1 = NuDatFileFindTree(header, name), -1 < iVar1)) &&
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
                lVar2 = NuFileSeek(header->openFiles[fileIndex].file, dat_file_infos[index].pos.l, 0);
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

int32_t NuDatFileRead(NuFileHandle file, void *dest, int32_t size) {
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
            NuFileSeek(pnVar1->openFiles[index].file, dat_file_infos[fileIndex].pos2.l, 0);
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

int64_t NuDatFileSeek(NuFileHandle file, uint32_t param_2, int32_t param_3, int32_t param_4) {
    int iVar1;
    nudathdr_s *pnVar2;
    int iVar3;
    uint uVar4;
    int iVar5;
    uint uVar6;
    int64_t lVar7;
    int local_24;
    int local_20;

    iVar1 = file + -0x800;
    pnVar2 = dat_file_infos[iVar1].ptr;
    iVar3 = dat_file_infos[iVar1].index;
    if (param_4 == 1) {
        local_24 = param_2 + dat_file_infos[iVar1].pos2.u[0];
        local_20 = param_3 + dat_file_infos[iVar1].pos2.i[1] + (uint)CARRY4(param_2, dat_file_infos[iVar1].pos2.u[0]);
    } else if (param_4 == 2) {
        uVar4 = dat_file_infos[iVar1].field3_0x14;
        uVar6 = uVar4 + dat_file_infos[iVar1].pos.u[0];
        local_24 = uVar6 - param_2;
        local_20 = ((((int)uVar4 >> 0x1f) + dat_file_infos[iVar1].pos.i[1] +
                     (uint)CARRY4(uVar4, dat_file_infos[iVar1].pos.u[0])) -
                    param_3) -
                   (uint)(uVar6 < param_2);
    } else {
        local_24 = param_2 + dat_file_infos[iVar1].pos.u[0];
        local_20 = param_3 + dat_file_infos[iVar1].pos.i[1] + (uint)CARRY4(param_2, dat_file_infos[iVar1].pos.u[0]);
    }
    lVar7 = NuFileSeek(pnVar2->openFiles[iVar3].file, CONCAT44(local_20, local_24), 0);
    dat_file_infos[iVar1].pos2.l = lVar7;
    iVar5 = dat_file_infos[iVar1].pos2.i[1];
    pnVar2->openFiles[iVar3].position.i[0] = dat_file_infos[iVar1].pos2.i[0];
    pnVar2->openFiles[iVar3].position.i[1] = iVar5;
    return dat_file_infos[iVar1].pos2.l;
}