#include "decomp.h"
#include "batman.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/numtx.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

static char gizmoblowupnametable[32][32];
static i32 gizmoblowupnametable_numids;
i32 GizmoBlowup_HitMultiplier = 1;

void GizBlowup_DeleteTerrain();
void GizmoBlowupUpdateMatrix(GIZMOBLOWUP_s *blowup);

void GizmoBlowupDraw(void *, void *, float) {
}

void GizmoSortBlowups(WORLDINFO_s *) {
}

void GizmoSwapBlowups(GIZMOBLOWUP_s *, GIZMOBLOWUP_s *) {
}

i32 InitGizmoBlowups(WORLDINFO_s *world) {
    world->gizmo_blowups = NULL;
    world->gizmo_blowup_count = 0;
    if (world->current_level->max_gizmo_blowups == 0) {
        return 0;
    }

    world->giz_buffer.addr = (world->giz_buffer.addr + 0xf) & ~static_cast<usize>(0xf);
    world->gizmo_blowups = static_cast<GIZMOBLOWUP_s *>(GameBufferAlloc(
        &world->giz_buffer, &world->unknown_0108, world->current_level->max_gizmo_blowups * sizeof(GIZMOBLOWUP_s)));
    memset(world->gizmo_blowups, 0, world->current_level->max_gizmo_blowups * sizeof(GIZMOBLOWUP_s));
    return world->gizmo_blowups != NULL;
}

void GizBlowup_Respawn(GIZMOBLOWUP_s *) {
}

void GizmoBlowupBlowup(GIZMOBLOWUP_s *, i32, i32, i32, GameObject_s *, i32) {
}

void BlowupObjEmit_Stop(PART_s *) {
}

void GizmoBlowupTypeAdd(WORLDINFO_s *, nuhspecial_s *, i32, i32 *) {
}

GIZMOBLOWUPTYPE_s *GizmoBlowup_FindType(char *name, WORLDINFO_s *world) {
    if (world == NULL || world->gizmo_blowup_types == NULL || world->gizmo_blowup_type_count == 0 || name == NULL) {
        return NULL;
    }
    for (i32 index = 0; index < world->gizmo_blowup_type_count; ++index) {
        if (NuStrCmp(world->gizmo_blowup_types[index].name, name) == 0) {
            return &world->gizmo_blowup_types[index];
        }
    }
    return NULL;
}

i32 InitGizmoBlowupTypes(WORLDINFO_s *world) {
    world->gizmo_blowup_types = NULL;
    world->gizmo_blowup_type_count = 0;
    if (world->current_level->max_gizmo_blowup_types == 0) {
        return 0;
    }
    world->gizmo_blowup_types = static_cast<GIZMOBLOWUPTYPE_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108,
                        world->current_level->max_gizmo_blowup_types * sizeof(GIZMOBLOWUPTYPE_s)));
    return world->gizmo_blowup_types != NULL;
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

void GizmoBlowupsFinalSetup(WORLDINFO_s *world) {
    for (i32 type_index = 0; type_index < world->gizmo_blowup_type_count; ++type_index) {
        GIZMOBLOWUPTYPE_s *type = &world->gizmo_blowup_types[type_index];
        type->animation_runtime_flags &= ~1;
        nuinstanim_s *animation = NuSpecialGetInstAnim(&type->animated_special);
        type->animation_base_frame = animation != NULL ? animation->ltime : 0.0f;
        if (NuSpecialExistsFn(&type->decal_special) != 0) {
            NuSpecialSetVisibility(&type->decal_special, 0);
        }
    }

    for (i32 instance_index = 0; instance_index < world->gizmo_blowup_count; ++instance_index) {
        GIZMOBLOWUP_s *blowup = &world->gizmo_blowups[instance_index];
        blowup->animation_time = 1.0f;
        blowup->field_0x9f &= ~0x08;
        blowup->visibility_flags &= ~0x03;
        blowup->output_flags = 0;
        blowup->state_flags = (blowup->state_flags & 0xb7) | 0x81;

        nuhspecial_s *special = blowup->override_special;
        if (special == NULL || NuSpecialExistsFn(special) == 0) {
            special = &blowup->type->animated_special;
        }

        if (NuSpecialExistsFn(special) == 0) {
            blowup->bounds_min = v000;
            blowup->bounds_max = v000;
        } else {
            if (NuSpecialExistsFn(&blowup->type->burst_special) != 0) {
                NuSpecialSetVisibility(&blowup->type->burst_special, 0);
            }
            if ((blowup->type->animation_flags & 0x20) != 0) {
                blowup->position = *NuSpecialGetDrawPos(special);
            }

            NUVEC centre;
            NuSpecialGetRadius(special, &centre, &blowup->target_scale);
            centre.x += blowup->position.x;
            centre.y += blowup->position.y;
            centre.z += blowup->position.z;
            blowup->bounds_min.x = centre.x - blowup->target_scale;
            blowup->bounds_min.y = centre.y - blowup->target_scale;
            blowup->bounds_min.z = centre.z - blowup->target_scale;
            blowup->bounds_max.x = centre.x + blowup->target_scale;
            blowup->bounds_max.y = centre.y + blowup->target_scale;
            blowup->bounds_max.z = centre.z + blowup->target_scale;

            nuinstanim_s *animation = NuSpecialGetInstAnim(special);
            if (animation != NULL && animation->playing != 0) {
                const u8 original_state = blowup->state_flags;
                blowup->state_flags = original_state | 0x10;
                if (animation->repeating != 0) {
                    blowup->state_flags = original_state | 0x58;
                }
            }
        }

        GizmoBlowupUpdateMatrix(blowup);
        blowup->initial_state_1 = static_cast<u8>(blowup->initial_state_1 * GizmoBlowup_HitMultiplier);
    }

    GizBlowup_DeleteTerrain();
    GizBlowup_InitTerrain();
}

void GizBlowup_DeleteTerrain() {
}

void GizmoBlowupTypeGetCount(WORLDINFO_s *) {
}

void GizmoBlowupUpdateMatrix(GIZMOBLOWUP_s *blowup) {
    if (blowup == NULL) {
        return;
    }

    NUVEC position = blowup->position;
    blowup->transform = *NuSpecialGetDrawMtx(&blowup->type->animated_special);
    *NUMTX_GET_ROW_VEC(&blowup->transform, 3) = v000;
    NuMtxRotateY(&blowup->transform, blowup->field_0xf2);
    NuMtxPreRotateX(&blowup->transform, blowup->field_0xf0);
    NuMtxPreRotateY(&blowup->transform, blowup->field_0xf4);
    NuMtxTranslate(&blowup->transform, &position);
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

i32 InitGizmoBlowupsMtxBuffer(WORLDINFO_s *world) {
    world->gizmo_blowup_mtx_buffer = NULL;
    world->giz_buffer.addr = (world->giz_buffer.addr + 0x7f) & ~static_cast<usize>(0x7f);
    world->gizmo_blowup_mtx_buffer =
        static_cast<NUMTX *>(GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, 0x8000));
    return world->gizmo_blowup_mtx_buffer != NULL;
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
