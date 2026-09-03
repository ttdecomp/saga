#include "legoapi/gizmos/door/zipups.h"

#include "batman.h"
#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

extern "C" void NewTerrPlatformsOff(void);
f32 GameShadow(GameObject_s *object, NUVEC *position, f32 probe_height, i32 terrain_mask);
void FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation);

struct ZIPUPPROGRESS {
    u32 active_mask;
    u32 visible_mask;
};

i32 zipup_gizmotype_id = -1;
static char zipup_outputName[] = "Active";
static NUVEC ZipUpHookOffset = {0.0f, -0.1143f, -0.0859f};

static i32 ZipUps_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_zipups;
}

static void ZipUps_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    for (i32 index = 0; index < world->zipup_count; ++index) {
        ZIPUP *zipup = &world->zipups[index];
        if (NuStrLen(zipup->name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, zipup);
        }
    }
}

static void ZipUps_Update(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL) {
        return;
    }

    CanDrawZipUpSwirls = 0;
    if (world->zipups == NULL) {
        return;
    }

    CanDrawZipUpSwirls = AvailableToPlayer(1u << 20, -1, 0, 0);
    ZIPUP *zipup = world->zipups;
    for (i32 index = 0; index < world->zipup_count; ++index, ++zipup) {
        GameObject_s *occupant = zipup->occupant;
        if (occupant != NULL && occupant->build_context == -1) {
            occupant->context_flags &= ~(1 << 5);
            occupant->field_0x788 = NULL;
            zipup->runtime_flags &= ~ZIPUP_RUNTIME_FLAG_OCCUPIED;
            zipup->occupant = NULL;
        }
    }
}

