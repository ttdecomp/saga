#include "decomp.h"

// Local (static) helpers whose mangled name collides with a global symbol of
// the same name elsewhere in the codebase. C++ requires static and
// non-static definitions of the same name to live in separate translation
// units, so this file hosts the local variants.

static __used__ u16 RotDiff(u16, u16) {
    return 0;
}
