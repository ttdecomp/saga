#include "legoapi/gizmos/door/spinner.h"

#include "batman.h"
#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/numtx.h"

#include <string.h>

extern "C" {
    void NewTerrPlatformsOff(void);
    i32 ShadowInfo(void);
    i32 NuSpecialCompare(nuhspecial_s *first, nuhspecial_s *second);
    char *NuIToA(i32 value, char *buffer, i32 radix);
    void NuRndrStartReflectionRender(i32 clear_depth);
    void NuRndrEndReflectionRender(void);
}

f32 GameShadow(GameObject_s *object, NUVEC *position, f32 probe_height, i32 terrain_mask);
i32 GizSpinner_GetState(GIZSPINNER_s *spinner);
i32 GizSpinner_Update(GIZSPINNER_s *spinner);
i32 GameAnimSet_IsAnimationReset(GAMEANIMSET_s *set);
i32 MatrixReflectionVU0_AXISY(NUMTX *matrix, f32 plane, f32 scale, NUMTX *result);

namespace {
    enum SPINNER_PROGRESS_FLAGS : u8 {
        SPINNER_PROGRESS_VALID = 0x01,
        SPINNER_PROGRESS_BASE_VISIBLE = 0x02,
        SPINNER_PROGRESS_ARM_VISIBLE = 0x04,
    };

    enum SPINNER_RUNTIME_FLAGS : u8 {
        SPINNER_RUNTIME_ANIMATION_HIDDEN = 0x20,
    };

    struct SPINNERARMRUNTIME {
        u32 field_0x00;
        NUMTX matrix;
    };

    struct SPINNERPROGRESSENTRY {
        f32 animation_position;
        u16 rotation;
        u8 flags;
        u8 field_0x07;
    };

    struct SPINNERPROGRESS {
        SPINNERPROGRESSENTRY entries[8];
    };

    DECOMP_ASSERT(sizeof(SPINNERPROGRESSENTRY) == 8, "spinner progress entry ABI");
    DECOMP_ASSERT(sizeof(SPINNERPROGRESS) == 0x40, "spinner progress ABI");
    DECOMP_ASSERT(sizeof(SPINNERARMRUNTIME) == 0x44, "spinner arm runtime ABI");

    SPINNERARMRUNTIME *GizSpinner_GetArms(GIZSPINNER_s *spinner) {
        return reinterpret_cast<SPINNERARMRUNTIME *>(spinner->field_0x0ad + 3);
    }
} // namespace

i32 spinner_gizmotype_id = -1;

static nuhspecial_s thingsSceneBase;
static nuhspecial_s thingsSceneArm;
static char gizspinner_outputName[14];

static i32 GizSpinner_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    return world->current_level->max_spinners;
}

static void GizSpinner_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world->spinners != NULL && world->current_level->max_spinners != 0) {
        i32 index = 0;
        do {
            if ((world->spinners[index].flags & GIZSPINNER_FLAG_VALID) != 0 &&
                NuStrLen(world->spinners[index].name) != 0) {
                AddGizmo(gizmo_sys, type_id, NULL, &world->spinners[index]);
            }
            ++index;
        } while (world->current_level->max_spinners > index);
    }
}

void GizSpinners_Update(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->spinners == NULL || world->current_level->max_spinners == 0) {
        return;
    }

    for (i32 index = 0; index < world->current_level->max_spinners; ++index) {
        GIZSPINNER_s *spinner = &world->spinners[index];
        if ((spinner->flags & GIZSPINNER_FLAG_VALID) == 0) {
            continue;
        }

        NUANG arm_angle = spinner->type == 0 ? 0 : static_cast<NUANG>(0x10000 / spinner->type);
        NUMTX matrix;
        NuMtxSetRotationY(&matrix, 0);
        NuMtxRotateY(&matrix, spinner->rotation + spinner->initial_rotation);
        NuMtxTranslate(&matrix, &spinner->position);
        spinner->matrix = matrix;

        SPINNERARMRUNTIME *arms = GizSpinner_GetArms(spinner);
        NUANG angle = 0;
        for (i32 arm = 0; arm < spinner->type; ++arm) {
            NuMtxSetRotationY(&matrix, 0);
            NuMtxRotateY(&matrix, spinner->rotation + spinner->initial_rotation);
            NuMtxRotateY(&matrix, angle);
            NuMtxTranslate(&matrix, &spinner->position);
            arms[arm].matrix = matrix;
            angle += arm_angle;
        }

        if (GameAnimSet_GetVisibility(spinner->anim_set) == GAMEANIMSET_VISIBILITY_NONE) {
            spinner->flags |= SPINNER_RUNTIME_ANIMATION_HIDDEN;
        } else {
            spinner->flags &= static_cast<u8>(~SPINNER_RUNTIME_ANIMATION_HIDDEN);
        }
        GizSpinner_Update(spinner);
    }
}