static void ZipUps_Draw(void *world_ptr, void *, float) {
    enum ZIPUP_SPECIALS {
        ZIPUP_SPECIAL_HOOK = 83,
        ZIPUP_SPECIAL_GROUND_SWIRL = 85,
        ZIPUP_SPECIAL_GROUND_MARKER = 86,
        ZIPUP_SPECIAL_ENDPOINT_SWIRL = 229,
    };
    enum ZIPUP_DRAW_MATRICES {
        ZIPUP_MATRIX_HOOK,
        ZIPUP_MATRIX_LOWER_MARKER,
        ZIPUP_MATRIX_UPPER_MARKER,
        ZIPUP_MATRIX_LOWER_SWIRL,
        ZIPUP_MATRIX_UPPER_SWIRL,
        ZIPUP_MATRIX_COUNT,
    };

    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world != NULL && world->zipups != NULL) {
        const u16 spin_angle = static_cast<u16>(NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0f * 65536.0f);
        const f32 pulse_phase = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0f;
        const f32 pulse_alpha = NuTrigTable[(static_cast<i32>(pulse_phase) >> 1) & 0x7fff] * 0.2f + 0.8f;

        NUVEC endpoint_position;
        NUVEC offset;
        NUVEC ground_position;
        NUMTX matrices[ZIPUP_MATRIX_COUNT];

        ZIPUP *zipup = world->zipups;
        for (i32 index = 0; index < world->zipup_count; ++index, ++zipup) {
            if ((zipup->flags & (ZIPUP_FLAG_ACTIVE | ZIPUP_FLAG_VISIBLE)) != (ZIPUP_FLAG_ACTIVE | ZIPUP_FLAG_VISIBLE)) {
                continue;
            }

            if ((zipup->flags & ZIPUP_FLAG_CONFIG_5) != 0) {
                if (world->lev_objs[ZIPUP_SPECIAL_HOOK].active != 0) {
                    offset.x = 0.0f;
                    offset.y = 0.84f;
                    offset.z = 0.9229f;

                    NuVecRotateY(&offset, &offset, zipup->facing_angle);
                    NuVecAdd(&endpoint_position, &zipup->lower_position, &offset);
                    NuMtxSetTranslation(&matrices[ZIPUP_MATRIX_HOOK], &endpoint_position);
                    NuSpecialDrawAt(&world->lev_objs[ZIPUP_SPECIAL_HOOK].special, &matrices[ZIPUP_MATRIX_HOOK]);

                    if ((zipup->flags & ZIPUP_FLAG_CONFIG_3) != 0) {
                        NuVecRotateY(&offset, &offset, 0x8000);
                        NuVecAdd(&endpoint_position, &zipup->upper_position, &offset);
                        NuMtxSetTranslation(&matrices[ZIPUP_MATRIX_HOOK], &endpoint_position);
                        NuSpecialDrawAt(&world->lev_objs[ZIPUP_SPECIAL_HOOK].special, &matrices[ZIPUP_MATRIX_HOOK]);
                    }
                }

                if (world->lev_objs[ZIPUP_SPECIAL_GROUND_MARKER].active != 0) {
                    NuMtxSetRotationY(&matrices[ZIPUP_MATRIX_LOWER_MARKER], spin_angle);
                    NuMtxRotateZ(&matrices[ZIPUP_MATRIX_LOWER_MARKER], zipup->lower_ground_z_rotation);
                    NuMtxRotateX(&matrices[ZIPUP_MATRIX_LOWER_MARKER], zipup->lower_ground_x_rotation);
                    ground_position.x = zipup->lower_position.x;
                    ground_position.y = zipup->lower_ground_height;
                    ground_position.z = zipup->lower_position.z;
                    NuMtxTranslate(&matrices[ZIPUP_MATRIX_LOWER_MARKER], &ground_position);
                    NuSpecialDrawAtAlpha(&world->lev_objs[ZIPUP_SPECIAL_GROUND_MARKER].special,
                                         &matrices[ZIPUP_MATRIX_LOWER_MARKER], pulse_alpha);
                }

                if ((zipup->flags & ZIPUP_FLAG_CONFIG_3) != 0 &&
                    world->lev_objs[ZIPUP_SPECIAL_GROUND_MARKER].active != 0) {
                    NuMtxSetRotationY(&matrices[ZIPUP_MATRIX_UPPER_MARKER], spin_angle);
                    NuMtxRotateZ(&matrices[ZIPUP_MATRIX_UPPER_MARKER], zipup->upper_ground_z_rotation);
                    NuMtxRotateX(&matrices[ZIPUP_MATRIX_UPPER_MARKER], zipup->upper_ground_x_rotation);
                    ground_position.x = zipup->upper_position.x;
                    ground_position.y = zipup->upper_ground_height;
                    ground_position.z = zipup->upper_position.z;
                    NuMtxTranslate(&matrices[ZIPUP_MATRIX_UPPER_MARKER], &ground_position);
                    NuSpecialDrawAtAlpha(&world->lev_objs[ZIPUP_SPECIAL_GROUND_MARKER].special,
                                         &matrices[ZIPUP_MATRIX_UPPER_MARKER], pulse_alpha);
                }
                continue;
            }

            if ((zipup->flags & ZIPUP_FLAG_CONFIG_4) != 0 && world->lev_objs[ZIPUP_SPECIAL_HOOK].active != 0) {
                EnableShadowMapRendering(0);
                NuMtxSetRotationX(&matrices[ZIPUP_MATRIX_HOOK], zipup->hook_x_rotation);
                NuMtxRotateY(&matrices[ZIPUP_MATRIX_HOOK], zipup->hook_y_rotation);
                NuMtxTranslate(&matrices[ZIPUP_MATRIX_HOOK], &zipup->hook_origin);
                NuSpecialDrawAt(&world->lev_objs[ZIPUP_SPECIAL_HOOK].special, &matrices[ZIPUP_MATRIX_HOOK]);
                ResetShadowMapRendering();
            }

            if (CanDrawZipUpSwirls == 0 || (zipup->flags & ZIPUP_FLAG_CONFIG_1) == 0 ||
                world->lev_objs[ZIPUP_SPECIAL_ENDPOINT_SWIRL].active == 0) {
                continue;
            }

            NuMtxSetTranslation(&matrices[ZIPUP_MATRIX_HOOK], &zipup->lower_position);
            NuSpecialDrawAtAlpha(&world->lev_objs[ZIPUP_SPECIAL_ENDPOINT_SWIRL].special, &matrices[ZIPUP_MATRIX_HOOK],
                                 1.0f);

            if ((zipup->flags & ZIPUP_FLAG_CONFIG_3) != 0) {
                NuMtxSetTranslation(&matrices[ZIPUP_MATRIX_HOOK], &zipup->upper_position);
                NuSpecialDrawAtAlpha(&world->lev_objs[ZIPUP_SPECIAL_ENDPOINT_SWIRL].special,
                                     &matrices[ZIPUP_MATRIX_HOOK], 1.0f);
            }

            if ((zipup->flags & ZIPUP_FLAG_CONFIG_2) == 0) {
                continue;
            }

            if (world->lev_objs[ZIPUP_SPECIAL_GROUND_SWIRL].active != 0) {
                NuMtxSetRotationY(&matrices[ZIPUP_MATRIX_LOWER_SWIRL], spin_angle);
                NuMtxRotateZ(&matrices[ZIPUP_MATRIX_LOWER_SWIRL], zipup->lower_ground_z_rotation);
                NuMtxRotateX(&matrices[ZIPUP_MATRIX_LOWER_SWIRL], zipup->lower_ground_x_rotation);
                ground_position.x = zipup->lower_position.x;
                ground_position.y = zipup->lower_ground_height;
                ground_position.z = zipup->lower_position.z;
                NuMtxTranslate(&matrices[ZIPUP_MATRIX_LOWER_SWIRL], &ground_position);
                NuSpecialDrawAtAlpha(&world->lev_objs[ZIPUP_SPECIAL_GROUND_SWIRL].special,
                                     &matrices[ZIPUP_MATRIX_LOWER_SWIRL], pulse_alpha);
            }

            if ((zipup->flags & ZIPUP_FLAG_CONFIG_3) != 0 && world->lev_objs[ZIPUP_SPECIAL_GROUND_SWIRL].active != 0) {
                NuMtxSetRotationY(&matrices[ZIPUP_MATRIX_UPPER_SWIRL], spin_angle);
                NuMtxRotateZ(&matrices[ZIPUP_MATRIX_UPPER_SWIRL], zipup->upper_ground_z_rotation);
                NuMtxRotateX(&matrices[ZIPUP_MATRIX_UPPER_SWIRL], zipup->upper_ground_x_rotation);
                ground_position.x = zipup->upper_position.x;
                ground_position.y = zipup->upper_ground_height;
                ground_position.z = zipup->upper_position.z;
                NuMtxTranslate(&matrices[ZIPUP_MATRIX_UPPER_SWIRL], &ground_position);
                NuSpecialDrawAtAlpha(&world->lev_objs[ZIPUP_SPECIAL_GROUND_SWIRL].special,
                                     &matrices[ZIPUP_MATRIX_UPPER_SWIRL], pulse_alpha);
            }
        }
    }
}

