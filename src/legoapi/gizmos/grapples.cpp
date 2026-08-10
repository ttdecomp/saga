#include "legoapi/gizmos/grapples.h"

#include "decomp.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

struct GrapplePlayerData {
    NUVEC fixed_target;
    u8 unknown_0c[0x24];
    f32 rope_length;
    u8 unknown_34[8];
    f32 climb_progress;
    u8 unknown_40[0x10];
    GRAPPLE *grapple;
    u8 unknown_54[0xa];
    u16 heading;
    u16 swing_phase;
    u8 unknown_62[9];
    u8 mode;
    u8 rotation_frame;
    i8 character_state;
};

#ifndef __x86_64__
static_assert(offsetof(GrapplePlayerData, rope_length) == 0x30, "GrapplePlayerData rope_length offset");
static_assert(offsetof(GrapplePlayerData, climb_progress) == 0x3c, "GrapplePlayerData climb_progress offset");
static_assert(offsetof(GrapplePlayerData, grapple) == 0x50, "GrapplePlayerData grapple offset");
static_assert(offsetof(GrapplePlayerData, heading) == 0x5e, "GrapplePlayerData heading offset");
static_assert(offsetof(GrapplePlayerData, swing_phase) == 0x60, "GrapplePlayerData swing_phase offset");
static_assert(offsetof(GrapplePlayerData, mode) == 0x6b, "GrapplePlayerData mode offset");
#endif

extern i32 LEGOCONTEXT_GRAPPLE;

extern "C" {
    void NuVecRotateX(NUVEC *destination, NUVEC *source, i32 angle);
    void NuVecRotateY(NUVEC *destination, NUVEC *source, i32 angle);
}

static GrapplePlayerData *GetGrapplePlayerData(GameObject_s *object) {
    return (GrapplePlayerData *)((u8 *)object + 0x738);
}

int Grapple_ReachedTop(GameObject_s *object) {
    GrapplePlayerData *data = GetGrapplePlayerData(object);
    return LEGOCONTEXT_GRAPPLE != -1 && data->character_state == LEGOCONTEXT_GRAPPLE &&
           (data->mode == 1 || (data->mode == 0 && data->climb_progress >= 0.5f));
}

void Grapple_SetPlayerTargetPoint(GameObject_s *object, NUVEC *target) {
    GrapplePlayerData *data = GetGrapplePlayerData(object);
    target->x = 0.0f;
    target->y = -data->rope_length;
    target->z = 0.0f;

    i32 rotation = (data->rotation_frame * 0x10000u) / 360;
    rotation = (i32)((f32)rotation * NuTrigTable[data->swing_phase >> 1] * data->grapple->swing_angle_scale);
    NuVecRotateX(target, target, rotation);
    NuVecRotateY(target, target, data->heading);
    NuVecAdd(target, target, &data->grapple->target_position);
}

int Grapple_SetTargetMom(GameObject_s *object) {
    GrapplePlayerData *data = GetGrapplePlayerData(object);
    NUVEC *position = (NUVEC *)((u8 *)object + 0x190);
    NUVEC *target_momentum = (NUVEC *)((u8 *)object + 0xf24);

    if (data->mode == 1) {
        target_momentum->x = (data->fixed_target.x - position->x) * 20.0f;
        target_momentum->y = (data->fixed_target.y - position->y) * 20.0f;
        target_momentum->z = (data->fixed_target.z - position->z) * 20.0f;
    } else {
        NUVEC target;
        Grapple_SetPlayerTargetPoint(object, &target);
        target_momentum->x = (target.x - position->x) * 5.0f;
        target_momentum->y = (target.y - position->y) * 5.0f;
        target_momentum->z = (target.z - position->z) * 5.0f;
    }
    return 1;
}

void Grapple_SetRotOrder(GameObject_s *object) {
    if (GetGrapplePlayerData(object)->mode != 1) {
        *((u8 *)object + 0x1086) = 2;
    }
}

int Grapple_LookAtPos(GameObject_s *object, NUVEC *position) {
    position->x = (*(GRAPPLE **)((u8 *)object + 0x788))->look_at_position.x;
    position->y = (*(GRAPPLE **)((u8 *)object + 0x788))->look_at_position.y - *(f32 *)((u8 *)object + 0x768);
    position->z = (*(GRAPPLE **)((u8 *)object + 0x788))->look_at_position.z;
    return 1;
}

int grapple_gizmotype_id = -1;

static int Grapples_GetMaxGizmos(void *grapple) {
    UNIMPLEMENTED();
}

static void Grapples_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void Grapples_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Grapples_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Grapple_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Grapple_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Grapple_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Grapple_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Grapple_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Grapple_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void *Grapples_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Grapples_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Grapples_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Grapples_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *Grapples_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int Grapples_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *Grapples_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Grapple";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Grapples_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Grapples_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Grapples_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Grapples_Draw;
    addtype.fns.get_gizmo_name_fn = Grapple_GetGizmoName;
    addtype.fns.get_output_fn = Grapple_GetOutput;
    addtype.fns.get_output_name_fn = Grapple_GetOutputName;
    addtype.fns.get_num_outputs_fn = Grapple_GetNumOutputs;
    addtype.fns.activate_fn = Grapple_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Grapple_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Grapples_AllocateProgressData;
    addtype.fns.clear_progress_fn = Grapples_ClearProgress;
    addtype.fns.store_progress_fn = Grapples_StoreProgress;
    addtype.fns.reset_fn = Grapples_Reset;
    addtype.fns.reserve_buffer_space_fn = Grapples_ReserveBufferSpace;
    addtype.fns.load_fn = Grapples_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    grapple_gizmotype_id = type_id;

    return &addtype;
}