static void GizSpinner_Draw(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world->spinners == NULL || world->current_level->max_spinners == 0) {
        return;
    }

    for (i32 index = 0; index < world->current_level->max_spinners; ++index) {
        EnableShadowMapRendering(0);
        GIZSPINNER_s *spinner = &world->spinners[index];
        if ((spinner->flags & GIZSPINNER_FLAG_VALID) == 0 ||
            ((spinner->flags & GIZSPINNER_FLAG_HIDE_BASE) != 0 && editor_active == 0)) {
            continue;
        }

        bool special_exists = NuSpecialExistsFn(&spinner->special) != 0;
        if (special_exists) {
            NuSpecialDrawAt(&spinner->special, &spinner->matrix);
            if (spinner->type != 0 && NuSpecialCompare(&spinner->special, &thingsSceneBase) != 0 &&
                NuSpecialExistsFn(&thingsSceneArm) != 0) {
                SPINNERARMRUNTIME *arms = GizSpinner_GetArms(spinner);
                for (i32 arm = 0; arm < spinner->type; ++arm) {
                    NuSpecialDrawAt(&thingsSceneArm, &arms[arm].matrix);
                }
            }
        }

        ResetShadowMapRendering();
        if ((spinner->state_flags & GIZSPINNER_STATE_SHADOW_PLATFORM) == 0 || !special_exists ||
            NuSpecialGetVisibilityFn(&spinner->special) == 0) {
            continue;
        }

        NUMTX reflection_matrix;
        if (MatrixReflectionVU0_AXISY(&spinner->matrix, spinner->ground_height, world->current_level->unknown_0cc,
                                      &reflection_matrix) == 0) {
            continue;
        }
        NuRndrStartReflectionRender(0);
        NuSpecialDrawAt(&spinner->special, &reflection_matrix);
        if (spinner->type != 0 && NuSpecialCompare(&spinner->special, &thingsSceneBase) != 0 &&
            NuSpecialExistsFn(&thingsSceneArm) != 0) {
            SPINNERARMRUNTIME *arms = GizSpinner_GetArms(spinner);
            for (i32 arm = 0; arm < spinner->type; ++arm) {
                NuSpecialDrawAt(&thingsSceneArm, &arms[arm].matrix);
            }
        }
        NuRndrEndReflectionRender();
    }
}

static void GizSpinner_PanelDraw(void *, void *, float) {
}

static char *GizSpinner_GetGizmoName(GIZMO *gizmo) {
    if (gizmo != NULL) {
        return static_cast<GIZSPINNER_s *>(gizmo->object)->name;
    }
    return NULL;
}

i32 GizSpinner_GetOutput(GIZMO *gizmo, i32 output_index, i32) {
    GIZSPINNER_s *spinner = static_cast<GIZSPINNER_s *>(gizmo->object);
    i32 current_output = -1;
    if (spinner != NULL && spinner->anim_set != NULL && spinner->primary_anim_obj != NULL &&
        spinner->primary_anim_obj->instance_animation != NULL) {
        f32 animation_length = spinner->primary_anim_obj->end_frame - spinner->primary_anim_obj->start_frame + 1.0f;
        f32 margin = animation_length / 40.0f;
        f32 frame = spinner->primary_anim_obj->instance_animation->ltime;
        i32 last_output = spinner->output_count - 1;
        for (i32 output = 0; output < last_output; ++output) {
            f32 output_frame = spinner->animation_points[output + 1];
            if (output_frame + margin > frame && frame > output_frame - margin) {
                current_output = output;
                break;
            }
        }
        if (current_output == -1 && animation_length + margin > frame && frame > animation_length - margin) {
            current_output = last_output;
        }
    }
    return output_index == current_output;
}

