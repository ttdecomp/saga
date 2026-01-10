#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include <stddef.h>
#include <string.h>

#include "decomp.h"

nudathdr_s *curr_dat = NULL;

NuFileDevice host_device = {
    .pathSeparator = '/',
    .formatNameFunc = (void *)DEV_FormatName,
};

NuFileDevice *default_device = &host_device;

int32_t numdevices = 0;

NuFileDevice devices[16] = {0};

int32_t file_criticalsection;
nudatfileinfo_s dat_file_infos[20];
FILE *g_fileHandles[32] = {NULL};
NuMemFile memfiles[16];
int32_t nufile_buffering_enabled;
fileinfo_s file_info[32];

void NuFileCorrectSlashes(NuFileDevice *device, char *path) {
    char sep;

    if (device == (NuFileDevice *)0x0) {
        sep = '\\';
    } else {
        sep = device->pathSeparator;
    }
    for (; *path != '\0'; path = path + 1) {
        if ((*path == '\\') || (*path == '/')) {
            *path = sep;
        }
    }
    return;
}

void NuFileReldirFix(NuFileDevice *device, char *path) {
    char *pcVar1;
    char *ptr;
    char *ptr2;

    ptr = path;
    ptr2 = path;
    do {
        if (*ptr2 == '\0') {
            *ptr = '\0';
            return;
        }
        if ((((*ptr2 == device->pathSeparator) && (ptr2[1] == '.')) && (ptr2[2] == '.')) &&
            (ptr2[3] == device->pathSeparator)) {
            ptr = ptr2;
            pcVar1 = ptr;
            do {
                ptr = pcVar1;
                if (ptr <= path)
                    goto LAB_00268be2;
                pcVar1 = ptr + -1;
                if (*pcVar1 == device->pathSeparator) {
                    ptr2 = ptr2 + 3;
                    ptr = pcVar1;
                    goto LAB_00268be2;
                }
            } while (*pcVar1 != ':');
            ptr2 = ptr2 + 4;
        }
    LAB_00268be2:
        *ptr = *ptr2;
        ptr = ptr + 1;
        ptr2 = ptr2 + 1;
    } while (1);
}

NuFileHandle NuFileOpen(const char *path, NuFileOpenMode mode) {
    return NuFileOpenDF(path, mode, curr_dat);
}

void NuFileClose(NuFileHandle file) {
    if (NUFILE_IS_PS(file)) {
        int32_t index = NUFILE_INDEX_PS(file);

        while (NuPSFileClose(index) < 0) {
        }

        if (file_info[index].buffer != NULL) {
            (file_info[index].buffer)->fileinfo = NULL;
        }

        memset(&file_info[index], 0, sizeof(fileinfo_s));
    } else if (NUFILE_IS_MEM(file) || NUFILE_IS_DAT(file)) {
        NuMemFileClose(file);
    } else if (NUFILE_IS_MC(file)) {
        // NuMcClose(NUFILE_INDEX_MC(file), 0);
        UNIMPLEMENTED("memory card specific");
    } else {
        // NuFileAndroidAPK::CloseFile(index);
        UNIMPLEMENTED("android specific");
    }
}

int32_t NuFileStatus(NuFileHandle file) {
    if (NUFILE_IS_NATIVE(file)) {
        while (1) {
        }
    }

    if (NUFILE_IS_MEM(file)) {
        return 0;
    }

    nudatfileinfo_s *fileInfo = &dat_file_infos[NUFILE_INDEX_DAT(file)];
    return NuFileStatus(fileInfo->ptr->openFiles[fileInfo[NUFILE_INDEX_DAT(file)].index].file);
}

