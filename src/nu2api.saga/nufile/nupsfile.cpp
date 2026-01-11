#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include <cstring>

#include "decomp.h"

extern "C" size_t NuPSFileRead(int32_t index, void *dest, size_t len) {
    LOG("index=%d, dest=%p, len=%zu", index, dest, len);

    return fread(dest, 1, len, g_fileHandles[index]);
}

extern "C" size_t NuPSFileWrite(int32_t index, const void *src, size_t len) {
    LOG("index=%d, src=%p, len=%zu", index, src, len);

    FILE **files = g_fileHandles;
    return fwrite(src, 1, len, files[index]);
}

extern "C" NUPSFILE NuPSFileOpen(const char *name, NUFILEMODE mode) {
    LOG("name=%s, mode=%d", name, mode);

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

            LOG("Opened file %s with index %d", path, i);
            return i;
        }
    }

    return -1;
}

extern "C" NUPSFILE NuGetFileHandlePS(void) {
    for (int32_t i = 0; i < 32; i++) {
        if (g_fileHandles[i] == NULL) {
            return i;
        }
    }

    return -1;
}

extern "C" int32_t NuPSFileClose(NUPSFILE index) {
    fclose(g_fileHandles[index]);
    g_fileHandles[index] = NULL;
    return 1;
}