#include "legoapi/gizmos/tubes.h"

#include "decomp.h"
#include "legoapi/gameobject.h"

extern i32 LEGOCONTEXT_GLIDE;
extern i32 LEGOCONTEXT_TUBE;

namespace TouchHacks {
    extern u8 TouchControlsActive;
}

i32 Tube_InCylinder(GameObject_s *object, TUBE *tube, f32 *distance_squared, i32 ignore_height) {
    if (tube == NULL || object == NULL) {
        return 0;
    }

    if (ignore_height == 0 && (object->bounds_max.y < tube->position.y || tube->top_y < object->bounds_min.y)) {
        return 0;
    }

    f32 delta_x = object->position.x - tube->position.x;
    f32 delta_z = object->position.z - tube->position.z;
    f32 object_distance_squared = delta_x * delta_x + delta_z * delta_z;
    f32 maximum_distance_squared = tube->radius_squared;
    if ((tube->flags & 0x10) != 0 && TouchHacks::TouchControlsActive != 0) {
        maximum_distance_squared *= 0.8f;
    }

    if (object_distance_squared > maximum_distance_squared) {
        return 0;
    }
    if (distance_squared != NULL) {
        *distance_squared = object_distance_squared;
    }
    return 1;
}

TUBE *Tube_InAnyCylinder(WORLDINFO_s *world, GameObject_s *object, i32 ignore_height) {
    TUBE *tubes = *(TUBE **)((u8 *)world + 0x4694);
    i32 tube_count = *(i32 *)((u8 *)world + 0x4698);
    if (tubes != NULL) {
        for (i32 i = 0; i < tube_count; i++) {
            if ((tubes[i].flags & 7) == 3 && Tube_InCylinder(object, &tubes[i], NULL, ignore_height)) {
                return &tubes[i];
            }
        }
    }
    return NULL;
}

i32 ObjInTube(GameObject_s *object) {
    i8 character_state = object->player_packet.character_state;
    if (LEGOCONTEXT_TUBE != -1 && character_state == LEGOCONTEXT_TUBE) {
        return 1;
    }
    if (LEGOCONTEXT_GLIDE != -1 && character_state == LEGOCONTEXT_GLIDE &&
        *(GameObject_s **)((u8 *)object + 0x788) != NULL) {
        return 1;
    }
    return 0;
}

static int Tubes_GetMaxGizmos(void *tube) {
    UNIMPLEMENTED();
}

static void Tubes_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void Tubes_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Tubes_Draw(void *, void *, float) {
}

static char *Tube_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Tube_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Tube_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Tube_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Tube_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Tube_ActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static void Tube_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Tube_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void *Tubes_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Tubes_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Tubes_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Tubes_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *Tubes_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int Tubes_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *Tubes_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Tube";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Tubes_GetMaxGizmos;
    addtype.fns.get_pos_fn = Tube_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Tubes_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Tubes_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Tubes_Draw;
    addtype.fns.get_gizmo_name_fn = Tube_GetGizmoName;
    addtype.fns.get_output_fn = Tube_GetOutput;
    addtype.fns.get_output_name_fn = Tube_GetOutputName;
    addtype.fns.get_num_outputs_fn = Tube_GetNumOutputs;
    addtype.fns.activate_fn = Tube_Activate;
    addtype.fns.activate_rev_fn = Tube_ActivateRev;
    addtype.fns.set_visibility_fn = Tube_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Tubes_AllocateProgressData;
    addtype.fns.clear_progress_fn = Tubes_ClearProgress;
    addtype.fns.store_progress_fn = Tubes_StoreProgress;
    addtype.fns.reset_fn = Tubes_Reset;
    addtype.fns.reserve_buffer_space_fn = Tubes_ReserveBufferSpace;
    addtype.fns.load_fn = Tubes_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
