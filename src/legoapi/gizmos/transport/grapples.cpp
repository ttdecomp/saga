#include "legoapi/gizmos/transport/grapples.h"

#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/world/world_shared.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

#include <stdio.h>

extern "C" void NewTerrPlatformsOff(void);
f32 GameShadow(GameObject_s *object, NUVEC *position, f32 probe_height, i32 terrain_mask);
void FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation);
void DrawRopeCurved(NUVEC *start, NUVEC *points, i32 point_count, i32 flags, NUMTL *material);
void EnableShadowMapRendering(i32 enabled);
void ResetShadowMapRendering();

extern i32 editor_active;

i32 grapple_gizmotype_id = -1;

struct GRAPPLEPROGRESS {
    u32 active_mask;
    u32 visible_mask;
};

static NUVEC GrapplePointOffset = {0.0f, -0.023f, 0.169f};
static GRAPPLE DynamicGrapple[4];

static void Grapple_ResetRopePoints(GRAPPLE *grapple) {
    const f32 segment_length = grapple->rope_length / 6.0f;
    NUVEC point = grapple->hook_position;
    for (i32 point_index = 0; point_index < 6; ++point_index) {
        point.y -= segment_length;
        grapple->target_rope_points[point_index] = point;
        grapple->rope_points[point_index] = point;
    }
}

static void Grapple_ResetDynamic(GRAPPLE *grapple, i32 index) {
    sprintf(grapple->name, "dynamic%i", index + 1);
    u8 flags = grapple->flags;
    flags |= GRAPPLE_FLAG_ACTIVE;
    flags &= ~GRAPPLE_FLAG_VISIBLE;
    grapple->y_rotation = 0;
    grapple->flags = flags;
    grapple->attached_object = NULL;
    grapple->has_terrain_platform = 1;
}

GameObject_s *Grapple_Occupied(GRAPPLE *grapple, GameObject_s *object, AIPATHCNX_s *connection);

static i32 Grapples_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_grapples;
}

static void Grapples_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    for (i32 index = 0; index < world->grapple_count; ++index) {
        GRAPPLE *grapple = &world->grapples[index];
        if (NuStrLen(grapple->name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, grapple);
        }
    }
}

