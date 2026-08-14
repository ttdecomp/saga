#include "nu2api/nufile/nufile.h"

i32 NuMcOpen(i32 port, i32 slot, char *filepath, i32 mode, i32 async) {
    return 0;
}

i32 NuMcClose(i32 fd, i32 async) {
    return 0;
}

i32 NuMcSeek(i32 fd, i32 offset, NUFILESEEK mode, i32 async) {
    return 0;
}

i32 NuMcOpenSize(i32 fd) {
    return 0;
}

i32 NuMcRead(i32 fd, void *buf, i32 size, i32 async) {
    return 0;
}

i32 NuMcWrite(i32 fd, void *data, i32 size, i32 async) {
    return 0;
}

i32 NuMcFileOpenSize(NUFILE file) {
    file -= 0x1000;

    return NuMcOpenSize(file);
}