static char *ZipUp_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<ZIPUP *>(gizmo->object)->name : NULL;
}

static i32 ZipUp_GetOutput(GIZMO *gizmo, i32, i32) {
    return gizmo != NULL && (static_cast<ZIPUP *>(gizmo->object)->flags & ZIPUP_FLAG_ACTIVE) != 0;
}

static char *ZipUp_GetOutputName(GIZMO *, i32) {
    return zipup_outputName;
}

static i32 ZipUp_GetNumOutputs(GIZMO *) {
    return 1;
}

static void ZipUp_Activate(GIZMO *gizmo, i32 active) {
    ZIPUP *zipup = static_cast<ZIPUP *>(gizmo->object);
    i32 active_flag = 0;
    if (active != 0) {
        zipup->direction = 0;
        active_flag = 1;
    }
    zipup->flags = static_cast<u8>((zipup->flags & ~ZIPUP_FLAG_ACTIVE) | (active_flag << 6));
}

static i32 ZipUp_ActivateRev(GIZMO *gizmo, i32 active, i32 query) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }

    ZIPUP *zipup = static_cast<ZIPUP *>(gizmo->object);
    if ((query & 1) != 0) {
        return zipup->direction != active;
    }
    if (active != 0) {
        zipup->direction = 1;
        zipup->flags &= ~ZIPUP_FLAG_ACTIVE;
    } else {
        zipup->direction = 0;
        zipup->flags |= ZIPUP_FLAG_ACTIVE;
    }
    return 1;
}

