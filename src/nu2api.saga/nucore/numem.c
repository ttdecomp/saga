#include "nu2api.saga/nucore/numem.h"

#include <string.h>

void NuMemSet128(void *ptr, u32 value, isize size) {
    u8 trunc;

    trunc = value;
    memset(ptr, trunc, size << 4);
}
