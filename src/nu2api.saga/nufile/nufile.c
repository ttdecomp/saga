#include "nu2api.saga/nufile/nufile.h"

#include "nu2api.saga/nustr/nustr.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include "decomp.h"

int NuFileOpen(const char *path, OpenMode mode) {
    return NuFileOpenDF(path, mode, curr_dat);
}

int NuFileOpenDF(const char *path, OpenMode mode, nudathdr_s *header) {
    int fileId;

    NuFileDevice *device = NuFileGetDeviceFromPath(path);

    if (device == (NuFileDevice *)0x0) {
        if (mode != MODE_WRITE && mode != MODE_APPEND) {
            fileId = 0;
            if (header == (nudathdr_s *)0x0) {
                UNIMPLEMENTED();
                // if (g_apkFileDevice != 0) {
                // fileId = NuFileAndroidAPK::OpenFile(path, 0);
                // }
            } else {
                fileId = NuDatFileOpen(header, path, mode);
            }
            if (0 < fileId) {
                return fileId;
            }
        }
        device = default_device;
    }

    char buf[256];

    //(*(code *)device->openFunc)(device, buf, path, 0x100);

    if (device->field0_0x0 == 2) {
        NuStrCat(buf, ";1");
    }

    /*

    if (device->field0_0x0 == 1) {
        local_1c = filename[2] + -0x30;
        local_20 = filename[3] + -0x30;
        index = NuMcOpen(local_1c, local_20, filename + *(int *)&device->field_0x48, mode, 0);
        if (index < 0) {
            index = 0;
        } else {
            index = index + 0x1000;
        }
    } else {
        fileIndex = NuPSFileOpen(buf, mode);
        if (fileIndex < 0) {
            index = 0;
        } else {
            index = fileIndex;
            memset(file_info.field31_0x28 + fileIndex * 0xe + -10, 0, 0x38);
            lVar2 = CONCAT44(local_2c._4_4_, (int)local_2c);
            file_info.field31_0x28[fileIndex * 0xe + -10] = index;
            file_info.field31_0x28[fileIndex * 0xe + 2] = mode;
            if (mode == 1) {
                file_info.field31_0x28[fileIndex * 0xe + -5] = 0;
                file_info.field31_0x28[fileIndex * 0xe + -4] = 0;
            } else if (index < 0x11) {
                do {
                    local_2c = lVar2;
                    lVar2 = NuFileSeek(fileIndex + 1, 0, 0, 2);
                } while (lVar2 < 0);
                *(long long *)(file_info.field31_0x28 + fileIndex * 0xe + -5) = lVar2;
                local_2c = lVar2;
                if (mode == 3) {
                    do {
                        iVar1 = NuFileStatus(fileIndex + 1);
                        lVar2 = local_2c;
                    } while (iVar1 != 0);
                }
                do {
                    local_2c = lVar2;
                    lVar2 = NuFileSeek(fileIndex + 1, 0, 0, 0);
                } while (lVar2 < 0);
                local_2c = lVar2;
                if (mode == 3) {
                    do {
                        iVar1 = NuFileStatus(fileIndex + 1);
                    } while (iVar1 != 0);
                }
            }
            if (mode == 0) {
                file_info.field31_0x28[fileIndex * 0xe] = nufile_buffering_enabled;
            }
            index = fileIndex + 1;
        }
    }
    */
}

NuFileDevice *NuFileGetDeviceFromPath(const char *path) {
    int i;

    for (i = 0; i < 8 && (path[i] != ':'); i++) {
    }

    if (i < 8) {
        for (int index = 0; index < _numdevices; index++) {
            if (NuStrNICmp(path, index * 0x234 + 0x6c93b4, devices[index].length) == 0) {
                return &devices[index];
            }
        }
    }

    return NULL;
}

unsigned int NameToHash(const char *name) {
    unsigned int hash = 0x811c9dc5;

    for (; *name != 0; name = name + 1) {
        hash = (hash ^ (int)*name) * 0x199933;
    }

    return hash;
}

int BinarySearch(int element, int *array, int length) {
    int end = length - 1;
    int start = 0;

    while (1) {
        if (end < start) {
            return -1;
        }

        int index = (end + start) / 2;
        if (array[index] == element) {
            return index;
        } else if (array[index] < element) {
            start = index + 1;
        } else {
            end = index - 1;
        }
    }
}

int NuDatFileGetFreeInfo(void) {
    NuThreadCriticalSectionBegin(file_criticalsection);

    int found = -1;
    for (int i = 0; i < 20; i++) {
        if (dat_file_infos[i].used == 0) {
            dat_file_infos[i].used = 1;
            found = i;
            break;
        }
    }

    NuThreadCriticalSectionEnd(file_criticalsection);
    return found;
}

int NuPSFileRead(int index, void *dest, int len) {
    return fread(dest, 1, len, g_fileHandles[index]);
}

int NuMemFileRead(int file, char *dest, int size) {
    if (file < 0x800) {
        int i = file - 0x400;
        int remaining = ((int)memfiles[i].end - (int)memfiles[i].ptr) + 1;

        if (remaining <= size) {
            size = remaining;
        }

        if (size != 0) {
            memcpy(dest, memfiles[i].ptr, size);
            memfiles[i].ptr = (void *)((int)memfiles[i].ptr + size);
        }

        return size;
    } else {
        return NuDatFileRead(file, dest, size);
    }
}