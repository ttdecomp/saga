#include "legoapi/gizmos/door/push.h"

#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/numem.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/numath/nuvec.h"

#include <string.h>

struct PUSHPROGRESS {
    u32 visible_mask;
    u32 state_mask;
    u32 position_mask;
    NUVEC positions[16];
    NUVEC end_positions[2][16];
};
DECOMP_ASSERT(sizeof(PUSHPROGRESS) == 0x24c, "push-block progress size");

enum PUSHBLOCK_CONFIG_FLAGS {
    PUSHBLOCK_FLAG_VISIBLE = 1 << 2,
    PUSHBLOCK_FLAG_STATE = 1 << 1,
};

enum PUSHBLOCK_RUNTIME_FLAGS {
    PUSHBLOCK_RUNTIME_MOVING = 1 << 0,
    PUSHBLOCK_RUNTIME_SEEKING = 1 << 1,
    PUSHBLOCK_RUNTIME_RESET_POSITION = 1 << 2,
    PUSHBLOCK_RUNTIME_RESET_HEIGHT = 1 << 3,
};

i32 pushblock_gizmotype_id;

extern "C" char *NuIToA(i32 value, char *buffer, i32 radix);
void ResetPushProgress(WORLDINFO_s *world, void *progress);
i32 GizPushBlock_EndFrameCompleted(pushblock_s *push_block, i32 output_index);
void GizObstacles_AddTrigger(NUVEC *position);
void MoveBlocks(WORLDINFO_s *world, pushblock_s *push_block, i32 index, NUVEC *velocity);
void PushSeekComplete(pushblock_s *push_block, i32 snap_index);

static i32 Push_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    return world != NULL ? world->current_level->max_push_blocks : 0;
}

static void Push_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    for (i32 index = 0; index < world->push_block_count; ++index) {
        if (NuStrLen(world->push_blocks[index].name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, &world->push_blocks[index]);
        }
    }
}

void UpdatePushBlocks(void *world_ptr, void *, float) {
    static f32 snap_time = 0.25f;

    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->push_block_count == 0 || world->push_blocks == NULL) {
        return;
    }

    snap_time = 0.25f;
    for (i32 index = 0; index < world->push_block_count; ++index) {
        pushblock_s *push_block = &world->push_blocks[index];
        const u8 visible = NuSpecialGetVisibilityFn(&push_block->special) != 0;
        push_block->flags_0ca = static_cast<u8>((push_block->flags_0ca & ~PUSHBLOCK_FLAG_VISIBLE) | (visible << 2));
        if (visible == 0) {
            continue;
        }

        GizObstacles_AddTrigger(push_block->position);
        if ((push_block->runtime_flags_0c8 & 0x81) == 0x81) {
            push_block->runtime_flags_0c8 &= 0x3f;
        }
        if (static_cast<i8>(push_block->runtime_flags_0c8) < 0) {
            push_block->runtime_flags_0c8 |= 0x40;
        }

        if ((push_block->runtime_flags_0c9 & PUSHBLOCK_RUNTIME_MOVING) == 0 &&
            (push_block->flags_0cb & PUSHBLOCK_FLAG_STATE) == 0 &&
            (push_block->runtime_flags_0c9 & PUSHBLOCK_RUNTIME_SEEKING) != 0) {
            const i32 snap_index = ((push_block->runtime_flags_0c9 >> 7) & 1) | ((push_block->flags_0ca & 3) << 1);
            if (push_block->snap_timer <= snap_time && push_block->snap_positions != NULL &&
                push_block->position != NULL) {
                f32 fraction = push_block->snap_timer / snap_time;
                if (fraction > 1.0f) {
                    fraction = 1.0f;
                }
                NUVEC *target = &push_block->snap_positions[snap_index];
                push_block->position->x =
                    push_block->snap_origin.x + (target->x - push_block->snap_origin.x) * fraction;
                push_block->position->y =
                    push_block->snap_origin.y + (target->y - push_block->snap_origin.y) * fraction;
                push_block->position->z =
                    push_block->snap_origin.z + (target->z - push_block->snap_origin.z) * fraction;
                NuSpecialUpdate(&push_block->special);
                for (i32 end_index = 0; end_index < push_block->end_position_count; ++end_index) {
                    NUMTX *matrix = NuSpecialGetInstanceMtx(&push_block->end_position_specials[end_index]);
                    if (matrix != NULL) {
                        matrix->m30 = push_block->end_position_origins[end_index].x +
                                      (target->x - push_block->end_position_origins[end_index].x) * fraction;
                        matrix->m32 = push_block->end_position_origins[end_index].z +
                                      (target->z - push_block->end_position_origins[end_index].z) * fraction;
                        NuSpecialUpdate(&push_block->end_position_specials[end_index]);
                    }
                }
                push_block->snap_timer += FRAMETIME;
            } else {
                PushSeekComplete(push_block, snap_index);
            }
        }
    }

    for (i32 index = 0; index < world->push_block_count; ++index) {
        pushblock_s *push_block = &world->push_blocks[index];
        if ((push_block->flags_0ca & PUSHBLOCK_FLAG_VISIBLE) != 0 &&
            (push_block->runtime_flags_0c9 & PUSHBLOCK_RUNTIME_MOVING) == 0 &&
            (push_block->flags_0cb & PUSHBLOCK_FLAG_STATE) == 0 && (push_block->runtime_flags_0c8 & 0x69) != 0) {
            MoveBlocks(world, push_block, index, &push_block->velocity);
        }
    }

    for (i32 index = 0; index < world->push_block_count; ++index) {
        pushblock_s *push_block = &world->push_blocks[index];
        if ((push_block->flags_0ca & PUSHBLOCK_FLAG_VISIBLE) == 0 ||
            (push_block->flags_0cb & PUSHBLOCK_FLAG_STATE) != 0) {
            continue;
        }
        if ((push_block->runtime_flags_0c9 & PUSHBLOCK_RUNTIME_RESET_POSITION) != 0 && push_block->position != NULL) {
            push_block->position->x = push_block->snap_origin.x;
            push_block->position->z = push_block->snap_origin.z;
            push_block->velocity = v000;
            push_block->target_velocity = v000;
            push_block->runtime_flags_0c8 &= 0x3f;
            push_block->runtime_flags_0c9 &= ~PUSHBLOCK_RUNTIME_RESET_POSITION;
            NuSpecialUpdate(&push_block->special);
        }
        if ((push_block->runtime_flags_0c9 & PUSHBLOCK_RUNTIME_RESET_HEIGHT) != 0 && push_block->position != NULL) {
            push_block->ground_height = push_block->settled_height;
            push_block->position->y = push_block->settled_height - push_block->bounds_min.y;
            push_block->velocity = v000;
            push_block->runtime_flags_0c8 &= ~PUSHBLOCK_RUNTIME_RESET_HEIGHT;
            push_block->runtime_flags_0c9 &= ~PUSHBLOCK_RUNTIME_RESET_HEIGHT;
            NuSpecialUpdate(&push_block->special);
        }
    }
}

