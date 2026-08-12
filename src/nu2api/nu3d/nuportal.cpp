#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nugscn.h"

static VARIPTR fstack;
static i8 fstackmem[12288];
static VARIPTR fstack_limit;
static VARIPTR fstack_top;

void NuPortalInit(void) {
    fstack.void_ptr = fstackmem;
    fstack_top.addr = ALIGN((usize)fstackmem, 16);
    fstack_limit.void_ptr = fstackmem + 0x3000;
}

void NuPortalSetActiveDirect(NUPORTAL *portal, int active) {
    if (portal == NULL) {
        return;
    }

    if (active) {
        portal->is_active |= 1;
    } else {
        portal->is_active = 0;
    }
}

void NuPortalMaxDepth(struct nugscn_s *scene, int depth) {
    scene->portal_depth = depth;
}
