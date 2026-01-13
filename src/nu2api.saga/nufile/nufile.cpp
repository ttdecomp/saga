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