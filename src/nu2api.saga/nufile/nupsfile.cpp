#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include <cstring>

#include "decomp.h"

int32_t NuPSFileRead(NUPSFILE index, void *dest, int32_t len) {
    LOG_DEBUG("index=%d, dest=%p, len=%zu", index, dest, len);

    return fread(dest, 1, len, g_fileHandles[index]);
}

int32_t NuPSFileWrite(NUPSFILE index, const void *src, int32_t len) {
    LOG_DEBUG("index=%d, src=%p, len=%zu", index, src, len);

    FILE **files = g_fileHandles;
    return fwrite(src, 1, len, files[index]);
}

NUPSFILE NuPSFileOpen(const char *name, NUFILEMODE mode) {
    LOG_DEBUG("name=%s, mode=%d", name, mode);

    char path[1024];

    memset(path, 0, sizeof(path));
    NuStrCpy(path, name);

    if (mode != 5) {
        for (char *c = path; *c != '\0'; c++) {
            if (*c == '\\') {
                *c = '/';
            }
        }

        NUPSFILE i = NuGetFileHandlePS();

        FILE *file = NULL;

        if (mode == NUFILE_MODE_READ) {
            file = fopen(path, "rb");
        } else if (mode == NUFILE_MODE_WRITE) {
            file = fopen(path, "wb");
        } else if (mode == NUFILE_MODE_APPEND) {
            file = fopen(path, "ab+");
        } else {
            return -1;
        }

        if (file != NULL) {
            g_fileHandles[i] = file;

            LOG_DEBUG("Opened file %s with index %d", path, i);
            return i;
        }
    }

    return -1;
}

NUPSFILE NuGetFileHandlePS(void) {
    for (int32_t i = 0; i < 32; i++) {
        if (g_fileHandles[i] == NULL) {
            return i;
        }
    }

    return -1;
}

int32_t NuPSFileClose(NUPSFILE index) {
    fclose(g_fileHandles[index]);
    g_fileHandles[index] = NULL;
    return 1;
}

int64_t NuPSFileLSeek(NUPSFILE index, int64_t offset, NUFILESEEK seekMode) {
    LOG_DEBUG("file=%d, offset=0x%llx, seekMode=%d", index, offset, seekMode);

    int32_t whence = 0;

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

    int64_t value = 0;

    if (fseek(g_fileHandles[index], offset, whence) == 0) {
        value = ftell(g_fileHandles[index]);
    }

    return value;
}