static char *GizPush_GetGizmoName(GIZMO *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return static_cast<pushblock_s *>(gizmo->object)->name;
}

i32 GizPush_GetOutput(GIZMO *gizmo, i32 output_index, i32) {
    pushblock_s *push_block = static_cast<pushblock_s *>(gizmo->object);
    if (push_block == NULL) {
        return 0;
    }
    return GizPushBlock_EndFrameCompleted(push_block, output_index) != 0;
}

char *GizPush_GetOutputName(GIZMO *gizmo, i32 output_index) {
    static char output_name[13] = "Any Complete";

    pushblock_s *push_block = static_cast<pushblock_s *>(gizmo->object);
    if (output_index < 0 || output_index > push_block->output_count) {
        return NULL;
    }
    if (output_index == 0) {
        return const_cast<char *>("Any Complete");
    }
    NuIToA(output_index, output_name, 10);
    NuStrCat(output_name, " Complete");
    return output_name;
}

i32 GizPush_GetNumOutputs(GIZMO *gizmo) {
    return static_cast<pushblock_s *>(gizmo->object)->output_count;
}

static void Push_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void Push_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 Pushblocks_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static void *Push_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, 0x24c);
}

static void Push_ClearProgress(void *, void *progress_ptr) {
    PUSHPROGRESS *progress = static_cast<PUSHPROGRESS *>(progress_ptr);
    if (progress != NULL) {
        progress->state_mask = 0;
        progress->visible_mask = ~0u;
        progress->position_mask = 0;
        NuMemSet128(progress->positions, 0, 0);
    }
}

static void Push_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    PUSHPROGRESS *progress = static_cast<PUSHPROGRESS *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    Push_ClearProgress(NULL, progress);
    if (world == NULL || world->push_blocks == NULL || world->push_block_count <= 0) {
        return;
    }

    for (i32 index = 0; index < world->push_block_count && index < 16; ++index) {
        pushblock_s *push_block = &world->push_blocks[index];
        const u32 mask = 1u << (index & 31);
        const i32 mask_index = index >> 5;
        u32 *visible_masks = &progress->visible_mask;
        u32 *state_masks = &progress->state_mask;
        u32 *position_masks = &progress->position_mask;

        if ((push_block->flags_0ca & PUSHBLOCK_FLAG_VISIBLE) == 0) {
            visible_masks[mask_index] &= ~mask;
        }
        if ((push_block->flags_0cb & PUSHBLOCK_FLAG_STATE) != 0) {
            state_masks[mask_index] |= mask;
        }

        NUMTX *matrix =
            NuSpecialExistsFn(&push_block->special) != 0 ? NuSpecialGetInstanceMtx(&push_block->special) : NULL;
        progress->positions[index] = matrix != NULL ? *reinterpret_cast<NUVEC *>(&matrix->m30) : v000;

        for (i32 end_index = 0; end_index < push_block->end_position_count; ++end_index) {
            nuhspecial_s *special = &push_block->end_position_specials[end_index];
            matrix = NuSpecialExistsFn(special) != 0 ? NuSpecialGetInstanceMtx(special) : NULL;
            progress->end_positions[end_index][index] =
                matrix != NULL ? *reinterpret_cast<NUVEC *>(&matrix->m30) : v000;
        }
        position_masks[mask_index] |= mask;
    }
}

