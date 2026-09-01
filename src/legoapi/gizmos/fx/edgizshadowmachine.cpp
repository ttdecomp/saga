#include "legoapi/gizmos/fx/edgizshadowmachine.h"

#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "legoapi/world/area.h"
#include "legoapi/world/world.h"

#include <string.h>

i32 edGizShadow_gizmotype_id;
static char shadow_gizmo_name[] = "ShadowDirection";
static char shadow_output_name[] = "Dont Use Me!";

static void edGizShadow_SetPreset1(EDGIZSHADOW *shadow) {
    shadow->field_0x10 = 2.0f;
    shadow->field_0x14 = 0.1f;
    shadow->field_0x18 = 0.0005f;
    shadow->field_0x1c = 0.01f;
    shadow->field_0x20 = 6.0f;
    shadow->field_0x24 = 1.3f;
    shadow->field_0x28 = 0.0f;
    shadow->field_0x2c = 0.75f;
    shadow->field_0x30 = -2.0f;
    shadow->preset = EDGIZSHADOW_PRESET_1;
}

static void edGizShadow_SetPreset2(EDGIZSHADOW *shadow) {
    shadow->field_0x10 = 2.0f;
    shadow->field_0x14 = 0.1f;
    shadow->field_0x18 = 0.0005f;
    shadow->field_0x1c = 0.01f;
    shadow->field_0x20 = 25.0f;
    shadow->field_0x24 = 0.5f;
    shadow->field_0x28 = 0.0f;
    shadow->field_0x2c = 0.5f;
    shadow->field_0x30 = -15.0f;
    shadow->preset = EDGIZSHADOW_PRESET_2;
}

static i32 edGizShadow_GetMaxGizmos(void *) {
    return 1;
}

static void edGizShadow_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    AddGizmo(gizmo_sys, type_id, NULL, &world->shadow_editor);
}

static char *edGizShadow_GetGizmoName(GIZMO *) {
    return shadow_gizmo_name;
}

static i32 edGizShadow_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *edGizShadow_GetOutputName(GIZMO *, i32) {
    return shadow_output_name;
}

static i32 edGizShadow_GetNumOutputs(GIZMO *) {
    return 1;
}

static void *edGizShadow_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
    world->shadow_editor = reinterpret_cast<EDGIZSHADOW *>(world->giz_buffer.addr);
    world->giz_buffer.addr += sizeof(EDGIZSHADOW);

    EDGIZSHADOW *shadow = world->shadow_editor;
    memset(shadow, 0, sizeof(*shadow));
    shadow->direction = {-2.0f, -10.0f, -2.0f};
    shadow->field_0x0c = 0.3f;
    shadow->field_0x10 = 2.0f;
    shadow->field_0x14 = 0.5f;
    shadow->field_0x18 = 0.0005f;
    shadow->field_0x1c = 0.01f;
    shadow->field_0x20 = 22.0f;
    shadow->field_0x24 = 0.5f;
    shadow->field_0x30 = -2.0f;
    shadow->preset = EDGIZSHADOW_PRESET_CUSTOM;
    return shadow;
}

static i32 edGizShadow_Load(void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->shadow_editor == NULL) {
        return 0;
    }

    EDGIZSHADOW *shadow = world->shadow_editor;
    const i32 version = EdFileReadChar();
    const i32 record_count = EdFileReadChar() + 1;
    for (i32 record = 0; record < record_count; ++record) {
        EdFileReadNuVec(&shadow->direction);
        shadow->field_0x0c = EdFileReadFloat();

        if (version <= 1) {
            shadow->field_0x10 = 2.0f;
            shadow->field_0x14 = 0.5f;
            shadow->field_0x18 = 0.0005f;
            shadow->field_0x1c = 0.01f;
            shadow->field_0x20 = 22.0f;
        } else {
            shadow->field_0x10 = EdFileReadFloat();
            shadow->field_0x14 = EdFileReadFloat();
            if (version == 2) {
                shadow->field_0x18 = 0.0005f;
                shadow->field_0x1c = 0.01f;
                shadow->field_0x20 = 22.0f;
            } else {
                shadow->field_0x18 = EdFileReadFloat();
                shadow->field_0x1c = EdFileReadFloat();
                if (version == 3) {
                    shadow->field_0x20 = 22.0f;
                } else {
                    shadow->field_0x20 = EdFileReadFloat();
                    (void)EdFileReadFloat();
                    (void)EdFileReadFloat();
                    if (version != 4) {
                        shadow->field_0x24 = EdFileReadFloat();
                        shadow->field_0x28 = EdFileReadFloat();
                        shadow->field_0x2c = EdFileReadFloat();
                        if (version != 6) {
                            shadow->field_0x30 = EdFileReadFloat();
                            if (version != 7) {
                                shadow->preset = static_cast<EDGIZSHADOW_PRESET>(EdFileReadInt());
                            }
                        }
                    }
                }
            }
        }

        if (world->area != NULL) {
            if ((world->area->flags & 1) != 0) {
                edGizShadow_SetPreset2(shadow);
            } else {
                edGizShadow_SetPreset1(shadow);
            }
        } else if (shadow->preset == EDGIZSHADOW_PRESET_1) {
            edGizShadow_SetPreset1(shadow);
        } else if (shadow->preset == EDGIZSHADOW_PRESET_2) {
            edGizShadow_SetPreset2(shadow);
        }
    }
    return 1;
}

ADDGIZMOTYPE *EdGizShadowMachine_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "ShadowEditor";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = edGizShadow_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = edGizShadow_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = edGizShadow_GetGizmoName;
    addtype.fns.get_output_fn = edGizShadow_GetOutput;
    addtype.fns.get_output_name_fn = edGizShadow_GetOutputName;
    addtype.fns.get_num_outputs_fn = edGizShadow_GetNumOutputs;
    addtype.fns.activate_fn = NULL;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = edGizShadow_ReserveBufferSpace;
    addtype.fns.load_fn = edGizShadow_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    edGizShadow_gizmotype_id = type_id;

    return &addtype;
}
