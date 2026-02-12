#include "nu2api.saga/nu3d/nuportal.h"

static VARIPTR fstack;
static i8 fstackmem[12288];
static VARIPTR fstack_limit;
static VARIPTR fstack_top;

void NuPortalInit(void) {
    // the ghidra readout for this is completely wrong
    fstack.void_ptr = fstackmem;
    fstack_top.addr = ALIGN((usize)fstackmem, 16);
    fstack_limit.void_ptr = fstackmem + 0x3000;
}

void NuPortalSetActiveDirect(NUPORTAL *portal, int active) {
    if (portal == NULL) {
        return;
    }

    if (active) {
        portal->active |= 1;
    } else {
        portal->active = 0;
    }
}