static void ZipUp_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL) {
        ZIPUP *zipup = static_cast<ZIPUP *>(gizmo->object);
        const u8 visible_flag = visible != 0;
        zipup->flags = static_cast<u8>((zipup->flags & ~ZIPUP_FLAG_VISIBLE) | (visible_flag << 7));
    }
}

static void *ZipUps_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(ZIPUPPROGRESS));
}

static void ZipUps_ClearProgress(void *, void *progress_data) {
    ZIPUPPROGRESS *progress = (ZIPUPPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->active_mask = ~0u;
    progress->visible_mask = ~0u;
}

static void ZipUps_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    ZIPUPPROGRESS *progress = static_cast<ZIPUPPROGRESS *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    progress->active_mask = ~0u;
    progress->visible_mask = ~0u;
    if (world == NULL || world->zipups == NULL) {
        return;
    }

    for (i32 index = 0; index < world->zipup_count && index < 32; ++index) {
        const u32 mask = 1u << index;
        if ((world->zipups[index].flags & ZIPUP_FLAG_VISIBLE) == 0) {
            progress->visible_mask &= ~mask;
        }
        if ((world->zipups[index].flags & ZIPUP_FLAG_ACTIVE) == 0) {
            progress->active_mask &= ~mask;
        }
    }
}

void ZipUps_Reset(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    ZIPUPPROGRESS *progress = static_cast<ZIPUPPROGRESS *>(progress_ptr);
    if (world == NULL) {
        return;
    }

    ZIPUP *zipup = world->zipups;
    if (zipup != NULL) {
        for (i32 index = 0; index < WORLD->zipup_count; ++index, ++zipup) {
            NuVecRotateX(&zipup->hook_position, &ZipUpHookOffset, zipup->hook_x_rotation);
            NuVecRotateY(&zipup->hook_position, &zipup->hook_position, zipup->hook_y_rotation);
            NuVecAdd(&zipup->hook_position, &zipup->hook_origin, &zipup->hook_position);

            NewTerrPlatformsOff();
            f32 ground_height = GameShadow(NULL, &zipup->lower_position, 5.0f, -1);
            if (ground_height != -78.0f) {
                zipup->lower_ground_height = ground_height + 0.005f;
                FindAnglesZX(&ShadNorm, &zipup->lower_ground_x_rotation, &zipup->lower_ground_z_rotation);
            } else {
                zipup->lower_ground_height = 2000000.0f;
            }

            NewTerrPlatformsOff();
            ground_height = GameShadow(NULL, &zipup->upper_position, 5.0f, -1);
            if (ground_height != -78.0f) {
                zipup->upper_ground_height = ground_height + 0.005f;
                FindAnglesZX(&ShadNorm, &zipup->upper_ground_x_rotation, &zipup->upper_ground_z_rotation);
            } else {
                zipup->upper_ground_height = 2000000.0f;
            }

            zipup->facing_angle = static_cast<u16>(NuAtan2D(zipup->upper_position.x - zipup->lower_position.x,
                                                            zipup->upper_position.z - zipup->lower_position.z));
            zipup->runtime_flags &= ~ZIPUP_RUNTIME_FLAG_OCCUPIED;
            zipup->occupant = NULL;
            zipup->flags |= ZIPUP_FLAG_ACTIVE | ZIPUP_FLAG_VISIBLE;

            if (progress != NULL && index <= 31) {
                const u32 bit = 1u << index;
                const u8 visible = (progress->visible_mask & bit) != 0;
                zipup->flags = static_cast<u8>((zipup->flags & ~ZIPUP_FLAG_VISIBLE) | (visible << 7));
                const u8 active = (progress->active_mask & bit) != 0;
                zipup->flags = static_cast<u8>((zipup->flags & ~ZIPUP_FLAG_ACTIVE) | (active << 6));
            }
        }
    }

    if (world->current_level != NULL && world->current_level == CLOUDCITYESCAPEC_LDATA && world->zipup_count > 0) {
        world->zipups[0].lower_ground_height += 0.005f;
    }
}

static void *ZipUps_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    void *reserved_space = NULL;
    world->zipups = NULL;
    world->zipup_count = 0;
    if (world->current_level->max_zipups != 0) {
        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
        world->zipups = reinterpret_cast<ZIPUP *>(world->giz_buffer.addr);
        world->giz_buffer.addr += world->current_level->max_zipups * sizeof(ZIPUP);
        reserved_space = world->zipups;
    }
    return reserved_space;
}

