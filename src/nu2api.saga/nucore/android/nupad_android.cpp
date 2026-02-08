#include "nu2api.saga/nucore/nupad.h"

void NuPadInitPS(NUGENERICPAD *pad) {
}

void NuPadOpenPS(NUPAD *pad) {
    pad->is_valid = true;
}

void NuPadClosePS(NUPAD *pad) {
}

i32 NuPadGetDeadzoneByPortPS(i32 port) {
    // No idea where this value comes from.
    return 0x14;
}