char *GizSpinner_GetOutputName(GIZMO *gizmo, i32 output_index) {
    GIZSPINNER_s *spinner = static_cast<GIZSPINNER_s *>(gizmo->object);
    if (output_index < 0 || output_index > spinner->output_count) {
        return NULL;
    }
    if (spinner->output_count > 1) {
        NuIToA(static_cast<i32>((static_cast<f32>(output_index) / (spinner->output_count - 1)) * 100.0f),
               gizspinner_outputName, 10);
        NuStrCat(gizspinner_outputName, "%% Complete");
    } else {
        char *name = "100%% Complete";
        if (spinner->output_count == 0) {
            name = "";
        }
        NuStrCpy(gizspinner_outputName, name);
    }
    return gizspinner_outputName;
}

i32 GizSpinner_GetNumOutputs(GIZMO *gizmo) {
    return static_cast<GIZSPINNER_s *>(gizmo->object)->output_count;
}

void GizSpinner_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo == NULL) {
        return;
    }
    GIZSPINNER_s *spinner = static_cast<GIZSPINNER_s *>(gizmo->object);
    if (active != 0) {
        spinner->flags &= static_cast<u8>(~(GIZSPINNER_FLAG_HIDE_ARM | SPINNER_RUNTIME_ANIMATION_HIDDEN));
        spinner->state_flags &= ~0x20u;
    } else {
        spinner->flags |= GIZSPINNER_FLAG_HIDE_ARM;
    }
}

void GizSpinner_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static NUVEC *GizSpinner_GetPos(GIZMO *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    return &static_cast<GIZSPINNER_s *>(gizmo->object)->position;
}

i32 GizSpinner_UsingSpecial(GIZMO **, void *, i32, char *) {
    UNIMPLEMENTED();
    return 0;
}

static i32 GizSpinner_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static i32 *GizSpinner_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         i32, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizSpinner_HasBoltHit(void *, void *, void *, NUVEC *, i32, float, NUVEC *, NUVEC *, BOLT *, u32,
                                 unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static void *GizSpinner_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, 0x40);
}

static void GizSpinner_ClearProgress(void *, void *progress_ptr) {
    SPINNERPROGRESS *progress = static_cast<SPINNERPROGRESS *>(progress_ptr);
    if (progress != NULL) {
        memset(progress, 0, sizeof(*progress));
    }
}

static void GizSpinner_StoreProgressData(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    SPINNERPROGRESSENTRY *entry = static_cast<SPINNERPROGRESSENTRY *>(progress_ptr);
    if (entry == NULL || world->spinners == NULL || world->current_level->max_spinners == 0) {
        return;
    }

    for (i32 index = 0; index < world->current_level->max_spinners; ++index, ++entry) {
        GIZSPINNER_s *spinner = &world->spinners[index];
        if ((spinner->flags & GIZSPINNER_FLAG_VALID) == 0) {
            continue;
        }
        entry->flags |= SPINNER_PROGRESS_VALID;
        entry->rotation = spinner->previous_rotation;
        entry->flags =
            static_cast<u8>((entry->flags & ~SPINNER_PROGRESS_ARM_VISIBLE) |
                            (((spinner->flags & GIZSPINNER_FLAG_HIDE_ARM) == 0) ? SPINNER_PROGRESS_ARM_VISIBLE : 0));
        entry->flags =
            static_cast<u8>((entry->flags & ~SPINNER_PROGRESS_BASE_VISIBLE) |
                            (((spinner->flags & GIZSPINNER_FLAG_HIDE_BASE) == 0) ? SPINNER_PROGRESS_BASE_VISIBLE : 0));
        if (GameAnimSet_IsAnimationReset(spinner->anim_set) != 0) {
            entry->animation_position = 0.0f;
        } else if (spinner->anim_set->state == GAMEANIMSET_STATE_AT_END) {
            entry->animation_position = 1.0f;
        } else {
            entry->animation_position = GameAnimSet_GetAnimPos(spinner->primary_anim_obj);
        }
    }
}

