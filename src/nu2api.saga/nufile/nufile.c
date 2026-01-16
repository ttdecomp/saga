#include "nu2api.saga/nufile/nufile.h"

#include "nu2api.saga/nucore/nustring.h"

#include "decomp.h"

NuFileDevice host_device = {
    .pathSeparator = '/',
    .formatNameFunc = (void *)DEV_FormatName,
};

NuFileDevice *default_device = &host_device;

static NUDATHDR *curr_dat = NULL;

int32_t numdevices = 0;

NuFileDevice devices[16] = {0};

int32_t file_criticalsection;
nudatfileinfo_s dat_file_infos[20];
FILE *g_fileHandles[32] = {NULL};
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

NUFILE NuFileOpen(const char *path, NUFILEMODE mode) {
    return NuFileOpenDF(path, mode, curr_dat, 0);
}

int32_t NuFileStatus(NUFILE file) {
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

NUFILE NuFileOpenDF(const char *path, NUFILEMODE mode, NUDATHDR *header, int32_t _unused) {
    LOG_DEBUG("path=%s, mode=%d, header=%p", path, mode, header);

    NuFileDevice *device = NuFileGetDeviceFromPath(path);
    NUFILE file;

    if (device == NULL) {
        if (mode != 1 && mode != 2) {
            file = 0;

            if (header == NULL) {
                // if (g_apkFileDevice != (NuFileDeviceAndroidAPK *)0x0) {
                // fileId = NuFileAndroidAPK::OpenFile(path, 0);
                //}
                // UNIMPLEMENTED("android specific");
                file = NUFILE_PS(NuPSFileOpen(path, mode));
            } else {
                file = NuDatFileOpen(header, path, mode);
            }

            if (file > 0) {
                return file;
            }
        }

        device = default_device;
    }

    char buf[256];

    typedef void (*formatFuncType)(NuFileDevice *, char *, const char *, int32_t);
    (*(formatFuncType)device->formatNameFunc)(device, buf, path, 0x100);

    if ((int32_t)device->vtable == 2) {
        NuStrCat(buf, ";1");
    }

    int index;

    if ((int32_t)device->vtable == 1) {
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
                    size = NuFileSeek(fileIndex + 1, 0, NUFILE_SEEK_END);
                } while (size < 0);

                file_info[fileIndex].size.l = size;

                if (mode == 3) {
                    while (NuFileStatus(fileIndex + 1) != 0) {
                    }
                }

                do {
                    size = NuFileSeek(fileIndex + 1, 0, NUFILE_SEEK_START);
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
    LOG_DEBUG("path=%s", path);

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

    LOG_DEBUG("No device found for path=%s", path);

    return NULL;
}

uint32_t NuFileOpenSize(NUFILE file) {
    LOG_DEBUG("file=%d", file);

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
                size = NuDatFileOpenSize(file);
            }
        } else {
            // size = NuMcFileOpenSize(file);
            UNIMPLEMENTED("memory card specific");
        }
    } else {
        // size = NuFileAndroidAPK::GetFileSize(file);
        UNIMPLEMENTED("android specific");
    }

    LOG_DEBUG("size=%u", size);

    return size;
}

