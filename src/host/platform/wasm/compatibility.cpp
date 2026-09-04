#include "decomp.h"

// The browser host does not currently use the compact animation packet path;
// its reconstructed layout is not safe to mutate from the player bootstrap.
extern "C" void ResetMiniAnimPacket(void *, i32) {
}