NuFileHandle NuFileOpenDF(const char *path, NuFileOpenMode mode, nudathdr_s *header) {
    LOG("path=%s, mode=%d, header=%p", path, mode, header);

    NuFileDevice *device = NuFileGetDeviceFromPath(path);
    int fileId;

    if (device == NULL) {
        if (mode != 1 && mode != 2) {
            fileId = 0;

            if (header == NULL) {
                // if (g_apkFileDevice != (NuFileDeviceAndroidAPK *)0x0) {
                // fileId = NuFileAndroidAPK::OpenFile(path, 0);
                //}
                // UNIMPLEMENTED("android specific");
                fileId = NUFILE_PS(NuPSFileOpen(path, mode));
            } else {
                // fileId = NuDatFileOpen(header, path, mode);
                UNIMPLEMENTED();
            }

            if (fileId > 0) {
                return fileId;
            }
        }

        device = default_device;
    }

    char buf[256];

    typedef void (*formatFuncType)(NuFileDevice *, char *, const char *, int32_t);
    (*(formatFuncType)device->formatNameFunc)(device, buf, path, 0x100);

    if (device->vtable == 2) {
        NuStrCat(buf, ";1");
    }

    int index;

    if (device->vtable == 1) {
        UNIMPLEMENTED("memory card specific");
        int _local[2];
        /*local_1c = buf[2] + -0x30;
        local_20 = buf[3] + -0x30;
        index = NuMcOpen(local_1c, local_20, buf + device->someLength, mode, 0);
        if (index < 0) {
            index = 0;
        } else {
            index = index + 0x1000;
        }*/
    } else {
        int fileIndex = NuPSFileOpen(buf, mode);

        if (fileIndex < 0) {
            index = 0;
        } else {
            int64_t size;

            index = fileIndex;
            memset(&file_info[fileIndex], 0, sizeof(fileinfo_s));

            file_info[fileIndex].file = index;
            file_info[fileIndex].mode = mode;

            if (mode == 1) {
                file_info[fileIndex].size.i[0] = 0;
                file_info[fileIndex].size.i[1] = 0;
            } else if (index < 17) {
                do {
                    size = NuFileSeek(fileIndex + 1, 0, 2);
                } while (size < 0);

                file_info[fileIndex].size.l = size;

                if (mode == 3) {
                    while (NuFileStatus(fileIndex + 1) != 0) {
                    }
                }

                do {
                    size = NuFileSeek(fileIndex + 1, 0, 0);
                } while (size < 0);

                if (mode == 3) {
                    while (NuFileStatus(fileIndex + 1) != 0) {
                    }
                }
            }

            if (mode == 0) {
                file_info[fileIndex].offset.i[1] = nufile_buffering_enabled;
            }

            index = fileIndex + 1;
        }
    }

    return index;
}

NuFileDevice *NuFileGetDeviceFromPath(const char *path) {
    LOG("path=%s", path);

    int i;
    for (i = 0; (i < 8 && (path[i] != ':')); i = i + 1) {
    }

    if (i < 8) {
        for (int32_t index = 0; index < numdevices; index++) {
            if (NuStrNICmp(path, devices[index].path, devices[index].length) == 0) {
                return &devices[index];
            }
        }
    }

    LOG("No device found for path=%s", path);

    return NULL;
}

size_t NuMemFileRead(NuFileHandle file, char *dest, size_t size) {
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
        // return NuDatFileRead(file, dest, size);
        UNIMPLEMENTED();
    }
}

int32_t NuFileSeek(NuFileHandle file, int64_t offset, int32_t seekMode) {
    LOG("file=%d, offset=%lld, seekMode=%d", file, offset, seekMode);

    int iVar1;
    uint uVar2;
    longlong lVar4;
    T in_stack_ffffffb4;
    undefined4 local_20;

    if (file < 0x2000) {
        if (file < 0x1000) {
            if (file < 0x400) {
                int index = NUFILE_INDEX_PS(file);
                if (file_info[index].offset.i[1] == 0) {
                    lVar4 = NuPSFileLSeek(NUFILE_INDEX_PS(file), offset, seekMode);
                } else {
                    if (seekMode == 1) {
                        file_info[index].field1_0x4.l = offset + file_info[index].field1_0x4.l;
                    } else if (seekMode == 2) {
                        file_info[index].field1_0x4.l = offset - file_info[index].size.l;
                    } else {
                        file_info[index].field1_0x4.l = offset;
                    }
                    lVar4 = file_info[index].field1_0x4.l;
                }
            } else {
                // lVar4 = NuMemFileSeek(file, (uint)offset, (int)(uint)offset >> 31, seekMode);
                UNIMPLEMENTED();
            }
        } else {
            // iVar3 = NuMcSeek(file + -0x1000, (uint)offset, seekMode, 0);
            // lVar4 = (longlong)iVar3;
            UNIMPLEMENTED("memory card specific");
        }
    } else {
        if (seekMode == 1) {
            local_20 = 1;
        } else if (seekMode == 2) {
            local_20 = 2;
        } else {
            local_20 = 0;
        }
        // NuFileAndroidAPK::SeekFile((NuFileAndroidAPK *)file, (uint)offset, CONCAT44(local_20, offset._4_4_),
        // in_stack_ffffffb4);
        UNIMPLEMENTED("android specific");
    }
    return lVar4;
}

uint32_t NuFileOpenSize(NuFileHandle file) {
    LOG("file=%d", file);

    uint32_t size;

    if (file < 0x2000) {
        if (file < 0x1000) {
            if (file < 0x800) {
                if (file < 0x400) {
                    size = (uint32_t)file_info[NUFILE_INDEX_PS(file)].size.l;
                } else {
                    // size = NuMemFileOpenSize(file);
                    UNIMPLEMENTED();
                }
            } else {
                // size = NuDatFileOpenSize(file);
                UNIMPLEMENTED();
            }
        } else {
            // size = NuMcFileOpenSize(file);
            UNIMPLEMENTED("memory card specific");
        }
    } else {
        // size = NuFileAndroidAPK::GetFileSize(file);
        UNIMPLEMENTED("android specific");
    }

    LOG("size=%u", size);

    return size;
}