static i32 ZipUps_Load(void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world->zipup_count != 0) {
        return 0;
    }

    const i32 version = EdFileReadInt();
    world->zipup_count = EdFileReadInt();
    for (i32 index = 0; index < world->zipup_count; ++index) {
        EdFileRead(world->zipups[index].name, sizeof(world->zipups[index].name));
        EdFileReadNuVec(&world->zipups[index].lower_position);
        EdFileReadNuVec(&world->zipups[index].hook_origin);
        EdFileReadNuVec(&world->zipups[index].upper_position);
        world->zipups[index].hook_x_rotation = EdFileReadUnsignedShort();
        world->zipups[index].hook_y_rotation = EdFileReadUnsignedShort();

        const u8 config_0 = EdFileReadUnsignedChar() != 0;
        world->zipups[index].flags = static_cast<u8>((world->zipups[index].flags & ~ZIPUP_FLAG_CONFIG_0) | config_0);
        const u8 config_1 = EdFileReadUnsignedChar() != 0;
        world->zipups[index].flags =
            static_cast<u8>((world->zipups[index].flags & ~ZIPUP_FLAG_CONFIG_1) | (config_1 << 1));
        const u8 config_3 = EdFileReadUnsignedChar() != 0;
        world->zipups[index].flags =
            static_cast<u8>((world->zipups[index].flags & ~ZIPUP_FLAG_CONFIG_3) | (config_3 << 3));

        if (version <= 1) {
            world->zipups[index].flags |= ZIPUP_FLAG_CONFIG_4;
            world->zipups[index].flags &= ~ZIPUP_FLAG_CONFIG_5;
            world->zipups[index].flags |= ZIPUP_FLAG_CONFIG_2;
        } else {
            const u8 config_4 = EdFileReadUnsignedChar() != 0;
            world->zipups[index].flags =
                static_cast<u8>((world->zipups[index].flags & ~ZIPUP_FLAG_CONFIG_4) | (config_4 << 4));
            if (version == 2) {
                world->zipups[index].flags &= ~ZIPUP_FLAG_CONFIG_5;
                world->zipups[index].flags |= ZIPUP_FLAG_CONFIG_2;
            } else {
                const u8 config_5 = EdFileReadUnsignedChar() != 0;
                world->zipups[index].flags =
                    static_cast<u8>((world->zipups[index].flags & ~ZIPUP_FLAG_CONFIG_5) | (config_5 << 5));
                if (version == 3) {
                    world->zipups[index].flags |= ZIPUP_FLAG_CONFIG_2;
                } else {
                    const u8 config_2 = EdFileReadUnsignedChar() != 0;
                    world->zipups[index].flags =
                        static_cast<u8>((world->zipups[index].flags & ~ZIPUP_FLAG_CONFIG_2) | (config_2 << 2));
                }
            }
        }
    }
    return 1;
}

ADDGIZMOTYPE *ZipUps_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "ZipUp";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = ZipUps_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = ZipUps_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = ZipUps_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = ZipUps_Draw;
    addtype.fns.get_gizmo_name_fn = ZipUp_GetGizmoName;
    addtype.fns.get_output_fn = ZipUp_GetOutput;
    addtype.fns.get_output_name_fn = ZipUp_GetOutputName;
    addtype.fns.get_num_outputs_fn = ZipUp_GetNumOutputs;
    addtype.fns.activate_fn = ZipUp_Activate;
    addtype.fns.activate_rev_fn = ZipUp_ActivateRev;
    addtype.fns.set_visibility_fn = ZipUp_SetVisibility;
    addtype.fns.allocate_progress_data_fn = ZipUps_AllocateProgressData;
    addtype.fns.clear_progress_fn = ZipUps_ClearProgress;
    addtype.fns.store_progress_fn = ZipUps_StoreProgress;
    addtype.fns.reset_fn = ZipUps_Reset;
    addtype.fns.reserve_buffer_space_fn = ZipUps_ReserveBufferSpace;
    addtype.fns.load_fn = ZipUps_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    zipup_gizmotype_id = type_id;

    return &addtype;
}