static void Push_Reset(void *world, void *, void *progress) {
    ResetPushProgress(static_cast<WORLDINFO_s *>(world), progress);
}

static void *PushBlocks_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    world->push_blocks = NULL;
    world->push_block_count = 0;
    if (world->current_level->max_push_blocks != 0) {
        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
        world->push_blocks = static_cast<pushblock_s *>(world->giz_buffer.void_ptr);
        world->giz_buffer.addr += world->current_level->max_push_blocks * sizeof(pushblock_s);
    }
    return world->push_blocks;
}

static i32 edpush_Load(void *world_ptr, void *) {
    static i32 version = -1;

    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world->push_block_count != 0) {
        return 0;
    }

    version = EdFileReadInt();
    world->push_block_count = EdFileReadInt();
    i32 valid_count = 0;
    char special_name[256];
    for (i32 file_index = 0; file_index < world->push_block_count; ++file_index) {
        pushblock_s *push_block = &world->push_blocks[valid_count];
        memset(special_name, 0, sizeof(special_name));
        EdFileRead(special_name, EdFileReadChar());

        i32 special_missing = 1;
        if (Gizmo_FindNuSpecial(world->current_gscn, &push_block->special, special_name, 1, world->gizmo_sys,
                                const_cast<char *>("PushBlock"), push_block->name) != 0) {
            NuStrCpy(push_block->name, NuSpecialGetName(&push_block->special));
            special_missing = 0;
        }

        push_block->snap_distance = EdFileReadFloat();
        push_block->flags_0cb &= ~0x04;
        if (EdFileReadChar() != 0) {
            push_block->flags_0cb |= 0x04;
        }
        push_block->flags_0cb &= ~0x08;
        if (EdFileReadChar() != 0) {
            push_block->flags_0cb |= 0x08;
        }
        push_block->flags_0ca &= ~0x10;
        if (EdFileReadChar() != 0) {
            push_block->flags_0ca |= 0x10;
        }
        push_block->flags_0ca &= ~0x20;
        if (EdFileReadChar() != 0) {
            push_block->flags_0ca |= 0x20;
        }

        if (version > 3) {
            push_block->flags_0cb &= ~0x20;
            if (EdFileReadChar() != 0) {
                push_block->flags_0cb |= 0x20;
            }
            push_block->flags_0cb &= ~0x40;
            if (EdFileReadChar() != 0) {
                push_block->flags_0cb |= 0x40;
            }
        }
        if (version > 4) {
            push_block->flags_0ca &= ~0x40;
            if (EdFileReadChar() != 0) {
                push_block->flags_0ca |= 0x40;
            }
            push_block->flags_0ca &= ~0x80;
            if (EdFileReadChar() != 0) {
                push_block->flags_0ca |= 0x80;
            }
        }

        push_block->end_position_count = 0;
        if (version > 2) {
            const u8 end_position_count = static_cast<u8>(EdFileReadChar());
            push_block->end_position_count = end_position_count;
            for (i32 end_index = 0; end_index < end_position_count; ++end_index) {
                memset(special_name, 0, sizeof(special_name));
                EdFileRead(special_name, EdFileReadChar());
                special_name[16] = '\0';
                if (Gizmo_FindNuSpecial(world->current_gscn, &push_block->end_position_specials[end_index],
                                        special_name, 1, world->gizmo_sys, const_cast<char *>("PushBlockLinkObject"),
                                        push_block->name) == 0) {
                    --push_block->end_position_count;
                }
            }
        }
        if (special_missing == 0) {
            ++valid_count;
        }
    }
    world->push_block_count = valid_count;
    return 1;
}

ADDGIZMOTYPE *Push_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "PushBlocks";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x24c;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Push_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Push_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = Pushblocks_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = UpdatePushBlocks;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizPush_GetGizmoName;
    addtype.fns.get_output_fn = GizPush_GetOutput;
    addtype.fns.get_output_name_fn = GizPush_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizPush_GetNumOutputs;
    addtype.fns.activate_fn = Push_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Push_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Push_AllocateProgressData;
    addtype.fns.clear_progress_fn = Push_ClearProgress;
    addtype.fns.store_progress_fn = Push_StoreProgress;
    addtype.fns.reset_fn = Push_Reset;
    addtype.fns.reserve_buffer_space_fn = PushBlocks_ReserveBufferSpace;
    addtype.fns.load_fn = edpush_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    pushblock_gizmotype_id = type_id;

    return &addtype;
}