static int32_t CSWTCH_152[3] = {1, 2, 0};

int64_t NuPSFileLSeek(int32_t index, int64_t offset, int32_t seekMode) {
    LOG("file=%d, offset=%lld, seekMode=%d", index, offset, seekMode);

    int whence = 0;

    if (seekMode - 1U < 2) {
        whence = CSWTCH_152[seekMode - 1U];
    }

    long value = 0;
    int bit32 = 0;

    if (fseek(g_fileHandles[index], offset, whence) == 0) {
        value = ftell(g_fileHandles[index]);
        bit32 = value >> 31;
    }

    return CONCAT44(bit32, value);
}

size_t NuFileRead(NuFileHandle file, void *dest, size_t length) {
    int index;
    uint uVar1;
    fileinfo_s *info;
    uint uVar2;
    int iVar3;
    uint uVar4;
    char *local_24;
    int bytesRead;

    if (file < 0x2000) {
        if (file < 0x1000) {
            if (file < 0x400) {
                index = file + -1;
                info = file_info + index;
                if (file_info[index].offset.i[1] == 0) {
                    bytesRead = NuPSFileRead(index, dest, length);
                } else {
                    if (file_info[index].buffer == (FileBuffer *)0x0) {
                        // AquireFileBuffer(info);
                    }
                    bytesRead = 0;
                    local_24 = dest;
                    while (0 < (int)length) {
                        if (file_info[index].size.i[1] <= file_info[index].field1_0x4.i[1]) {
                            if (file_info[index].size.i[1] < file_info[index].field1_0x4.i[1]) {
                                return bytesRead;
                            }
                            if (file_info[index].size.u[0] <= file_info[index].field1_0x4.u[0]) {
                                return bytesRead;
                            }
                        }
                        if ((file_info[index].field1_0x4.i[1] < file_info[index].field6_0x20) ||
                            ((file_info[index].field1_0x4.i[1] <= file_info[index].field6_0x20 &&
                              (file_info[index].field1_0x4.u[0] < file_info[index].field5_0x1c)))) {
                        LAB_0026ae23:
                            if (file_info[index].field2_0xc != file_info[index].field1_0x4.i[0] ||
                                file_info[index].field1_0x4.i[1] != file_info[index].field3_0x10) {
                                NuPSFileLSeek(info->file, file_info[index].field1_0x4.l, 0);
                                uVar2 = file_info[index].field1_0x4.i[1];
                                file_info[index].field2_0xc = file_info[index].field1_0x4.i[0];
                                file_info[index].field3_0x10 = uVar2;
                            }
                            iVar3 = NuPSFileRead(info->file, (file_info[index].buffer)->field2_0x8, 0x400);
                            file_info[index].offset.i[0] = iVar3;
                            uVar2 = file_info[index].field3_0x10;
                            file_info[index].field5_0x1c = file_info[index].field2_0xc;
                            file_info[index].field6_0x20 = uVar2;
                            uVar2 = file_info[index].field2_0xc;
                            uVar4 = file_info[index].field3_0x10;
                            uVar1 = file_info[index].offset.u[0];
                            file_info[index].field2_0xc = uVar1 + uVar2;
                            file_info[index].field3_0x10 = ((int)uVar1 >> 0x1f) + uVar4 + (uint)CARRY4(uVar1, uVar2);
                        } else {
                            uVar2 = file_info[index].offset.u[0];
                            iVar3 = ((int)uVar2 >> 0x1f) + file_info[index].field6_0x20 +
                                    (uint)CARRY4(uVar2, file_info[index].field5_0x1c);
                            if ((iVar3 <= file_info[index].field1_0x4.i[1]) &&
                                ((iVar3 < file_info[index].field1_0x4.i[1] ||
                                  (uVar2 + file_info[index].field5_0x1c <= file_info[index].field1_0x4.u[0]))))
                                goto LAB_0026ae23;
                        }
                        uVar4 = file_info[index].offset.i[0] +
                                (file_info[index].field5_0x1c - file_info[index].field1_0x4.i[0]);
                        uVar2 = length;
                        if ((int)uVar4 <= (int)length) {
                            uVar2 = uVar4;
                        }
                        if (uVar2 != 0) {
                            memcpy(local_24,
                                   (void *)((int)file_info[index].buffer +
                                            (file_info[index].field1_0x4.i[0] - file_info[index].field5_0x1c) + 8),
                                   uVar2);
                        }
                        local_24 = local_24 + uVar2;
                        bytesRead = bytesRead + uVar2;
                        length = length - uVar2;
                        uVar4 = file_info[index].field1_0x4.u[0];
                        iVar3 = file_info[index].field1_0x4.i[1];
                        file_info[index].field1_0x4.i[0] = uVar2 + uVar4;
                        file_info[index].field1_0x4.i[1] = ((int)uVar2 >> 0x1f) + iVar3 + (uint)CARRY4(uVar2, uVar4);
                    }
                }
            } else {
                bytesRead = NuMemFileRead(file, dest, length);
            }
        } else {
            // bytesRead = NuMcRead(file + -0x1000, dest, length, 0);
        }
    } else {
        // bytesRead = NuFileAndroidAPK::ReadFile(file, dest, length);
    }
    return bytesRead;
}

