#include "legoapi/gizmos/gizmopickups.h"

#include "decomp.h"
#include "globals.h"
#include "legoapi/gameobject.h"
#include "legoapi/gamepads.h"
#include "legoapi/qrand.h"

void CollectPowerUp(GameObject_s *object, NUVEC *position, u16 powerup_id, i32 player_index);
void GizmoPickup_CollectCoin(WORLDINFO_s *world, NUVEC *position, i32 pickup_index, i32 coin_type, GameObject_s *object,
                             i32 player_index);
void CollectMinikit(NUVEC *position, char *pickup, i32 player_index);
void CollectHitPoint(GameObject_s *object, NUVEC *position, i32 player_index);
void GameAudio_PlaySfx(i32 sfx_id, NUVEC *position, i32 unknown_2, i32 unknown_3);

f32 (*GizmoPickups_Collide2DFn)(GameObject_s *object);

void Pup_CollectPowerUp(WORLDINFO_s *, GIZMOPICKUP *pickup, i32, GameObject_s *object, i32 player_index) {
    CollectPowerUp(object, &pickup->position, pickup->powerup_id, player_index);
}

void PowerUp_Update(GameObject_s *object) {
    if (object->player_packet.cheat_powerup_time > 0.0f) {
        object->player_packet.cheat_powerup_time -= FRAMETIME;
        if (object->player_packet.cheat_powerup_time <= 0.0f) {
            GameAudio_PlaySfx(0x52, NULL, 0, 0);
            return;
        }

        GameAudio_PlaySfx(0x51, NULL, 0, 0);
        ConstantRumble(object, QRAND_FLOAT() * 0.5f, 0.0f);
    }
}

void Pup_CollectCoin(WORLDINFO_s *world, GIZMOPICKUP *pickup, i32 pickup_index, GameObject_s *object,
                     i32 player_index) {
    GizmoPickup_CollectCoin(world, &pickup->position, pickup_index, pickup->coin_type, object, player_index);
}

void Pup_CollectMinikit(WORLDINFO_s *, GIZMOPICKUP *pickup, i32, GameObject_s *object, i32 player_index) {
    CollectMinikit(&pickup->position, (char *)pickup, player_index);
    GAMEPAD_s *game_pad = (GAMEPAD_s *)object->player_packet.game_pad;
    NewBuzz(game_pad->pad, 0.2f, 0);
}

void Pup_CollectHeart(WORLDINFO_s *, GIZMOPICKUP *pickup, i32, GameObject_s *object, i32 player_index) {
    CollectHitPoint(object, &pickup->position, player_index);
}

int gizmopickup_typeid = -1;

static int GizmoPickups_GetMaxGizmos(void *pickup) {
    UNIMPLEMENTED();
}

static void GizmoPickups_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizmoPickups_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizmoPickups_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoPickup_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizmoPickup_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizmoPickup_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizmoPickup_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizmoPickup_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GizmoPickup_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoPickup_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void *GizmoPickups_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizmoPickups_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizmoPickups_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizmoPickups_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizmoPickups_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int GizmoPickups_Load(void *, void *) {
    UNIMPLEMENTED();
}

void GizmoPickups_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizmoPickups_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizmoPickup";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x100;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizmoPickups_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoPickup_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizmoPickups_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizmoPickups_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizmoPickups_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoPickup_GetGizmoName;
    addtype.fns.get_output_fn = GizmoPickup_GetOutput;
    addtype.fns.get_output_name_fn = GizmoPickup_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoPickup_GetNumOutputs;
    addtype.fns.activate_fn = GizmoPickup_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizmoPickup_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizmoPickups_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizmoPickups_ClearProgress;
    addtype.fns.store_progress_fn = GizmoPickups_StoreProgress;
    addtype.fns.reset_fn = GizmoPickups_Reset;
    addtype.fns.reserve_buffer_space_fn = GizmoPickups_ReserveBufferSpace;
    addtype.fns.load_fn = GizmoPickups_Load;
    addtype.fns.post_load_fn = GizmoPickups_PostLoad;
    addtype.fns.add_level_sfx_fn = NULL;
    gizmopickup_typeid = type_id;

    return &addtype;
}
