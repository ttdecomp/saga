#include "nu2api.saga/nufile/android/nufile_android.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "decomp.h"
#include "lostandfound/tmclient.h"
#include "nu2api.saga/nuandroid/nuios.h"
#include "nu2api.saga/nucore/nuapi.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nuthread/nuthread.h"

NuFileAndroidAPK::NuFileAndroidAPK(const char *filepath, NuFile::OpenMode::T mode)
    : NuFileBase(filepath, mode, 0x534f2020) {
}

NuFileAndroidAPK::~NuFileAndroidAPK() {
    Close();
}

i64 NuFileAndroidAPK::Seek(i64 offset, NuFile::SeekOrigin::T) {
}

isize NuFileAndroidAPK::Read(void *buf, usize size) {
}

isize NuFileAndroidAPK::Write(const void *buf, usize size) {
    return 0;
}

void NuFileAndroidAPK::Close() {
}

SAGA_NOMATCH i32 NuFileAndroidAPK::OpenFile(const char *filepath, NuFile::OpenMode::T mode) {
    UNIMPLEMENTED("android specific");
}

SAGA_NOMATCH i32 NuFileAndroidAPK::CloseFile(NUFILE file) {
    UNIMPLEMENTED("android specific");
}

SAGA_NOMATCH i64 NuFileAndroidAPK::SeekFile(NUFILE file, i64 offset, NuFile::SeekOrigin::T mode) {
    UNIMPLEMENTED("android specific");
}

SAGA_NOMATCH i32 NuFileAndroidAPK::ReadFile(NUFILE file, void *buf, u32 size) {
    UNIMPLEMENTED("android specific");
}

SAGA_NOMATCH i64 NuFileAndroidAPK::GetFilePos(NUFILE file) {
    UNIMPLEMENTED("android specific");
}

SAGA_NOMATCH i64 NuFileAndroidAPK::GetFileSize(NUFILE file) {
    UNIMPLEMENTED("android specific");
}

static FILE *g_fileHandles[32] = {NULL};

i32 NuPSFileRead(NUPSFILE index, void *dest, i32 len) {
    return fread(dest, 1, len, g_fileHandles[index]);
}

i32 NuPSFileWrite(NUPSFILE index, const void *src, i32 len) {
    LOG_DEBUG("index=%d, src=%p, len=%zu", index, src, len);

    FILE **files = g_fileHandles;
    return fwrite(src, 1, len, files[index]);
}

NUPSFILE NuPSFileOpen(char *filepath, NUFILEMODE mode) {
    char path[0x400];
    NUPSFILE ps_file;
    FILE *file;

    if (mode == NUFILE_MODE_CNT) {
        return -1;
    }

    memset(path, 0, sizeof(path));
    NuStrCpy(path, filepath);

    for (char *cursor = path; *cursor != '\0'; cursor++) {
        if (*cursor == '\\') {
            *cursor = '/';
        }
    }

    ps_file = NuGetFileHandlePS();

    file = NULL;
    switch (mode) {
        case NUFILE_READ:
            file = fopen(path, "rb");
            break;
        case NUFILE_WRITE:
            file = fopen(path, "wb");
            break;
        case NUFILE_APPEND:
            file = fopen(path, "ab+");
            break;
        default:
            return -1;
    }

    if (file == NULL) {
        return -1;
    }

    g_fileHandles[ps_file] = file;

    LOG_DEBUG("Opened file %s with index %d", path, ps_file);
    return ps_file;
}

NUPSFILE NuGetFileHandlePS(void) {
    for (i32 i = 0; i < 32; i++) {
        if (g_fileHandles[i] == NULL) {
            return i;
        }
    }

    return -1;
}

i32 NuPSFileClose(NUPSFILE index) {
    fclose(g_fileHandles[index]);
    g_fileHandles[index] = NULL;
    return 1;
}

i64 NuPSFileLSeek(NUPSFILE index, i64 offset, NUFILESEEK seekMode) {
    LOG_DEBUG("file=%d, offset=0x%llx, seekMode=%d", index, offset, seekMode);

    i32 whence = 0;

    switch (seekMode) {
        case NUFILE_SEEK_START:
            whence = SEEK_SET;
            break;
        case NUFILE_SEEK_CURRENT:
            whence = SEEK_CUR;
            break;
        case NUFILE_SEEK_END:
            whence = SEEK_END;
            break;
    }

    i64 value = 0;

    if (fseek(g_fileHandles[index], offset, whence) == 0) {
        value = ftell(g_fileHandles[index]);
    }

    return value;
}

void NuPSFileInitDevices(int device_id, int reboot_iop, int eject) {
    if (nuapi_use_target_manager) {
        the_tm_client = new TMClient(nuapi_use_target_manager, nuapi_target_manager_mac_address);
    }

    read_critical_section = NuThreadCreateCriticalSection();
    chdir(NuIOS_GetAppBundlePath());
}
