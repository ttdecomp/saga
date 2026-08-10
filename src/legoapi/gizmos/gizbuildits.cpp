#include "legoapi/gizmos/gizbuildits.h"

#include "decomp.h"
#include "legoapi/gameobject.h"
#include "legoapi/players.h"

extern i32 LEGOCONTEXT_BUILDIT;

struct GAMECAMERA_s;
extern GAMECAMERA_s *GameCam;
void GameCam_Blend(GAMECAMERA_s *camera, f32 blend_time, f32 hold_time, i32 restore);

void GizBuildIt_SetHeadTarget(GIZBUILDIT *buildit, GameObject_s *object) {
    if (buildit->part_count != 0) {
        i32 part_index = buildit->current_part;
        if (part_index >= buildit->part_count) {
            part_index = buildit->part_count - 1;
        }
        SetHeadTarget(object, &buildit->parts[part_index]->target->head_target, 0, 2.0f, 1.0f, 2.0f);
    }
}

f32 GizBuildItMul(GameObject_s *object) {
    if ((i8)object->state_flags < 0 && Player_HasFastBuild(object)) {
        return 3.0f;
    }

    i32 progress = object->player_packet.buildit_step;
    GIZBUILDIT *buildit = object->player_packet.buildit;
    i32 current_part = buildit->current_part;
    i32 part_count = buildit->part_count;
    if ((current_part - progress + part_count) / 2 <= current_part) {
        progress = part_count - current_part;
    }
    progress = MAX(progress, 0);
    progress = MIN(progress, 10);
    return (f32)progress / 10.0f + 1.0f;
}

void GizBuildIt_SetStepTime(GIZBUILDIT *buildit, GameObject_s *object) {
    buildit->step_time = 0.3f;
    if (object != NULL) {
        buildit->step_time /= GizBuildItMul(object);
    }
    buildit->previous_step_time = buildit->step_time;
}

void ReleaseBuildIt(GameObject_s *object, i32 complete) {
    if (LEGOCONTEXT_BUILDIT != -1 && object->player_packet.character_state == LEGOCONTEXT_BUILDIT) {
        object->player_packet.character_state = -1;
        GameCam_Blend(GameCam, 0.5f, complete != 0 ? 0.6f : 0.0f, 1);
    }
}

int gizbuildit_gizmotype_id = -1;

static int GizBuildIts_GetMaxGizmos(void *buildit) {
    UNIMPLEMENTED();
}

static void GizBuildIts_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_EarlyUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizBuildIts_LateUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizBuildIts_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoBuildit_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizmoBuildit_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizmoBuildit_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizmoBuildit_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizmoBuildit_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GizmoBuildit_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoBuildit_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void *GizBuildIts_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizBuildIts_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int GizBuildIts_Load(void *, void *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizBuildIts_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizBuildit";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x60;
    addtype.fns.early_update_fn = GizBuildIts_EarlyUpdate;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizBuildIts_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoBuildit_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizBuildIts_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizBuildIts_LateUpdate;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizBuildIts_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoBuildit_GetGizmoName;
    addtype.fns.get_output_fn = GizmoBuildit_GetOutput;
    addtype.fns.get_output_name_fn = GizmoBuildit_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoBuildit_GetNumOutputs;
    addtype.fns.activate_fn = GizmoBuildit_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizmoBuildit_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizBuildIts_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizBuildIts_ClearProgress;
    addtype.fns.store_progress_fn = GizBuildIts_StoreProgress;
    addtype.fns.reset_fn = GizBuildIts_Reset;
    addtype.fns.reserve_buffer_space_fn = GizBuildIts_ReserveBufferSpace;
    addtype.fns.load_fn = GizBuildIts_Load;
    addtype.fns.post_load_fn = GizBuildIts_PostLoad;
    addtype.fns.add_level_sfx_fn = NULL;
    gizbuildit_gizmotype_id = type_id;

    return &addtype;
}
