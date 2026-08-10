#include "nu2api/nucore/numem.h"

#include <string.h>

void NuMemSet128(void *ptr, u8 value, isize size) {
    memset(ptr, value, size << 4);
}
