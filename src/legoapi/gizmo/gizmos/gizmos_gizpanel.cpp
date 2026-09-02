#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

extern "C" f32 GIZPANEL_PLAYERPOSLIFT;
extern "C" {
    void DeletePlatinst(i32 platform_id);
    i16 NewPlatPickupInst(void *object, i32 object_type);
    void PlatInstRotate(i32 platform_id, i32 enabled);
}

f32 GameShadow(GameObject_s *object, NUVEC *position, f32 probe_height, i32 terrain_mask);
void FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation);
void GizPanel_GetAbsTargetPos(GIZPANEL_s *panel, nuvec_s *target_position, i32 player_position);

static __used__ void GizPanel_CreateTerrain(GIZPANEL_s *panel) {
    if (panel == NULL) {
        return;
    }
    if (panel->platform_id != -1) {
        DeletePlatinst(panel->platform_id);
    }

    switch (panel->model_variant) {
        case 0:
            panel->platform_id = NewPlatPickupInst(panel, 4);
            break;
        case 1:
            panel->platform_id = NewPlatPickupInst(panel, 7);
            break;
        case 2:
            panel->platform_id = NewPlatPickupInst(panel, 5);
            break;
        case 3:
            panel->platform_id = NewPlatPickupInst(panel, 6);
            break;
        default:
            break;
    }
    PlatInstRotate(panel->platform_id, 1);
}

void GizPanel_Use(GameObject_s &, GIZPANEL_s &) {
}

void GizPanel_Reset(GIZPANEL_s *panel) {
    NUVEC *floor_position = &panel->floor_position;
    floor_position->y = 0.0f;
    floor_position->x = 0.0f;
    floor_position->z = 0.4f;
    NuVecRotateY(floor_position, floor_position, static_cast<u16>(panel->y_rotation + 0x8000));
    NuVecAdd(floor_position, floor_position, &panel->position);

    NUVEC target_position;
    GizPanel_GetAbsTargetPos(panel, &target_position, 0);
    target_position.y = panel->position.y;
    floor_position->y = GameShadow(NULL, floor_position, 0.2f, -1);
    const f32 target_floor = GameShadow(NULL, &target_position, 0.2f, -1);
    panel->target_offset.y = target_floor;
    if (target_floor != -1.0f) {
        panel->target_offset.y = target_floor + GIZPANEL_PLAYERPOSLIFT;
        FindAnglesZX(&ShadNorm, &panel->target_pitch, &panel->target_roll);
    }

    panel->activation_time = 0.0f;
    panel->arm_x_rotation = 0;
    panel->flags =
        static_cast<GIZPANEL_FLAGS>((panel->flags & 0xfc) | GIZPANEL_FLAG_VISIBLE | GIZPANEL_FLAG_TRACK_PLAYER);
    NuMtxSetRotationY(&panel->matrix, panel->y_rotation);
    NuMtxTranslate(&panel->matrix, &panel->position);
}

void GizPanel_PlaySfx(char *, nuvec_s *, i32) {
}

void GizPanel_MoveCode(WORLDINFO_s *, GameObject_s *, i32) {
}

void GizPanel_BeingUsed(GIZPANEL_s *) {
}

void GizPanel_FindByName(WORLDINFO_s *, char *) {
}

void GizPanel_UpdateHint(HINT_s *) {
}

void GizPanel_CanUsePanel(GameObject_s *, GIZPANEL_s *) {
}

void GizPanel_FindNearest(WORLDINFO_s *, nuvec_s *, GameObject_s *, float *, i32) {
}

void GizPanel_InitTerrain(WORLDINFO_s *world) {
    GIZPANELSYS_s *panel_sys = world->giz_panel_sys;
    if (panel_sys == NULL || panel_sys->count <= 0) {
        return;
    }

    for (i32 index = 0; index < panel_sys->count; ++index) {
        GIZPANEL_s *panel = &panel_sys->panels[index];
        panel->platform_id = -1;
        GizPanel_CreateTerrain(panel);
        panel_sys = world->giz_panel_sys;
    }
}

void GizPanel_GetAbsPlayerPos(GIZPANEL_s *, nuvec_s *) {
}

void GizPanel_GetAbsTargetPos(GIZPANEL_s *panel, nuvec_s *target_position, i32 player_position) {
    if (panel == NULL || target_position == NULL) {
        return;
    }

    NUVEC offset;
    if (player_position != 0) {
        if (panel->model_variant == 0) {
            offset = {-0.04f, panel->target_offset.y, -0.3f};
        } else if (panel->model_variant == 1) {
            offset = {0.035f, panel->target_offset.y, -0.25f};
        } else {
            offset = panel->target_offset;
        }
    } else {
        offset = panel->target_offset;
    }

    NuVecRotateY(&offset, &offset, panel->y_rotation);
    *target_position = {
        offset.x + panel->position.x,
        offset.y,
        offset.z + panel->position.z,
    };
}

void GIZPANEL_s::ClearMechObjectInterface() {
}

void GIZPANEL_s::GetMechObjectInterface() {
}
