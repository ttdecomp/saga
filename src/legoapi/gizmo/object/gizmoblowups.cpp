#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

static char gizmoblowupnametable[32][32];
static i32 gizmoblowupnametable_numids;

void GizmoBlowupDraw(void *, void *, float) {
}

void GizmoSortBlowups(WORLDINFO_s *) {
}

void GizmoSwapBlowups(GIZMOBLOWUP_s *, GIZMOBLOWUP_s *) {
}

void InitGizmoBlowups(WORLDINFO_s *) {
}

void GizBlowup_Respawn(GIZMOBLOWUP_s *) {
}

void GizmoBlowupBlowup(GIZMOBLOWUP_s *, i32, i32, i32, GameObject_s *, i32) {
}

void BlowupObjEmit_Stop(PART_s *) {
}

void GizmoBlowupTypeAdd(WORLDINFO_s *, nuhspecial_s *, i32, i32 *) {
}

void GizmoBlowup_FindType(char *, WORLDINFO_s *) {
}

void InitGizmoBlowupTypes(WORLDINFO_s *) {
}

void SetGizmoBlowUpTarget(GameObject_s *, GIZMOBLOWUP_s *) {
}

void GizBlowup_InitTerrain() {
}

void GizmoBlowupTypeRemove(GIZMOBLOWUPTYPE_s *, WORLDINFO_s *) {
}

void GizmoBlowup_HitBlowup(GameObject_s *, GIZMOBLOWUP_s *, i32, BOLT_s *, nuvec_s *, unsigned char *, u32, i32) {
}

void FindNearestGizmoBlowUp(WORLDINFO_s *, nuvec_s *, float) {
}

void GizmoBlowupCreateStuff(GIZMOBLOWUP_s *) {
}

void GizmoBlowupsFinalSetup(WORLDINFO_s *) {
}

void GizBlowup_DeleteTerrain() {
}

void GizmoBlowupTypeGetCount(WORLDINFO_s *) {
}

void GizmoBlowupUpdateMatrix(GIZMOBLOWUP_s *) {
}

void GizmoBlowups_TotalScore(void *) {
}

void GizmoBlowupTypeNameBlank(char *) {
}

void GizmoBlowupCheckProximity(WORLDINFO_s *, GameObject_s *) {
}

void GizmoBlowupGenDecalMatrix(GIZMOBLOWUP_s *, numtx_s *, i32) {
}

i32 GizmoBlowupGetNameTableId(char *name) {
    for (i32 id = 0; id < gizmoblowupnametable_numids; ++id) {
        if (NuStrICmp(gizmoblowupnametable[id], name) == 0) {
            return id;
        }
    }

    if (gizmoblowupnametable_numids >= 32) {
        return -1;
    }

    NuStrNCpy(gizmoblowupnametable[gizmoblowupnametable_numids], name, sizeof(gizmoblowupnametable[0]));
    return gizmoblowupnametable_numids++;
}

void InitGizmoBlowupsMtxBuffer(WORLDINFO_s *) {
}

void RemapTypeFlagToBlowupFlag(u32) {
}

void GizmoBlowupGenShadowMatrix(GIZMOBLOWUP_s *, numtx_s *) {
}

void GizBlowup_InitSingleTerrain(GIZMOBLOWUP_s *) {
}

void GizBlowup_DeleteSingleTerrain(GIZMOBLOWUP_s *) {
}

void GizmoBlowupVisibilityOverrides(WORLDINFO_s *) {
}

void GizmoBlowup_SetAutoSetReflectY(GIZMOBLOWUP_s *, nuvec_s *) {
}

extern void Transform_DrawTarget(NUVEC *position, f32 scale, f32 opacity);
extern i32 Transform_TargettedByObj(void *object);

void GizmoBlowup_TransformDraw_Game(GIZMOBLOWUP_s *blowup) {
    if (Transform_TargettedByObj(blowup) != 0) {
        return;
    }

    Transform_DrawTarget(&blowup->mid_position, 1.4f * blowup->target_scale, 0.4f);
}

void RemapAllTypeFlagsToBlowupFlags(u32) {
}

void GizmoBlowupTypeGetNameFromIndex(WORLDINFO_s *, i32) {
}

i32 GizmoBlowupGetTypeFromNameTableId(WORLDINFO_s *world, i32 name_id) {
    if (name_id < 0 || name_id >= gizmoblowupnametable_numids) {
        return -1;
    }

    const char *name = gizmoblowupnametable[name_id];
    for (i32 type_index = 0; type_index < world->gizmo_blowup_type_count; ++type_index) {
        if (NuStrICmp(world->gizmo_blowup_types[type_index].name, name) == 0) {
            return type_index;
        }
    }

    return -1;
}