static void Grapples_Update(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    i32 x_phase = 0;
    i32 z_phase = 0;

    GRAPPLE *grapple = world->grapples;
    for (i32 grapple_index = 0; grapple_index < world->grapple_count; ++grapple_index, ++grapple) {
        if ((grapple->flags & (GRAPPLE_FLAG_ACTIVE | GRAPPLE_FLAG_VISIBLE)) ==
            (GRAPPLE_FLAG_ACTIVE | GRAPPLE_FLAG_VISIBLE)) {
            if (grapple->activation_progress > 0.0f) {
                grapple->activation_progress -= FRAMETIME;
            }

            if (NuSpecialExistsFn(&grapple->display_specials[0]) != 0) {
                grapple->hook_position = *NuSpecialGetDrawPos(&grapple->display_specials[0]);
                grapple->shadow_probe_position.x = grapple->hook_position.x;
                grapple->shadow_probe_position.y = grapple->hook_position.y - 0.1f;
                grapple->shadow_probe_position.z = grapple->hook_position.z;
                grapple->ground_position.x = grapple->hook_position.x;
                grapple->ground_position.z = grapple->hook_position.z;
                grapple->ground_position.y = GameShadow(NULL, &grapple->hook_position, 5.0f, -1);
                if (grapple->ground_position.y != 2000000.0f) {
                    grapple->ground_position.y += 0.005f;
                    FindAnglesZX(&ShadNorm, &grapple->ground_x_rotation, &grapple->ground_z_rotation);
                }
            }

            if ((grapple->flags & GRAPPLE_FLAG_DISABLED) != 0) {
                GameObject_s *occupant = Grapple_Occupied(grapple, NULL, NULL);
                NUVEC attachment_position;

                if (occupant != NULL) {
                    PLAYERCHARACTERCONFIG_s *character_config = occupant->apiobj.character_data->player_config;
                    const NUVEC *attachment_source;
                    const i32 joint_a = character_config->grapple_joint_a;
                    if (joint_a == -1 || occupant->apiobj.character_model->points_of_interest[joint_a] == NULL) {
                        GRAPPLE *occupied_grapple = static_cast<GRAPPLE *>(occupant->field_0x788);
                        attachment_source = &occupant->apiobj.upper_position;
                        if ((occupied_grapple->flags & GRAPPLE_FLAG_DISABLED) != 0) {
                            attachment_source = &occupant->apiobj.collision_position;
                        }
                    } else {
                        attachment_source = NUMTX_GET_ROW_VEC(&occupant->joint_matrices[joint_a], 3);
                        const i32 joint_b = character_config->grapple_joint_b;
                        if (joint_b != -1 && occupant->apiobj.character_model->points_of_interest[joint_b] != NULL) {
                            attachment_position = *NUMTX_GET_ROW_VEC(&occupant->joint_matrices[joint_b], 3);
                            goto attachment_ready;
                        }
                    }
                    attachment_position = *attachment_source;
                attachment_ready:;
                } else {
                    const f32 sway = grapple->rope_length * 0.0125f;
                    const i32 x_index =
                        ((static_cast<u16>(NuFmod(GameTimer.time_elapsed, 1.5f) / 1.5f * 65536.0f) + x_phase) >> 1) &
                        0x7fff;
                    attachment_position.x = grapple->hook_position.x + NuTrigTable[x_index] * sway;
                    attachment_position.y = grapple->hook_position.y - grapple->rope_length * 0.333f;
                    const i32 z_index =
                        ((static_cast<u16>(NuFmod(GameTimer.time_elapsed, 1.217f) / 1.217f * 65536.0f) + z_phase) >>
                         1) &
                        0x7fff;
                    attachment_position.z = grapple->hook_position.z + NuTrigTable[z_index] * sway;
                }

                NUVEC direction;
                const f32 distance = NuVecDist(&attachment_position, &grapple->hook_position, &direction);
                f32 slack = 0.0f;
                f32 segment_offset = 0.0f;
                if (grapple->rope_length > distance) {
                    slack = grapple->rope_length - distance;
                    const f32 inverse_distance = 1.0f / distance;
                    direction.x *= inverse_distance;
                    direction.y *= inverse_distance;
                    direction.z *= inverse_distance;
                    segment_offset = slack / 6.0f;
                }

                for (i32 point_index = 0; point_index < 6; ++point_index) {
                    const f32 point_fraction = static_cast<f32>(point_index + 1) / 6.0f;
                    const f32 offset = segment_offset * static_cast<f32>(point_index + 1);
                    grapple->target_rope_points[point_index].x = attachment_position.x + direction.x * offset;
                    grapple->target_rope_points[point_index].y = attachment_position.y + direction.y * offset;
                    grapple->target_rope_points[point_index].z = attachment_position.z + direction.z * offset;

                    if (occupant != NULL && slack > 0.0f) {
                        const f32 wave_scale = 0.1f * slack * point_fraction;
                        const i32 x_index =
                            ((static_cast<u16>(NuFmod(GameTimer.time_elapsed, 1.5f) / 1.5f * 65536.0f) + x_phase) >>
                             1) &
                            0x7fff;
                        grapple->target_rope_points[point_index].x += NuTrigTable[x_index] * wave_scale;
                        const i32 z_index =
                            ((static_cast<u16>(NuFmod(GameTimer.time_elapsed, 1.217f) / 1.217f * 65536.0f) + z_phase) >>
                             1) &
                            0x7fff;
                        grapple->target_rope_points[point_index].z += NuTrigTable[z_index] * wave_scale;
                    }
                }

                SeekVec(&grapple->rope_points[0], &grapple->rope_points[0], &grapple->target_rope_points[0], 24.0f);
                SeekVec(&grapple->rope_points[1], &grapple->rope_points[1], &grapple->target_rope_points[1], 12.0f);
                SeekVec(&grapple->rope_points[2], &grapple->rope_points[2], &grapple->target_rope_points[2], 8.0f);
                SeekVec(&grapple->rope_points[3], &grapple->rope_points[3], &grapple->target_rope_points[3], 6.0f);
                SeekVec(&grapple->rope_points[4], &grapple->rope_points[4], &grapple->target_rope_points[4], 4.5f);
                SeekVec(&grapple->rope_points[5], &grapple->rope_points[5], &grapple->target_rope_points[5], 3.0f);
            }
        }

        x_phase += 0x2000;
        z_phase += 0x2666;
    }

    grapple = DynamicGrapple;
    GRAPPLE *dynamic_end = &DynamicGrapple[4];
    while (grapple != dynamic_end) {
        if (grapple->activation_progress > 0.0f) {
            grapple->activation_progress -= FRAMETIME;
        }

        GameObject_s *attached = grapple->attached_object;
        if (attached == NULL || grapple->retain_attachment != 0) {
            ++grapple;
            continue;
        }

        const u16 required_flags = 0x1001;
        if ((attached->apiobj.field_0x1f8 & required_flags) != required_flags || attached->apiobj.field_0x287 != 0) {
            grapple->attached_object = NULL;
            grapple->flags &= ~GRAPPLE_FLAG_VISIBLE;
            ++grapple;
            continue;
        }

        GameObject_s *linked_object = attached->field_0xcc0;
        if (linked_object == NULL || LEGOCONTEXT_BEENTAKENOVER == -1 ||
            linked_object->character_context != LEGOCONTEXT_BEENTAKENOVER) {
            grapple->flags &= ~GRAPPLE_FLAG_VISIBLE;
            ++grapple;
            continue;
        }

        grapple->position = attached->apiobj.lower_position;
        grapple->y_rotation = static_cast<u16>(attached->apiobj.field_0x276 + 0x8000);
        const u8 had_terrain_platform = grapple->has_terrain_platform;
        NuVecRotateX(&grapple->hook_position, &GrapplePointOffset, grapple->x_rotation);
        NuVecRotateY(&grapple->hook_position, &grapple->hook_position, grapple->y_rotation);
        NuVecAdd(&grapple->hook_position, &grapple->hook_position, &grapple->position);

        grapple->shadow_probe_position.x = grapple->hook_position.x;
        grapple->shadow_probe_position.y = grapple->hook_position.y - 0.1f;
        grapple->shadow_probe_position.z = grapple->hook_position.z;
        grapple->ground_position.x = grapple->hook_position.x;
        grapple->ground_position.z = grapple->hook_position.z;
        if (had_terrain_platform == 0) {
            NewTerrPlatformsOff();
        }
        grapple->ground_position.y = GameShadow(NULL, &grapple->hook_position, 5.0f, -1);
        if (grapple->ground_position.y != 2000000.0f) {
            grapple->ground_position.y += 0.005f;
            FindAnglesZX(&ShadNorm, &grapple->ground_x_rotation, &grapple->ground_z_rotation);
        }
        if (grapple->ground_position.y != 2000000.0f) {
            const f32 available_length = grapple->hook_position.y - (grapple->ground_position.y + 0.1f);
            if (grapple->rope_length > available_length) {
                if (0.7f > available_length) {
                    grapple->rope_length = 0.7f;
                } else {
                    grapple->rope_length = available_length;
                }
            }
        }

        const f32 segment_length = grapple->rope_length / 6.0f;
        NUVEC point = grapple->hook_position;
        for (i32 point_index = 0; point_index < 6; ++point_index) {
            point.y -= segment_length;
            grapple->target_rope_points[point_index] = point;
            grapple->rope_points[point_index] = point;
        }
        grapple->flags |= GRAPPLE_FLAG_ACTIVE | GRAPPLE_FLAG_VISIBLE;
        grapple->has_terrain_platform = 1;
        grapple->activation_progress = 0.0f;
        ++grapple;
    }
}