static void GizSpinner_ResetAll(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    SPINNERPROGRESSENTRY *entry = static_cast<SPINNERPROGRESSENTRY *>(progress_ptr);
    if (world == NULL || world->spinners == NULL || entry == NULL) {
        return;
    }

    i32 index = 0;
    if (world->current_level->max_spinners != 0) {
        do {
            GIZSPINNER_s &spinner = world->spinners[index];
            if ((spinner.flags & GIZSPINNER_FLAG_VALID) != 0) {
                if (entry != NULL && (entry->flags & SPINNER_PROGRESS_VALID) != 0) {
                    spinner.rotation = entry->rotation;
                    if (entry->animation_position != 0.0f) {
                        GameAnimSet_JumpToAnimPos(spinner.anim_set, entry->animation_position);
                    } else {
                        GameAnimSet_JumpToEnd(spinner.anim_set);
                    }

                    if ((entry->flags & SPINNER_PROGRESS_ARM_VISIBLE) != 0) {
                        spinner.flags &= static_cast<u8>(~GIZSPINNER_FLAG_HIDE_ARM);
                    } else {
                        spinner.flags |= GIZSPINNER_FLAG_HIDE_ARM;
                    }
                    if ((entry->flags & SPINNER_PROGRESS_BASE_VISIBLE) != 0) {
                        spinner.flags &= static_cast<u8>(~GIZSPINNER_FLAG_HIDE_BASE);
                    } else {
                        spinner.flags |= GIZSPINNER_FLAG_HIDE_BASE;
                    }
                } else {
                    spinner.rotation = spinner.initial_rotation;
                    GameAnimSet_JumpToAnimPos(spinner.anim_set, 0.0f);
                }

                spinner.field_0x090 = 0.0f;
                spinner.room_index = -1;
                spinner.previous_rotation = spinner.rotation;
                spinner.target_rotation = spinner.rotation;
                spinner.state = static_cast<u8>(GizSpinner_GetState(&spinner));
                spinner.state_flags = (spinner.state_flags & ~0x300u) | GIZSPINNER_STATE_RESET;

                NuMtxSetRotationY(&spinner.matrix, spinner.initial_rotation);
                NuMtxTranslate(&spinner.matrix, &spinner.position);
                NewTerrPlatformsOff();

                if (NuSpecialGetMtx(&spinner.special) == NULL) {
                    spinner.ground_height = 2000000.0f;
                } else {
                    NUMTX *special_matrix = NuSpecialGetMtx(&spinner.special);
                    spinner.ground_height = GameShadow(NULL, NUMTX_GET_ROW_VEC(special_matrix, 3), 5.0f, -1);
                    if (spinner.ground_height != -78.0f && ShadowInfo() == 2) {
                        spinner.state_flags |= GIZSPINNER_STATE_SHADOW_PLATFORM;
                    }
                }
            }
            ++index;
            ++entry;
        } while (world->current_level->max_spinners > index);
    }

    GizSpinners_Update(world, NULL, FRAMETIME);
}

static void *GizSpinner_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    world->spinners = NULL;
    world->spinner_count = 0;
    if (world->current_level->max_spinners != 0) {
        world->spinner_anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->unknown_0108, 0,
                                                                world->current_level->max_spinner_anim_objs);

        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 16);
        world->spinners = static_cast<GIZSPINNER_s *>(world->giz_buffer.void_ptr);
        world->giz_buffer.addr += world->current_level->max_spinners * sizeof(GIZSPINNER_s);
        memset(world->spinners, 0, world->current_level->max_spinners * sizeof(GIZSPINNER_s));

        for (i32 index = 0; index < world->current_level->max_spinners; ++index) {
            world->spinners[index].anim_set = GameAnimSet_Create(&world->giz_buffer, &world->unknown_0108,
                                                                 world->spinner_anim_pool, world->game_anim_sys);
        }

        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
        return world->spinners;
    }
    return NULL;
}

static i32 GizSpinner_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Spinner_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Spinner";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x40;
    addtype.fns.early_update_fn = GizSpinners_Update;
    addtype.fns.panel_draw_fn = GizSpinner_PanelDraw;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizSpinner_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizSpinner_GetPos;
    addtype.fns.using_special_fn = GizSpinner_UsingSpecial;
    addtype.fns.add_gizmos_fn = GizSpinner_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = GizSpinner_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = GizSpinner_GetBestBoltTarget;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = GizSpinner_HasBoltHit;
    addtype.fns.draw_fn = GizSpinner_Draw;
    addtype.fns.get_gizmo_name_fn = GizSpinner_GetGizmoName;
    addtype.fns.get_output_fn = GizSpinner_GetOutput;
    addtype.fns.get_output_name_fn = GizSpinner_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizSpinner_GetNumOutputs;
    addtype.fns.activate_fn = GizSpinner_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizSpinner_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizSpinner_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizSpinner_ClearProgress;
    addtype.fns.store_progress_fn = GizSpinner_StoreProgressData;
    addtype.fns.reset_fn = GizSpinner_ResetAll;
    addtype.fns.reserve_buffer_space_fn = GizSpinner_ReserveBufferSpace;
    addtype.fns.load_fn = GizSpinner_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    spinner_gizmotype_id = type_id;

    return &addtype;
}
