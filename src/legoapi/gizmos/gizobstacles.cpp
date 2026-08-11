#include "legoapi/gizmos/gizobstacles.h"

#include "decomp.h"

struct GAMEANIMSET_s;

void GameAnimSet_SetVisibility(GAMEANIMSET_s *animation_set, i32 visible);
void GameAnimSet_JumpToStart(GAMEANIMSET_s *animation_set);
void GameAnimSet_EvaluateState(GAMEANIMSET_s *animation_set);
void GizObstacle_PlayBackwards(GIZOBSTACLE *obstacle);
void AddGameMessage(char *text, NUVEC *position, f32 duration, NUVEC *velocity, f32 scale, u8 red, u8 green, u8 blue,
                    u32 flags, f32 delay);

int obstacle_gizmotype_id = -1;
static NUVEC *gizobstacletriggers[16];
static i32 ngizobstacletriggers;

static void GizObstacles_AddTrigger(NUVEC *position) {
    if (position == NULL) {
        return;
    }
    if (ngizobstacletriggers >= 16) {
        NUVEC message_position = v001;
        AddGameMessage("MAXGIZOBSTACLETRIGGERS needs increasing, ask a programmer!", &message_position, 0.4f, NULL,
                       0.4f, 200, 200, 200, 0x2080, 0.0f);
        return;
    }
    gizobstacletriggers[ngizobstacletriggers++] = position;
}

static int GizObstacles_GetMaxGizmos(void *obstacle) {
    if (obstacle == NULL) {
        return 0;
    }
    u8 *level = *(u8 **)((u8 *)obstacle + 0x128);
    return level[0xfb];
}

static void GizObstacles_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizObstacles_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoObstacle_GetGizmoName(GIZMO *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return (char *)gizmo->object;
}

static int GizmoObstacle_GetOutput(GIZMO *gizmo, int output, int force) {
    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
    if (obstacle == NULL || ((obstacle->flags & 3) != 3 && force == 0) || obstacle->animation_set == NULL) {
        return 0;
    }

    i32 state = *(i32 *)((u8 *)obstacle->animation_set + 0xc);
    switch (output) {
        case 0:
            return state == 2;
        case 1:
            return state != 0;
        case 2:
            return obstacle->proximity != 0;
        case 3:
            return state == 0;
        case 4:
            return state == 1;
        default:
            return 0;
    }
}

static char *GizmoObstacle_GetOutputName(GIZMO *gizmo, int output_index) {
    switch (output_index) {
        case 0:
            return "AtEnd";
        case 1:
            return "NotAtStart";
        case 2:
            return "Proximity";
        case 3:
            return "AtStart";
        case 4:
            return "PlayingForward";
        default:
            return NULL;
    }
}

static int GizmoObstacle_GetNumOutputs(GIZMO *gizmo) {
    return 5;
}

static void GizmoObstacle_Activate(GIZMO *gizmo, int active) {
    if (gizmo == NULL) {
        return;
    }
    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
    if (active == 0) {
        obstacle->flags &= ~1;
        obstacle->proximity = 0;
        return;
    }
    if ((obstacle->flags & 1) == 0) {
        GameAnimSet_JumpToStart((GAMEANIMSET_s *)obstacle->animation_set);
    }
    obstacle->proximity = 0;
    obstacle->flags |= 1;
    obstacle->behavior_flags &= 0x7f;
    obstacle->reverse_flags &= ~1;
    GameAnimSet_EvaluateState((GAMEANIMSET_s *)obstacle->animation_set);
}

static int GizmoObstacle_ActivateRev(GIZMO *gizmo, int reverse, int flags) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }

    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
    if ((obstacle->reverse_flags & 1) != 0) {
        i32 state = *(i32 *)((u8 *)obstacle->animation_set + 0xc);
        if ((u32)(state - 1) <= 1) {
            obstacle->flags |= 1;
            obstacle->reverse_flags &= ~1;
        }
    }
    u8 behavior_flags = obstacle->behavior_flags;
    if ((behavior_flags & 4) != 0) {
        obstacle->reverse_flags &= ~1;
        obstacle->flags |= 1;
    }

    if ((flags & 1) == 0) {
        if (reverse == 0) {
            obstacle->reverse_flags &= ~1;
            obstacle->flags |= 1;
            return 1;
        }
        GizObstacle_PlayBackwards(obstacle);
        obstacle->reverse_flags |= 1;
        obstacle->flags &= ~1;
    } else {
        if (reverse == (obstacle->reverse_flags & 1)) {
            if ((flags & 4) == 0 || ((obstacle->flags >> 1) & 1) != reverse) {
                return 0;
            }
        }
        if (reverse == 1 && (behavior_flags & 4) != 0) {
            return 0;
        }
    }
    return 1;
}

static void GizmoObstacle_SetVisibility(GIZMO *gizmo, int visible) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
    GameAnimSet_SetVisibility((GAMEANIMSET_s *)obstacle->animation_set, visible);
    obstacle->flags = obstacle->flags & ~2 | (visible != 0) * 2;
}

static int GizmoObstacle_GetPos(GIZMO *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    return (i32)(usize) & ((GIZOBSTACLE *)gizmo->object)->position;
}

static int GizObstacles_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
}

static int *GizObstacles_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float,
                                           float, int, int, int) {
    UNIMPLEMENTED();
}

static int GizObstacles_BoltHit(void *, void *, void *, NUVEC *, int, float, NUVEC *, NUVEC *, BOLT *, unsigned int,
                                unsigned char *) {
    UNIMPLEMENTED();
}

i32 GizObstacles_TotalScore(void *world) {
    u8 *set = *(u8 **)((u8 *)world + 0x46a8);
    if (set == NULL) {
        return 0;
    }
    GIZOBSTACLE *obstacles = *(GIZOBSTACLE **)set;
    u16 count = *(u16 *)(set + 8);
    if (obstacles == NULL || count == 0) {
        return 0;
    }

    i32 total = 0;
    for (i32 i = 0; i < count; i++) {
        total += obstacles[i].score;
    }
    return total;
}

static void *GizObstacles_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizObstacles_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizObstacles_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int GizObstacles_Load(void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_AddLevelSfx(void *, void *, int *, int *, int) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizObstacles_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizObstacle";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x70;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizObstacles_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoObstacle_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizObstacles_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = GizObstacles_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = GizObstacles_GetBestBoltTarget;
    addtype.fns.late_update_fn = GizObstacles_Update;
    addtype.fns.bolt_hit_fn = GizObstacles_BoltHit;
    addtype.fns.draw_fn = GizObstacles_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoObstacle_GetGizmoName;
    addtype.fns.get_output_fn = GizmoObstacle_GetOutput;
    addtype.fns.get_output_name_fn = GizmoObstacle_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoObstacle_GetNumOutputs;
    addtype.fns.activate_fn = GizmoObstacle_Activate;
    addtype.fns.activate_rev_fn = GizmoObstacle_ActivateRev;
    addtype.fns.set_visibility_fn = GizmoObstacle_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizObstacles_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizObstacles_ClearProgress;
    addtype.fns.store_progress_fn = GizObstacles_StoreProgress;
    addtype.fns.reset_fn = GizObstacles_Reset;
    addtype.fns.reserve_buffer_space_fn = GizObstacles_ReserveBufferSpace;
    addtype.fns.load_fn = GizObstacles_Load;
    addtype.fns.post_load_fn = GizObstacles_PostLoad;
    addtype.fns.add_level_sfx_fn = GizObstacles_AddLevelSfx;
    obstacle_gizmotype_id = type_id;

    return &addtype;
}
