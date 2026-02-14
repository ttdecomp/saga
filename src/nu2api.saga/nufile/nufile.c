#include "nu2api.saga/nufile/nufile.h"

#include "decomp.h"

#include "nu2api.saga/nucore/nustring.h"

int DEVHOST_Interrogate(NUFILE_DEVICE *device) {
    device->status = 1;

    return 1;
}

int DEV_FormatName(NUFILE_DEVICE *device, char *formatted_name, char *path, int buf_size) {
    char buf[512];
    int device_name_len;
    char *subpath;
    int path_pos;
    int path_len;
    int sep_pos;
    char *root;

    device_name_len = NuStrLen(device->name);

    if (NuStrNICmp(path, device->name, device_name_len) == 0) {
        sep_pos = NuStrLen(device->name);
        subpath = &path[sep_pos];

        if (*subpath == '\\' || *subpath == '/') {
            NuStrCpy(buf, path);
            root = device->root;
        } else {
            root = device->root;
            NuStrCpy(buf, root);
            NuStrCat(buf, device->cur_dir);
            NuStrCat(buf, subpath);
        }
    } else {
        root = device->root;
        NuStrCpy(buf, root);
        NuStrCat(buf, device->cur_dir);
        NuStrCat(buf, path);
    }

    path_pos = NuStrLen(root);
    NuFileCorrectSlashes(device, buf + path_pos);
    NuFileReldirFix(device, buf);

    path_len = NuStrLen(buf);
    if (buf_size > path_len) {
        NuStrCpy(formatted_name, buf);
        return 1;
    }

    return 0;
}

i32 NuFileReadDir(NUFILE file) {
    return 0;
}

i32 NuFileExists(char *name) {
    LOG_DEBUG("name=%s", name);

    return NuFileSize(name) > 0;
}

i32 NuFileNormalise(char *dst, i32 length, const char *src) {
    NuStrNCpy(dst, src, length);
    return 1;
}
