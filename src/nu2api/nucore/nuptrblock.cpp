#include "nu2api/nucore/nuptrblock.h"

#include "nu2api/nucore/common.h"

void *NuPtrBlockFix(void *block) {
    VARIPTR buf;
    VARIPTR ptr;
    VARIPTR start;
    i32 count;
    i32 i;

    start.void_ptr = buf.void_ptr = block;
    start.addr += 4;

    buf.addr = *buf.u32_ptr + buf.addr;

    count = *buf.u32_ptr++;

    for (i = 0; i < count; i++) {
        ptr.addr = *buf.u32_ptr + buf.addr;

        if (*ptr.u32_ptr != 0) {
            *ptr.u32_ptr = *ptr.u32_ptr + ptr.addr;
        }

        buf.addr += 4;
    }

    return start.void_ptr;
}