static __used__ void Grapples_DrawList(WORLDINFO *world, GRAPPLE *grapples, i32 grapple_count, u16 target_rotation,
                                       f32 target_alpha) {
    if (grapples == NULL || world == NULL || grapple_count <= 0) {
        return;
    }

    for (i32 index = 0; index < grapple_count; ++index) {
        GRAPPLE *grapple = &grapples[index];
        if ((grapple->flags & GRAPPLE_FLAG_VISIBLE) == 0) {
            continue;
        }

        if (NuSpecialExistsFn(&grapple->display_specials[0]) == 0 && LEGOOBJ_GRAPPLE_HOOK != -1 &&
            world->lev_objs[LEGOOBJ_GRAPPLE_HOOK].active != 0) {
            NUMTX hook_matrix;
            NuMtxSetRotationX(&hook_matrix, grapple->x_rotation);
            NuMtxRotateY(&hook_matrix, grapple->y_rotation);
            NuMtxTranslate(&hook_matrix, &grapple->position);
            NuSpecialDrawAt(&world->lev_objs[LEGOOBJ_GRAPPLE_HOOK].special, &hook_matrix);
        }

        if ((grapple->flags & GRAPPLE_FLAG_ACTIVE) == 0) {
            continue;
        }

        const u8 occupancy_flags = grapple->flags & (GRAPPLE_FLAG_DISABLED | GRAPPLE_FLAG_REVERSED);
        if (occupancy_flags == GRAPPLE_FLAG_DISABLED && Grapple_Occupied(grapple, NULL, NULL) == NULL) {
            DrawRopeCurved(&grapple->hook_position, grapple->rope_points, 6, 0, NULL);
        }

        if ((grapple->flags & GRAPPLE_FLAG_DISABLED) != 0 && editor_active == 0) {
            continue;
        }
        if (grapple->ground_position.y == 2000000.0f || LEGOOBJ_FLOORTARGET == -1 ||
            world->lev_objs[LEGOOBJ_FLOORTARGET].active == 0) {
            continue;
        }

        NUMTX target_matrix;
        NuMtxSetRotationY(&target_matrix, target_rotation);
        if (grapple->ground_z_rotation != 0) {
            NuMtxRotateZ(&target_matrix, grapple->ground_z_rotation);
        }
        if (grapple->ground_x_rotation != 0) {
            NuMtxRotateX(&target_matrix, grapple->ground_x_rotation);
        }
        NuMtxScaleU(&target_matrix, 0.8f);
        NuMtxTranslate(&target_matrix, &grapple->ground_position);

        f32 alpha = target_alpha;
        if (editor_active == 0) {
            GameObject_s *nearest_player;
            f32 distance_squared;
            if (!FindNearestPlayerToVec(&grapple->ground_position, &nearest_player, distance_squared, false, 0)) {
                continue;
            }

            const i32 distance_band = static_cast<i32>(distance_squared / 6.0f);
            const i32 fade_index = distance_band > 0 ? 0x2000 : (distance_band << 13) & 0x6000;
            alpha -= NuTrigTable[fade_index];
        }

        if (alpha > 0.0f) {
            NuSpecialDrawAtAlpha(&world->lev_objs[LEGOOBJ_FLOORTARGET].special, &target_matrix, alpha);
        }
    }
}

