#include "legoapi/gizmos/door/plugs.h"

#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"

struct PLUGPROGRESS {
    i32 visible_mask;
    i32 active_mask;
    i32 plugged_mask;
};

i32 plug_gizmotype_id = -1;

static i32 Plugs_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_plugs;
}

static void Plugs_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->plug_sys == NULL) {
        return;
    }

    for (i32 index = 0; index < world->plug_sys->count; ++index) {
        PLUG &plug = world->plug_sys->plugs[index];
        if (NuStrLen(plug.name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, &plug);
        }
    }
}

static void Plugs_Update(void *, void *, float) {
}

static void Plugs_Draw(void *, void *, float) {
}

static char *Plug_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<PLUG *>(gizmo->object)->name : NULL;
}

static i32 Plug_GetOutput(GIZMO *gizmo, i32, i32) {
    PLUG *plug = static_cast<PLUG *>(gizmo->object);
    return (plug->flags & (PLUG_FLAG_ACTIVE | PLUG_FLAG_VISIBLE | PLUG_FLAG_PLUGGED)) ==
           (PLUG_FLAG_ACTIVE | PLUG_FLAG_VISIBLE | PLUG_FLAG_PLUGGED);
}

static char *Plug_GetOutputName(GIZMO *, i32) {
    return const_cast<char *>("Plugged");
}

static i32 Plug_GetNumOutputs(GIZMO *) {
    return 1;
}

static void Plug_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo == NULL) {
        return;
    }
    PLUG *plug = static_cast<PLUG *>(gizmo->object);
    const u8 active_flag = active != 0;
    plug->flags = static_cast<u8>((plug->flags & ~PLUG_FLAG_ACTIVE) | active_flag);
}

static void Plug_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo == NULL) {
        return;
    }
    PLUG *plug = static_cast<PLUG *>(gizmo->object);
    const u8 visible_flag = visible != 0;
    plug->flags = static_cast<u8>((plug->flags & ~PLUG_FLAG_VISIBLE) | (visible_flag << 1));
}

static NUVEC *Plug_GetPos(GIZMO *gizmo) {
    return gizmo != NULL ? &static_cast<PLUG *>(gizmo->object)->position : NULL;
}

static void *Plugs_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(PLUGPROGRESS));
}

static void Plugs_ClearProgress(void *, void *progress_ptr) {
    PLUGPROGRESS *progress = static_cast<PLUGPROGRESS *>(progress_ptr);
    if (progress == NULL) {
        return;
    }
    progress->visible_mask = -1;
    progress->active_mask = -1;
    progress->plugged_mask = 0;
}

static void Plugs_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    PLUGPROGRESS *progress = static_cast<PLUGPROGRESS *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    progress->visible_mask = -1;
    progress->active_mask = -1;
    progress->plugged_mask = 0;
    if (world == NULL || world->plug_sys == NULL || world->plug_sys->plugs == NULL) {
        return;
    }

    for (i32 index = 0; index < world->plug_sys->count; ++index) {
        if (index == 32) {
            break;
        }
        const u32 bit = 1u << index;
        const u8 flags = world->plug_sys->plugs[index].flags;
        if ((flags & PLUG_FLAG_VISIBLE) == 0) {
            progress->visible_mask &= ~bit;
        }
        if ((flags & PLUG_FLAG_ACTIVE) == 0) {
            progress->active_mask &= ~bit;
        }
        if ((flags & PLUG_FLAG_PLUGGED) != 0) {
            // This is how the original encodes the state, despite having a
            // separately initialized plugged mask in the progress record.
            progress->active_mask |= bit;
        }
    }
}

static void Plugs_Reset(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    PLUGPROGRESS *progress = static_cast<PLUGPROGRESS *>(progress_ptr);
    if (world == NULL || world->plug_sys == NULL || world->plug_sys->plugs == NULL) {
        return;
    }

    for (i32 index = 0; index < world->plug_sys->count; ++index) {
        PLUG &plug = world->plug_sys->plugs[index];
        plug.flags = static_cast<u8>((plug.flags | PLUG_FLAG_ACTIVE | PLUG_FLAG_VISIBLE) & ~PLUG_FLAG_PLUGGED);
        if (progress == NULL || index > 31) {
            continue;
        }

        const u32 bit = 1u << index;
        plug.flags = static_cast<u8>((plug.flags & ~(PLUG_FLAG_VISIBLE | PLUG_FLAG_PLUGGED)) |
                                     ((progress->visible_mask & bit) != 0 ? PLUG_FLAG_VISIBLE : 0));
        plug.flags = static_cast<u8>((plug.flags & ~PLUG_FLAG_ACTIVE) |
                                     ((progress->active_mask & bit) != 0 ? PLUG_FLAG_ACTIVE : 0));
        if ((progress->plugged_mask & bit) != 0) {
            plug.flags |= PLUG_FLAG_PLUGGED;
        }
    }
}

static void *Plugs_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    world->plug_sys = NULL;
    if (world->current_level->max_plugs == 0) {
        return NULL;
    }

    world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
    PLUG *plugs = static_cast<PLUG *>(world->giz_buffer.void_ptr);
    world->giz_buffer.addr += world->current_level->max_plugs * sizeof(PLUG);
    world->plug_sys = static_cast<PLUGSYS *>(world->giz_buffer.void_ptr);
    world->giz_buffer.addr += sizeof(PLUGSYS);
    world->plug_sys->plugs = plugs;
    world->plug_sys->count = 0;
    return world->plug_sys;
}

static i32 Plugs_Load(void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->plug_sys == NULL || world->plug_sys->count != 0) {
        return 0;
    }

    const i32 version = EdFileReadInt();
    world->plug_sys->count = EdFileReadInt();
    for (i32 index = 0; index < world->plug_sys->count; ++index) {
        PLUG &plug = world->plug_sys->plugs[index];
        EdFileRead(plug.name, sizeof(plug.name));
        EdFileReadNuVec(&plug.position);
        plug.x_rotation = EdFileReadUnsignedShort();
        plug.y_rotation = EdFileReadUnsignedShort();
        plug.enabled = EdFileReadUnsignedChar();
        plug.target_id = version <= 1 ? 0 : EdFileReadUnsignedShort();
    }
    return 1;
}

ADDGIZMOTYPE *Plugs_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Plug";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0xc;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Plugs_GetMaxGizmos;
    addtype.fns.get_pos_fn = Plug_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Plugs_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Plugs_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Plugs_Draw;
    addtype.fns.get_gizmo_name_fn = Plug_GetGizmoName;
    addtype.fns.get_output_fn = Plug_GetOutput;
    addtype.fns.get_output_name_fn = Plug_GetOutputName;
    addtype.fns.get_num_outputs_fn = Plug_GetNumOutputs;
    addtype.fns.activate_fn = Plug_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Plug_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Plugs_AllocateProgressData;
    addtype.fns.clear_progress_fn = Plugs_ClearProgress;
    addtype.fns.store_progress_fn = Plugs_StoreProgress;
    addtype.fns.reset_fn = Plugs_Reset;
    addtype.fns.reserve_buffer_space_fn = Plugs_ReserveBufferSpace;
    addtype.fns.load_fn = Plugs_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    plug_gizmotype_id = type_id;

    return &addtype;
}
