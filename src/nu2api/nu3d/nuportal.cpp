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

void NuPortalSetActiveDirect(NUPORTAL *portal, i32 active) {
    if (portal == NULL) {
        return;
    }

    if (active) {
        portal->is_active |= NUPORTAL_FLAG_ACTIVE;
    } else {
        portal->is_active = 0;
    }
}

void NuPortalSetActive(NUGSCN *scene, i32 portal_id, i32 active) {
    for (u32 index = 0; index < scene->max_portals; ++index) {
        NUPORTAL *portal = &scene->portals[index];
        if (portal->id == portal_id) {
            if (active != 0) {
                portal->is_active |= NUPORTAL_FLAG_ACTIVE;
            } else {
                portal->is_active = 0;
            }
        }
    }
}

void NuPortalMaxDepth(struct nugscn_s *scene, i32 depth) {
    scene->portal_depth = depth;
}