static void Grapples_Draw(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    const f32 rotation_time = NuFmod(GameTimer.time_elapsed, 5.0f);
    const f32 pulse_time = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0f;
    const f32 target_alpha = NuTrigTable[(static_cast<i32>(pulse_time) >> 1) & 0x7fff] * 0.2f + 0.8f;

    EnableShadowMapRendering(0);
    const u16 rotation = static_cast<u16>(rotation_time / 5.0f * 65536.0f);
    Grapples_DrawList(world, world->grapples, world->grapple_count, rotation, target_alpha);
    Grapples_DrawList(world, DynamicGrapple, 4, rotation, target_alpha);
    ResetShadowMapRendering();
}

static char *Grapple_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<GRAPPLE *>(gizmo->object)->name : NULL;
}

static i32 Grapple_GetOutput(GIZMO *gizmo, i32 output_index, i32) {
    GRAPPLE *grapple = static_cast<GRAPPLE *>(gizmo->object);
    if ((grapple->flags & (GRAPPLE_FLAG_ACTIVE | GRAPPLE_FLAG_VISIBLE)) !=
        (GRAPPLE_FLAG_ACTIVE | GRAPPLE_FLAG_VISIBLE)) {
        return 0;
    }
    if (output_index == 1 || output_index == 2) {
        return Grapple_Occupied(grapple, NULL, NULL) != 0;
    }
    return 1;
}

static char *Grapple_GetOutputName(GIZMO *, i32 output_index) {
    if (output_index == 1) {
        return const_cast<char *>("Occupied");
    }
    if (output_index == 2) {
        return const_cast<char *>("Occupied By 2");
    }
    return const_cast<char *>("Active");
}

static i32 Grapple_GetNumOutputs(GIZMO *) {
    return 3;
}

static void Grapple_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL) {
        GRAPPLE *grapple = static_cast<GRAPPLE *>(gizmo->object);
        grapple->flags = (grapple->flags & ~GRAPPLE_FLAG_ACTIVE) | (active != 0 ? GRAPPLE_FLAG_ACTIVE : 0);
        if ((grapple->flags & GRAPPLE_FLAG_ACTIVE) != 0) {
            grapple->activation_progress = 1.0f;
        }
    }
}

static void Grapple_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL) {
        GRAPPLE *grapple = static_cast<GRAPPLE *>(gizmo->object);
        const u8 visible_flag = visible != 0;
        grapple->flags = static_cast<u8>((grapple->flags & ~GRAPPLE_FLAG_VISIBLE) | (visible_flag << 1));
    }
}