int32_t nufile_lasterror = 0;
int32_t nufile_try_packed = 0;

int32_t NuFileLoadBuffer(const char *name, void *dest, int32_t size) {

    nufile_lasterror = 0;

    if (curr_dat == NULL) {
        return 0;
    }

    int32_t i = NuDatFileLoadBuffer(curr_dat, name, dest, size);
    if (nufile_lasterror != -1) {
        return 0;
    }

    if (i == 0) {
        int32_t j = NuFileExists(name);

        if (j == 0) {
            nufile_lasterror = -2;
        } else {
            j = NuFileOpen(name, NUFILE_OPENMODE_READ);
            if (j == 0) {
                nufile_lasterror = -3;
            } else {
                if (nufile_try_packed == 0) {
                    i = NuFileOpenSize(j);
                    if ((size < i) || (i == 0)) {
                        nufile_lasterror = -1;
                        i = 0;
                    } else {
                        while (NuFileRead(j, dest, i) < 0) {
                            while (NuFileSeek(j, 0, 0) < 0) {
                            }
                        }
                    }
                } else {
                    // i = NuPPLoadBuffer(j, dest, size);
                    UNIMPLEMENTED();
                }
                NuFileClose(j);
            }
        }
    }

    return i;
}

void NuMemFileClose(NuFileHandle file) {
    if (NUFILE_IS_PS(file)) {
        do {
        } while (1);
    }

    if (NUFILE_IS_MEM(file)) {
        memfiles[NUFILE_INDEX_MEM(file)].used = 0;
    } else if (NUFILE_IS_DAT(file)) {
        NuDatFileClose(file);
    }
}

int32_t NuFileExists(const char *name) {
    LOG("name=%s", name);
    return NuFileSize(name) > 0 ? 1 : 0;
}

uint64_t NuFileSize(const char *name) {
    int buffering = nufile_buffering_enabled;
    nufile_buffering_enabled = 0;

    int file;
    uint64_t pos;

    if (curr_dat == NULL || (file = NuDatFileFindTree(curr_dat, name), file < 0)) {
        pos = -1;
        if (((name != NULL) && (pos = -1, *name != '\0')) &&
            (file = NuFileOpen(name, NUFILE_OPENMODE_READ), pos = -1, file != 0)) {
            do {
                pos = NuFileSeek(file, 0, 2);
            } while (pos < 0);
            pos = NuFilePos(file);
            NuFileClose(file);
        }
    } else {
        buffering = nufile_buffering_enabled;
        pos = (uint64_t)curr_dat->finfo[file].size;
    }

    nufile_buffering_enabled = buffering;

    return pos;
}

uint64_t NuFilePos(NuFileHandle file) {
    longlong pos_;
    int local_14;

    if (file < 0x2000) {
        if (file < 0x1000) {
            if (file < 0x400) {
                file = file + -1;
                if (file_info[file].offset.i[1] == 0) {
                    do {
                        pos_ = NuPSFileLSeek(file, 0, 1);
                    } while (pos_ < 0);
                    local_14 = (int)pos_;
                } else {
                    local_14 = file_info[file].field1_0x4.i[0];
                }
            } else {
                local_14 = NuMemFilePos(file);
            }
        } else {
            // local_14 = NuMcSeek(file + -0x1000, 0, 2, 0);
            UNIMPLEMENTED("memory card specific");
        }
    } else {
        // local_14 = NuFileAndroidAPK::GetFilePos(index);
        UNIMPLEMENTED("android specific");
    }
    return local_14;
}

int32_t NuMemFilePos(NuFileHandle file) {
    if (file < 0x800) {
        return (int)memfiles[file + -0x400].ptr - (int)memfiles[file + -0x400].buffer;
    } else {
        return NuDatFilePos(file);
    }
}