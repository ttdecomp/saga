#include "legoapi/gizmos/traps/gizturrets.h"

#include "decomp.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "legoapi/world/level.h"
#include "nu2api/nucore/nustring.h"

i32 turret_gizmotype_id = -1;

static i32 GizTurrets_GetMaxGizmos(void *turret) {
    WORLDINFO *world = static_cast<WORLDINFO *>(turret);
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_turrets;
}

static void GizTurrets_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *, void *data) {
    GIZTURRETSYS_s *turret_sys = static_cast<GIZTURRETSYS_s *>(data);
    if (turret_sys != NULL) {
        if (turret_sys->count != 0) {
            i32 i = 0;
            do {
                if (NuStrLen(turret_sys->turrets[i].name) != 0) {
                    AddGizmo(gizmo_sys, type_id, NULL, &turret_sys->turrets[i]);
                }
                ++i;
            } while (turret_sys->count > i);
        }
    }
}

static void GizTurrets_Update(void *, void *system_ptr, float frame_time) {
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    if (system == NULL || system->count == 0) {
        return;
    }

    GIZTURRET_s *turret = system->turrets;
    for (i32 index = 0; index < system->count; ++index, ++turret) {
        const u8 frame_flags = turret->flags;
        turret->flags = static_cast<u8>(frame_flags & ~GIZTURRET_FLAG_FIRED_THIS_FRAME);
        turret->runtime_flags &= ~GIZTURRET_RUNTIME_FLAG_ROTATION_SFX_PLAYING;

        const u8 update_mask = GIZTURRET_FLAG_VISIBLE | GIZTURRET_FLAG_ACTIVE | GIZTURRET_FLAG_UPDATE_DISABLED;
        if ((frame_flags & update_mask) == (GIZTURRET_FLAG_VISIBLE | GIZTURRET_FLAG_ACTIVE)) {
            turret->fire_cooldown -= frame_time;
            if (turret->fire_cooldown < 0.0f) {
                turret->fire_cooldown = 0.0f;
            }
            // Target selection, aiming, animation, and firing continue here in the original.
        }
    }
}

static void GizTurrets_Draw(void *world_ptr, void *system_ptr, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    if (system == NULL || system->count == 0) {
        return;
    }

    for (i32 index = 0; index < system->count; ++index) {
        GIZTURRET_s &turret = system->turrets[index];
        if ((turret.flags & GIZTURRET_FLAG_VISIBLE) == 0) {
            continue;
        }
        if (turret.room_id >= 0 && world->rooms_visible_ptr[turret.room_id] == 0) {
            continue;
        }
        if ((turret.animation_flags & GIZTURRET_ANIMATION_FLAG_DRAW_REFLECTION) == 0) {
            continue;
        }
        GameAnimSet_DrawReflection(turret.anim_set, 2, turret.reflection_alpha, NULL);
    }
}

static char *GizmoTurret_GetGizmoName(GIZMO *gizmo) {
    if (gizmo != NULL) {
        GIZTURRET_s *turret = static_cast<GIZTURRET_s *>(gizmo->object);
        return turret->name;
    }
    return NULL;
}

static i32 GizmoTurret_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizmoTurret_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizmoTurret_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void GizmoTurret_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoTurret_ActivateRev(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static void GizmoTurret_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static NUVEC *GizmoTurret_GetPos(GIZMO *gizmo) {
    if (gizmo != NULL) {
        GIZTURRET_s *turret = static_cast<GIZTURRET_s *>(gizmo->object);
        if (turret != NULL) {
            return &turret->position;
        }
    }
    return NULL;
}

static i32 GizTurrets_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static i32 *GizTurrets_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         i32, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizTurrets_BoltHit(void *, void *, void *, NUVEC *, i32, float, NUVEC *, NUVEC *, BOLT *, u32,
                              unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static void *GizTurrets_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, 0x70);
}

static void GizTurrets_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizTurrets_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZTURRETSYS_s *turret_sys = static_cast<GIZTURRETSYS_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, sizeof(GIZTURRETSYS_s)));

    turret_sys->capacity = world->current_level->max_turrets;
    turret_sys->turrets = static_cast<GIZTURRET_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, turret_sys->capacity * sizeof(GIZTURRET_s)));
    turret_sys->anim_pool =
        GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->unknown_0108, 4, turret_sys->capacity * 2);

    for (i32 i = 0; i < turret_sys->capacity; ++i) {
        turret_sys->turrets[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->unknown_0108, turret_sys->anim_pool, world->game_anim_sys);
    }
    world->giz_turret_sys = turret_sys;
    return turret_sys;
}

static i32 GizTurrets_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

static void GizTurrets_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_AddLevelSfx(void *, void *, i32 *, i32 *, i32) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizTurrets_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizTurret";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x70;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizTurrets_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoTurret_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizTurrets_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = GizTurrets_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = GizTurrets_GetBestBoltTarget;
    addtype.fns.late_update_fn = GizTurrets_Update;
    addtype.fns.bolt_hit_fn = GizTurrets_BoltHit;
    addtype.fns.draw_fn = GizTurrets_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoTurret_GetGizmoName;
    addtype.fns.get_output_fn = GizmoTurret_GetOutput;
    addtype.fns.get_output_name_fn = GizmoTurret_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoTurret_GetNumOutputs;
    addtype.fns.activate_fn = GizmoTurret_Activate;
    addtype.fns.activate_rev_fn = GizmoTurret_ActivateRev;
    addtype.fns.set_visibility_fn = GizmoTurret_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizTurrets_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizTurrets_ClearProgress;
    addtype.fns.store_progress_fn = GizTurrets_StoreProgress;
    addtype.fns.reset_fn = GizTurrets_Reset;
    addtype.fns.reserve_buffer_space_fn = GizTurrets_ReserveBufferSpace;
    addtype.fns.load_fn = GizTurrets_Load;
    addtype.fns.post_load_fn = GizTurrets_PostLoad;
    addtype.fns.add_level_sfx_fn = GizTurrets_AddLevelSfx;
    turret_gizmotype_id = type_id;

    return &addtype;
}