static void *Grapples_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(GRAPPLEPROGRESS));
}

static void Grapples_ClearProgress(void *, void *progress_ptr) {
    GRAPPLEPROGRESS *progress = static_cast<GRAPPLEPROGRESS *>(progress_ptr);
    if (progress != NULL) {
        progress->active_mask = ~0u;
        progress->visible_mask = ~0u;
    }
}

static void Grapples_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GRAPPLEPROGRESS *progress = static_cast<GRAPPLEPROGRESS *>(progress_ptr);

    if (progress == NULL) {
        return;
    }

    progress->active_mask = ~0u;
    progress->visible_mask = ~0u;
    if (world == NULL || world->grapples == NULL) {
        return;
    }

    for (i32 index = 0; index < world->grapple_count && index < 32; ++index) {
        const u32 mask = 1u << index;
        if ((world->grapples[index].flags & GRAPPLE_FLAG_VISIBLE) == 0) {
            progress->visible_mask &= ~mask;
        }
        if ((world->grapples[index].flags & GRAPPLE_FLAG_ACTIVE) == 0) {
            progress->active_mask &= ~mask;
        }
    }
}

static void Grapples_Reset(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GRAPPLEPROGRESS *progress = static_cast<GRAPPLEPROGRESS *>(progress_ptr);
    if (world == NULL) {
        return;
    }

    GRAPPLE *grapple = world->grapples;
    if (grapple != NULL) {
        for (i32 index = 0; index < world->grapple_count; ++index, ++grapple) {
            const u8 has_terrain_platform = grapple->has_terrain_platform;
            NuVecRotateX(&grapple->hook_position, &GrapplePointOffset, grapple->x_rotation);
            NuVecRotateY(&grapple->hook_position, &grapple->hook_position, grapple->y_rotation);
            NuVecAdd(&grapple->hook_position, &grapple->hook_position, &grapple->position);

            const f32 shadow_y = grapple->hook_position.y - 0.1f;
            const f32 hook_x = grapple->hook_position.x;
            grapple->shadow_probe_position.x = hook_x;
            grapple->ground_position.x = hook_x;
            grapple->shadow_probe_position.y = shadow_y;
            const f32 hook_z = grapple->hook_position.z;
            grapple->shadow_probe_position.z = hook_z;
            grapple->ground_position.z = hook_z;

            if (has_terrain_platform == 0) {
                NewTerrPlatformsOff();
            }
            grapple->ground_position.y = GameShadow(NULL, &grapple->hook_position, 5.0f, -1);
            if (grapple->ground_position.y != 2000000.0f) {
                grapple->ground_position.y += 0.005f;
                FindAnglesZX(&ShadNorm, &grapple->ground_x_rotation, &grapple->ground_z_rotation);
            }
            if (grapple->ground_position.y != 2000000.0f) {
                const f32 available_length = grapple->hook_position.y - (grapple->ground_position.y + 0.1f);
                if (grapple->rope_length > available_length) {
                    grapple->rope_length = available_length < 0.7f ? 0.7f : available_length;
                }
            }

            Grapple_ResetRopePoints(grapple);
            grapple->flags |= GRAPPLE_FLAG_ACTIVE | GRAPPLE_FLAG_VISIBLE;
            grapple->has_terrain_platform = 0;
            grapple->activation_progress = 0.0f;

            if (progress != NULL && index <= 31) {
                const u32 bit = 1u << index;
                const u8 visible = (progress->visible_mask & bit) != 0;
                grapple->flags = static_cast<u8>((grapple->flags & ~GRAPPLE_FLAG_VISIBLE) | (visible << 1));
                const u8 active = (progress->active_mask & bit) != 0;
                grapple->flags = static_cast<u8>((grapple->flags & ~GRAPPLE_FLAG_ACTIVE) | active);
            }
        }
    }

    for (i32 index = 0; index < 4; ++index) {
        Grapple_ResetDynamic(&DynamicGrapple[index], index);
    }
}

static void *Grapples_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    void *reserved_space = NULL;
    world->grapples = NULL;
    world->grapple_count = 0;

    if (world->current_level->max_grapples != 0) {
        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
        world->grapples = reinterpret_cast<GRAPPLE *>(world->giz_buffer.addr);
        world->giz_buffer.addr += world->current_level->max_grapples * sizeof(GRAPPLE);
        reserved_space = world->grapples;
    }
    return reserved_space;
}

static i32 Grapples_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Grapples_RegisterGizmo(i32 type_id) {
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
