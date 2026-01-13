#include "nu2api.saga/nufile/nufile.h"

#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include <stddef.h>
#include <string.h>

#include "decomp.h"

NUDATHDR *NuDatOpen(char *name, void **bufferBase, int32_t zero) {
    LOG_DEBUG("name=%s bufferBase=%p zero=%d", name, bufferBase, zero);
    return NuDatOpenEx(name, bufferBase, zero, 0);
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

NUDATHDR *NuDatOpenEx(char *name, void **bufferBase, int zero, short mode) {
    LOG_DEBUG("name=%s bufferBase=%p zero=%d mode=%d", name, bufferBase, zero, mode);

    int32_t buffering = nufile_buffering_enabled;

    NUFILE file = NuFileOpenDF(name, (int32_t)mode, NULL, 0);
    if (file != 0) {
        int32_t size = NuFileOpenSize(file);

        int32_t buffer[2];
        NuFileRead(file, buffer, 8);
        APIEndianSwap(buffer, 2, 4);

        int64_t offset = (int64_t)buffer[1] << 32 | (uint32_t)buffer[0];
        NuFileSeek(file, offset, 0);

        int32_t len = NuStrLen(name);

        NUDATHDR *header = BUFFER_ALLOC(bufferBase, NUDATHDR);
        memset(header, 0, 0x178);

        header->field300_0x16c = 1;
        header->path = (char *)*bufferBase;
        *bufferBase = (void *)((int)*bufferBase + (len + 0x10U & 0xfffffff0));
        NuStrCpy(header->path, name);

        NuFileRead(file, header, 4);
        APIEndianSwap(header, 1, 4);

        NuFileRead(file, &header->filesCount, 4);
        APIEndianSwap(&header->filesCount, 1, 4);

        header->finfo = BUFFER_ALLOC_ARRAY(bufferBase, header->filesCount, struct nudathdr_struct1);
        NuFileRead(file, header->finfo, header->filesCount * sizeof(struct nudathdr_struct1));

        for (int32_t i = 0; i < header->filesCount; i = i + 1) {
            APIEndianSwap(header->finfo + i, 1, 4);
            APIEndianSwap(&header->finfo[i].field1_0x4, 1, 4);
            APIEndianSwap(&header->finfo[i].size, 1, 4);
        }

        NuFileRead(file, &header->treeCount, 4);
        APIEndianSwap(&header->treeCount, 1, 4);
        header->filetree = BUFFER_ALLOC_ARRAY(bufferBase, header->treeCount, struct nudathdr_struct2);

        if (header->version < -4) {
            NuFileRead(file, header->filetree, header->treeCount * sizeof(struct nudathdr_struct2));
        } else {
            NuFileRead(file, header->filetree, header->treeCount * 8);
            struct nudathdr_struct2 *struct2 = header->filetree;
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
            struct nudathdr_struct1 *puVar4;
            struct nudathdr_struct1 *puVar5;
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