int32_t NuFileRead(NUFILE file, void *dest, int32_t length) {
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

NUDATHDR *NuDatSet(NUDATHDR *header) {
    NUDATHDR *dat = curr_dat;
    curr_dat = header;
    return dat;
}

int32_t NuFileLoadBuffer(const char *name, void *dest, int32_t size) {
    int j;
    int iVar2;
    longlong lVar1;
    int i;

    nufile_lasterror = 0;
    i = 0;
    if ((curr_dat == NULL) || (i = NuDatFileLoadBuffer(curr_dat, name, dest, size), nufile_lasterror != -1)) {
        if (i == 0) {
            j = NuFileExists(name);
            if (j == 0) {
                nufile_lasterror = -2;
            } else {
                j = NuFileOpen(name, NUFILE_MODE_READ);
                if (j == 0) {
                    nufile_lasterror = -3;
                } else {
                    if (nufile_try_packed == 0) {
                        i = NuFileOpenSize(j);
                        if ((size < i) || (i == 0)) {
                            nufile_lasterror = -1;
                            i = 0;
                        } else {
                            while (iVar2 = NuFileRead(j, dest, i), iVar2 < 0) {
                                do {
                                    lVar1 = NuFileSeek(j, 0, 0);
                                } while (lVar1 < 0);
                            }
                        }
                    } else {
                        // i = NuPPLoadBuffer(j, dest, size);
                        UNIMPLEMENTED("PP");
                    }
                    NuFileClose(j);
                }
            }
        }
    } else {
        i = 0;
    }

    return i;
}

int32_t NuFileExists(const char *name) {
    LOG_DEBUG("name=%s", name);
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
            (file = NuFileOpen(name, NUFILE_MODE_READ), pos = -1, file != 0)) {
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

uint64_t NuFilePos(NUFILE file) {
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

uint8_t DEV_FormatName(NuFileDevice *device, char *dest, char *path, int length) {
    LOG_DEBUG("device=%p, dest=%p, path=%p, length=%d", device, dest, path, length);

    int n;
    int iVar1;
    int len;
    char *prefix;
    char buf[512];
    char sep;

    prefix = device->path;
    n = NuStrLen(prefix);
    iVar1 = NuStrNICmp(path, prefix, n);
    if (iVar1 == 0) {
        iVar1 = NuStrLen(prefix);
        sep = path[iVar1];
        if ((sep == '/') || (sep == '\\')) {
            NuStrCpy(buf, path);
        } else {
            NuStrCpy(buf, device->field37_0x4c);
            NuStrCat(buf, device->field39_0xac);
            NuStrCat(buf, path + iVar1);
        }
    } else {
        NuStrCpy(buf, device->field37_0x4c);
        NuStrCat(buf, device->field39_0xac);
        NuStrCat(buf, path);
    }
    iVar1 = NuStrLen(device->field37_0x4c);
    NuFileCorrectSlashes(device, buf + iVar1);
    NuFileReldirFix(device, buf);
    len = NuStrLen(buf);
    if (len < length) {
        NuStrCpy(dest, buf);
    }

    LOG_DEBUG("Formatted path: %s, returning %d", buf, len < length);

    return len < length;
}

int8_t NuFileReadChar(NUFILE file) {
    int8_t value = 0;
    NuFileRead(file, &value, sizeof(int8_t));
    return value;
}

int32_t NuFileReadDir(NUFILE file) {
    int32_t value;
    NuFileRead(file, &value, sizeof(int32_t));
    return value;
}

float NuFileReadFloat(NUFILE file) {
    float value;
    NuFileRead(file, &value, sizeof(float));
    return value;
}

int32_t NuFileReadInt(NUFILE file) {
    int32_t value;
    NuFileRead(file, &value, sizeof(int32_t));
    return value;
}

int16_t NuFileReadShort(NUFILE file) {
    int16_t value;
    NuFileRead(file, &value, sizeof(int16_t));
    return value;
}

uint8_t NuFileReadUnsignedChar(NUFILE file) {
    uint8_t value = 0;
    NuFileRead(file, &value, sizeof(uint8_t));
    return value;
}

uint32_t NuFileReadUnsignedInt(NUFILE file) {
    uint32_t value;
    NuFileRead(file, &value, sizeof(uint32_t));
    return value;
}

uint16_t NuFileReadUnsignedShort(NUFILE file) {
    uint16_t value;
    NuFileRead(file, &value, sizeof(uint16_t));
    return value;
}

uint16_t NuFileReadWChar(NUFILE file) {
    int16_t value = 0;
    NuFileRead(file, &value, sizeof(uint16_t));
    return value;
}
