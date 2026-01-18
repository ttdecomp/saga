#include "nu2api.saga/nufile/nufile.h"

#include "nu2api.saga/nucore/nustring.h"

#include "decomp.h"

int32_t numdevices = 0;

NUFILE_DEVICE devices[16] = {0};

int32_t file_criticalsection;
FILE *g_fileHandles[32] = {NULL};

NUFILE_DEVICE *NuFileGetDeviceFromPath(char *path) {
    LOG_DEBUG("path=%s", path);

    int i;
    for (i = 0; (i < 8 && (path[i] != ':')); i = i + 1) {
    }

    if (i < 8) {
        for (int32_t index = 0; index < numdevices; index++) {
            if (NuStrNICmp(path, devices[index].name, devices[index].match_len) == 0) {
                return &devices[index];
            }
        }
    }

    LOG_DEBUG("No device found for path=%s", path);

    return NULL;
}

int32_t NuFileExists(char *name) {
    LOG_DEBUG("name=%s", name);
    return NuFileSize(name) > 0 ? 1 : 0;
}

int DEVHOST_Interrogate(NUFILE_DEVICE *device) {
    device->status = 1;

    return 1;
}

int DEV_FormatName(NUFILE_DEVICE *device, char *formatted_name, char *path, int buf_size) {
    int device_name_len;

    device_name_len = NuStrLen(device->name);
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
