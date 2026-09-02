#include "nu2api_nufile_types.h"

NuFileAndroidAPK *NuFileAndroidAPK::ms_fileId[0x400];

void NuFileAndroidAPK::Init() {
}

void NuFileAndroidAPK::Open(char const *, NuFile::OpenMode::T) {
}

void NuFileAndroidAPK::ResetId(i32 id) {
    ms_fileId[id - 0x2000] = NULL;
}

i32 NuFileAndroidAPK::SetFileId(NuFileAndroidAPK *file) {
    for (i32 i = 0; i < 0x400; i++) {
        if (ms_fileId[i] == NULL) {
            ms_fileId[i] = file;
            return i + 0x2000;
        }
    }
    return -1;
}
