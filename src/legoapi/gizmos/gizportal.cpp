#include "legoapi/gizmos/gizportal.h"

#include <stdio.h>

#include "decomp.h"

#include "legoapi/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuportal.h"

i32 portal_gizmotype_id;

static i32 Portal_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = (WORLDINFO *)world_info;

    if (world == NULL || world->current_gscn == NULL) {
        return 0;
    }

    return world->current_gscn->max_portals;
}

static void Portal_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_info, void *) {
    WORLDINFO *world = (WORLDINFO *)world_info;

    if (world == NULL || world->current_gscn == NULL) {
        return;
    }

    for (int i = 0; i < world->current_gscn->max_portals; i++) {
        NUPORTAL *portal = &world->current_gscn->portals[i];

        if (portal->id == 0) {
            continue;
        }

        AddGizmo(gizmo_sys, type_id, NULL, portal);
    }
}

static char *Portal_GetGizmoName(GIZMO *gizmo) {
    static char name[16];

    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }

    NUPORTAL *portal = (NUPORTAL *)gizmo->object;

    sprintf(name, "%s%d", "portal_", portal->id);

    return name;
}

static int Portal_GetOutput(GIZMO *gizmo, int, int) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }

    NUPORTAL *portal = (NUPORTAL *)gizmo->object;

    return portal->is_active & 1;
}

char *Portal_GetOutputName(GIZMO *gizmo, i32 output_index) {
    static char name[64] = "Active";

    return name;
}

static i32 Portal_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

void Portal_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    NuPortalSetActiveDirect((NUPORTAL *)gizmo->object, active);
}

static i32 Portal_ActivateRev(GIZMO *gizmo, i32 is_inactive, i32 unknown) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }

    NUPORTAL *portal = (NUPORTAL *)gizmo->object;

    if ((unknown & 1) == 0) {
        NuPortalSetActiveDirect(portal, is_inactive == 0);
    } else if (is_inactive != 0) {
        if ((portal->is_active & 1) == 0 || is_inactive != 1) {
            return 0;
        }
    }

    return 1;
}

static void *Portals_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(GIZPORTALPROGRESS));
}

static void Portals_ClearProgress(void *, void *progress) {
    GIZPORTALPROGRESS *portal_progress = (GIZPORTALPROGRESS *)progress;

    if (portal_progress == NULL) {
        return;
    }

    portal_progress->progress_mask = ~0u;
}

static void Portals_StoreProgress(void *world_info, void *, void *progress) {
    WORLDINFO *world = (WORLDINFO *)world_info;
    GIZPORTALPROGRESS *portal_progress = (GIZPORTALPROGRESS *)progress;

    if (portal_progress != NULL) {
        portal_progress->progress_mask = ~0u;
    }

    if (world == NULL || world->current_gscn == NULL || portal_progress == NULL ||
        world->current_gscn->max_portals <= 0) {
        return;
    }

    NUPORTAL *portal = world->current_gscn->portals;
    NUPORTAL *end = portal + world->current_gscn->max_portals;
    int index = 0;
    for (;;) {
        if (portal->id != 0 && index <= SIZEOF_BITS(portal_progress->progress_mask) - 1) {
            if (portal->is_active & 1) {
                portal_progress->progress_mask |= (1 << index);
            }
            index++;
        }
        portal++;

        if (portal == end) {
            break;
        }
    }
}

static void Portals_Reset(void *world_info, void *, void *progress) {
    WORLDINFO *world = (WORLDINFO *)world_info;
    GIZPORTALPROGRESS *portal_progress = (GIZPORTALPROGRESS *)progress;

    if (world == NULL || world->current_gscn == NULL) {
        return;
    }

    PortalDoors_Reset(world);

    NUGSCN *gscn = world->current_gscn;
    if (gscn->max_portals <= 0) {
        return;
    }

    int index = 0;
    for (int i = 0; i < gscn->max_portals; i++) {
        NUPORTAL *portal = &gscn->portals[i];
        if (portal->id == 0) {
            continue;
        }

        NuPortalSetActiveDirect(portal, 1);
        gscn = world->current_gscn;
        if (index <= SIZEOF_BITS(portal_progress->progress_mask) - 1 && portal_progress != NULL &&
            (gscn->portals[i].is_active & 1)) {
            portal_progress->progress_mask |= (1 << index);
        }

        index++;
    }
}

ADDGIZMOTYPE *Portal_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Portal";
    addtype.prefix = "portal_";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Portal_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Portal_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = Portal_GetGizmoName;
    addtype.fns.get_output_fn = Portal_GetOutput;
    addtype.fns.get_output_name_fn = Portal_GetOutputName;
    addtype.fns.get_num_outputs_fn = Portal_GetNumOutputs;
    addtype.fns.activate_fn = Portal_Activate;
    addtype.fns.activate_rev_fn = Portal_ActivateRev;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.allocate_progress_data_fn = Portals_AllocateProgressData;
    addtype.fns.clear_progress_fn = Portals_ClearProgress;
    addtype.fns.store_progress_fn = Portals_StoreProgress;
    addtype.fns.reset_fn = Portals_Reset;
    addtype.fns.reserve_buffer_space_fn = NULL;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    portal_gizmotype_id = type_id;

    return &addtype;
}

void PortalDoors_Reset(WORLDINFO *world_info) {
    UNIMPLEMENTED();
}
