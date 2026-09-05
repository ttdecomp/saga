#include "gameframework/saveload.h"

i32 ChecksumSaveData(void *buffer, i32 size) {
    i32 n = size / 4;
    u32 sum = 0x5c0999;

    for (i32 i = 0; i < n; i++) {
        sum += static_cast<u32 *>(buffer)[i];
    }

    return sum;
}
