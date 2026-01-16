#include "nu2api.saga/nufile/nufile.h"

#include <string.h>

namespace NuFile {
    namespace SeekOrigin {
        typedef int32_t T;
    };
}; // namespace NuFile

class NuFileAndroidAPK {
  public:
    static SAGA_NOMATCH int32_t CloseFile(NUFILE file) {
        UNIMPLEMENTED("android specific");
    }

    SAGA_NOMATCH int64_t SeekFile(int64_t, NuFile::SeekOrigin::T) {
        UNIMPLEMENTED("android specific");
    }
};

void NuFileClose(NUFILE file) {
    if (NUFILE_IS_MEM(file) || NUFILE_IS_DAT(file)) {
        NuMemFileClose(file);
    } else if (NUFILE_IS_MC(file)) {
        NuMcClose(NUFILE_INDEX_MC(file), 0);
    } else if (NUFILE_IS_PS(file)) {
        int32_t index = NUFILE_INDEX_PS(file);

        while (NuPSFileClose(index) < 0) {
        }

        if (file_info[index].buffer != NULL) {
            file_info[index].buffer->fileinfo = NULL;
        }

        memset(&file_info[index], 0, sizeof(fileinfo_s));
    } else {
        NuFileAndroidAPK::CloseFile(file);
    }
}

int32_t NuFileSeek(NUFILE file, int64_t offset, NUFILESEEK seekMode) {
    LOG_DEBUG("file=%d, offset=0x%llx, seekMode=%d", file, offset, seekMode);

    if (NUFILE_IS_PS(file)) {
        int index = NUFILE_INDEX_PS(file);
        if (file_info[index].offset.i[1] == 0) {
            return NuPSFileLSeek(NUFILE_INDEX_PS(file), offset, seekMode);
        } else {
            if (seekMode == NUFILE_SEEK_CURRENT) {
                file_info[index].field1_0x4.l = offset + file_info[index].field1_0x4.l;
            } else if (seekMode == NUFILE_SEEK_END) {
                file_info[index].field1_0x4.l = offset - file_info[index].size.l;
            } else {
                file_info[index].field1_0x4.l = offset;
            }
            return file_info[index].field1_0x4.l;
        }
    } else if (NUFILE_IS_MEM(file) || NUFILE_IS_DAT(file)) {
        return NuMemFileSeek(file, offset, seekMode);
    } else if (NUFILE_IS_MC(file)) {
        return NuMcSeek(NUFILE_INDEX_MC(file), offset, seekMode);
    } else {
        NUFILESEEK whence;

        if (seekMode == NUFILE_SEEK_CURRENT) {
            whence = NUFILE_SEEK_CURRENT;
        } else if (seekMode == NUFILE_SEEK_END) {
            whence = NUFILE_SEEK_END;
        } else {
            whence = NUFILE_SEEK_START;
        }

        return ((NuFileAndroidAPK *)file)->SeekFile(offset, whence);
    }
}

int32_t NuFileLoadBufferVP(char *filepath, VARIPTR *buf, VARIPTR *buf_end) {
    int32_t len;

    len = NuFileLoadBuffer(filepath, buf, buf_end->char_ptr - buf->char_ptr);

    buf->char_ptr = buf->char_ptr + len;

    return len;
}

NUMEMFILE memfiles[20];

NUFILE NuMemFileOpen(void *buf, int buf_size, NUFILEMODE mode) {
    int i;

    if (buf_size > 0 && (mode == NUFILE_MODE_READ || mode == NUFILE_MODE_WRITE)) {
        for (i = 0; i < 20; i++) {
            if (!memfiles[i].used) {
                memfiles[i].buffer = (char *)buf;
                memfiles[i].end = (char *)buf + buf_size - 1;
                memfiles[i].ptr = memfiles[i].buffer;
                memfiles[i].mode = mode;
                memfiles[i].used = 1;

                return i + 0x400;
            }
        }
    }

    return 0;
}

void NuMemFileClose(NUFILE file) {
    if (file >= 0x2000) {
        do {
        } while (true);
    }

    if (file >= 0x800) {
        NuDatFileClose(file);
    } else {
        file -= 0x400;
        memfiles[file].used = 0;
    }
}

int NuMemFileRead(NUFILE file, void *buf, int size) {
    int remaining;

    if (file >= 0x800) {
        return NuDatFileRead(file, buf, size);
    }

    file -= 0x400;

    size = memfiles[file].end - memfiles[file].ptr + 1 <= size ? memfiles[file].end - memfiles[file].ptr + 1 : size;

    if (size != 0) {
        memcpy(buf, memfiles[file].ptr, size);

        memfiles[file].ptr = memfiles[file].ptr + size;
    }

    return size;
}

int NuMemFileWrite(NUFILE file, void *data, int size) {
    file -= 0x400;

    size = memfiles[file].end - memfiles[file].ptr + 1 <= size ? memfiles[file].end - memfiles[file].ptr + 1 : size;

    if (size != 0) {
        memcpy(memfiles[file].ptr, data, size);

        memfiles[file].ptr += size;
    }

    return size;
}

int64_t NuMemFileSeek(NUFILE file, int64_t offset, NUFILESEEK whence) {
    if (file >= 0x800) {
        return NuDatFileSeek(file, offset, whence);
    }

    file -= 0x400;

    switch (whence) {
        case NUFILE_SEEK_CURRENT:
            memfiles[file].ptr = memfiles[file].ptr + offset;
            break;
        case NUFILE_SEEK_END:
            memfiles[file].ptr = memfiles[file].end - offset;
            break;
        default:
            memfiles[file].ptr = memfiles[file].buffer + offset;
            break;
    }

    return (int64_t)(memfiles[file].ptr - memfiles[file].buffer);
}

int64_t NuMemFilePos(NUFILE file) {
    if (file >= 0x800) {
        return NuDatFilePos(file);
    }

    file -= 0x400;

    return (int64_t)(memfiles[file].ptr - memfiles[file].buffer);
}
