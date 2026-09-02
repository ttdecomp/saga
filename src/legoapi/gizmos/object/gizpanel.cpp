#include "legoapi/gizmos/object/gizpanel.h"

#include "decomp.h"
#include "batman.h"
#include "gameapi/edtools/edfile.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

#include <string.h>

struct GIZPANELPROGRESS {
    i32 state;
    i32 goodie_state;
    i32 baddie_state;
};

i32 gizpanel_gizmotype_id = -1;
extern "C" {
    f32 GIZPANEL_PLAYERPOSLIFT = 0.001f;
    f32 GIZPANEL_PICY = 0.0775f;
    f32 GIZPANEL_PICZ = -0.123f;
}

void FindAnglesXY(NUVEC *direction, u16 *x_rotation, u16 *y_rotation);
extern "C" {
    void DeletePlatinst(i32 platform_id);
    i16 NewPlatPickupInst(void *object, i32 object_type);
    void PlatInstRotate(i32 platform_id, i32 enabled);
}

static void GizPanel_CreateTerrain(GIZPANEL *panel) {
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

static i32 GizPanel_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    return world != NULL ? world->current_level->max_giz_panels : 0;
}

static void GizPanel_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZPANELSYS_s *panel_sys;
    if (world != NULL && (panel_sys = world->giz_panel_sys) != NULL && panel_sys->count > 0) {
        i32 index = 0;
        do {
            if (NuStrLen(panel_sys->panels[index].name) != 0) {
                AddGizmo(gizmo_sys, type_id, NULL, &world->giz_panel_sys->panels[index]);
            }
            panel_sys = world->giz_panel_sys;
            ++index;
        } while (panel_sys->count > index);
    }
}

static void GizPanel_Update(void *world_ptr, void *, float delta_time) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZPANELSYS_s *panel_sys = world->giz_panel_sys;
    if (panel_sys == NULL || panel_sys->count <= 0) {
        return;
    }

    GameObject_s **players = Player;
    for (i32 index = 0; index < panel_sys->count; ++index) {
        GIZPANEL &panel = panel_sys->panels[index];
        GameObject_s *nearest_player = NULL;
        NUVEC player_direction;

        if ((panel.flags & GIZPANEL_FLAG_TRACK_PLAYER) != 0) {
            const f32 maximum_distance = (panel.flags & GIZPANEL_FLAG_PLAYER_NEAR) != 0 ? 1.21f : 0.81f;
            f32 nearest_distance = 1000000000.0f;
            for (i32 player_index = 0; player_index < 8; ++player_index) {
                GameObject_s *candidate = players[player_index];
                if (candidate == NULL || (candidate->apiobj.flags_high & 0x10) == 0 ||
                    candidate->apiobj.field_0x287 != 0) {
                    continue;
                }

                NUVEC panel_position = panel.position;
                if (panel.model_variant == 2) {
                    panel_position.y += 0.245f;
                    panel_position.z -= 0.19f;
                } else if (panel.model_variant == 3) {
                    panel_position.y += 0.34f;
                    panel_position.z -= 0.17f;
                }

                NUVEC direction;
                const f32 distance = NuVecDistSqr(&panel_position, &candidate->apiobj.collision_position, &direction);
                if (distance < nearest_distance && distance < maximum_distance) {
                    nearest_distance = distance;
                    nearest_player = candidate;
                    player_direction = direction;
                }
            }
        }

        if (nearest_player == NULL) {
            panel.target_x_rotation = SeekRot(panel.target_x_rotation, 0xe000, 10.0f);
            panel.target_y_rotation = SeekRot(panel.target_y_rotation, 0, 10.0f);
            if ((panel.flags & GIZPANEL_FLAG_PLAYER_NEAR) != 0) {
                PlaySfx(const_cast<char *>("DPanelDis"), &panel.position);
            }
            panel.flags = static_cast<GIZPANEL_FLAGS>(panel.flags & ~GIZPANEL_FLAG_PLAYER_NEAR);
            if (panel.activation_time > 0.0f) {
                panel.activation_time -= delta_time;
                if (panel.activation_time < 0.0f) {
                    panel.activation_time = 0.0f;
                }
            }
            continue;
        }

        const u16 previous_x_rotation = panel.target_x_rotation;
        const u16 previous_y_rotation = panel.target_y_rotation;
        u16 target_x_rotation;
        u16 target_y_rotation;
        FindAnglesXY(&player_direction, &target_x_rotation, &target_y_rotation);
        target_y_rotation = static_cast<u16>(target_y_rotation - panel.y_rotation);

        if (previous_y_rotation <= 0x4000) {
            if (target_y_rotation > 0x4000 && target_y_rotation < 0xc000) {
                target_y_rotation = 0x4000;
            }
            if (previous_x_rotation <= 0x4000) {
                if (target_x_rotation > 0x4000 && target_x_rotation < 0xc000) {
                    target_x_rotation = static_cast<u16>(target_x_rotation + 0x4000);
                }
            } else if (previous_x_rotation > 0xbfff && target_x_rotation < 0xc000 && target_x_rotation > 0x4000) {
                target_x_rotation = 0xc000;
            }
        } else {
            if (previous_y_rotation >= 0xc000 && target_y_rotation <= 0xbfff && target_y_rotation >= 0x4001) {
                target_y_rotation = 0xc000;
            }
            if (previous_x_rotation <= 0x4000) {
                if (target_x_rotation > 0x4000 && target_x_rotation < 0xc000) {
                    target_x_rotation = static_cast<u16>(target_x_rotation + 0x4000);
                }
            } else if (previous_x_rotation > 0xbfff && target_x_rotation < 0xc000 && target_x_rotation > 0x4000) {
                target_x_rotation = 0xc000;
            }
        }

        panel.target_x_rotation = SeekRot(panel.target_x_rotation, target_x_rotation, 5.0f);
        panel.target_y_rotation = SeekRot(panel.target_y_rotation, target_y_rotation, 5.0f);
        if ((panel.flags & GIZPANEL_FLAG_PLAYER_NEAR) == 0) {
            panel.activation_time = 0.0f;
            panel.flags = static_cast<GIZPANEL_FLAGS>(panel.flags | GIZPANEL_FLAG_PLAYER_NEAR);
            PlaySfx(const_cast<char *>("DPanelAct"), &panel.position);
        } else {
            panel.activation_time += FRAMETIME;
            if (panel.activation_time > 0.8f) {
                panel.activation_time = 0.8f;
            }
        }
    }
}

static void GizPanel_Draw(void *world_ptr, void *, float) {
    struct PANEL_MODELS {
        i32 base;
        i32 animated;
        i32 animated_transition;
        i32 state_off;
        i32 state_on;
        i32 target;
        i32 arm;
        NUVEC arm_offset;
    };

    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZPANELSYS_s *panel_sys;
    if (world == NULL || (panel_sys = world->giz_panel_sys) == NULL || panel_sys->count == 0) {
        return;
    }

    const u16 target_spin = static_cast<u16>(NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0f * 65536.0f);
    const f32 pulse_phase = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0f;
    const f32 target_alpha = 0.8f + NuTrigTable[(static_cast<i32>(pulse_phase) >> 1) & 0x7fff] * 0.2f;

    for (i32 index = 0; index < panel_sys->count; ++index) {
        GIZPANEL &panel = panel_sys->panels[index];
        if ((panel.flags & GIZPANEL_FLAG_VISIBLE) == 0 && editor_active == 0) {
            continue;
        }

        TouchHacks::TintStack tint;
        panel.flash_timer -= FRAMETIME;
        const bool flashing = TouchHacks::ShouldFlash(panel.flash_timer);
        if (flashing) {
            NUCOLOUR3 *flash_colour = TouchHacks::GetFlashColour();
            NuRndrLightingStateCurrent.ambient = *flash_colour;
            NuRndrSetAmbientLightPS(flash_colour);
            NUVEC special_tint = {0.5f, 0.5f, 2.0f};
            NuSpecialConstTint(1, &special_tint);
        }

        if ((panel.draw_flags & GIZPANEL_DRAW_FLAG_NO_SHADOW_RESET) != 0) {
            ResetShadowMapRendering();
        } else {
            EnableShadowMapRendering(0);
        }

        PANEL_MODELS models = {-1, -1, -1, -1, -1, -1, -1, {0.0f, 0.0f, 0.0f}};
        const bool alternate = (panel.draw_flags & GIZPANEL_DRAW_FLAG_ALT_MODEL) != 0;
        switch (panel.model_variant) {
            case 0:
                models.base = alternate ? 0x107 : 0x106;
                models.animated = (panel.flags & GIZPANEL_FLAG_BADDIE) != 0 ? 0x109 : 0x108;
                models.animated_transition = 0x10a;
                models.state_off = 0x10c;
                models.state_on = 0x10b;
                models.target = 0x55;
                break;
            case 1:
                models.base = alternate ? 0x107 : 0x100;
                models.animated = (panel.flags & GIZPANEL_FLAG_BADDIE) != 0 ? 0x102 : 0x101;
                models.animated_transition = 0x103;
                models.state_off = 0x105;
                models.state_on = 0x104;
                models.target = 0x55;
                break;
            case 2:
                models.base = 0x10d;
                models.animated = 0x10e;
                models.animated_transition = 0x10f;
                models.state_off = 0x111;
                models.state_on = 0x110;
                models.target = 0x113;
                models.arm = 0x112;
                models.arm_offset = {0.0f, 0.245f, 0.19f};
                break;
            case 3:
                models.base = 0x114;
                models.animated = 0x115;
                models.animated_transition = 0x116;
                models.state_off = 0x118;
                models.state_on = 0x117;
                models.target = 0x11a;
                models.arm = 0x119;
                models.arm_offset = {0.0f, 0.34f, 0.17f};
                break;
            default:
                break;
        }

        NUMTX base_matrix;
        NuMtxSetRotationY(&base_matrix, panel.y_rotation);
        NuMtxTranslate(&base_matrix, &panel.position);
        if ((panel.flags & GIZPANEL_FLAG_HIDE_BASE) == 0 && models.base != -1 &&
            world->lev_objs[models.base].active != 0) {
            NuSpecialDrawAt(&world->lev_objs[models.base].special, &base_matrix);
        }

        const i32 state_model = (panel.flags & GIZPANEL_FLAG_STATE) != 0 ? models.state_on : models.state_off;
        if ((panel.flags & GIZPANEL_FLAG_HIDE_BASE) == 0 && state_model != -1 &&
            world->lev_objs[state_model].active != 0) {
            NuSpecialDrawAt(&world->lev_objs[state_model].special, &base_matrix);
        }

        if ((panel.flags & GIZPANEL_FLAG_HIDE_BASE) == 0 && (panel.activation_time > 0.0f || editor_active != 0) &&
            models.animated != -1) {
            NUVEC picture_offset = {0.0f, GIZPANEL_PICY, GIZPANEL_PICZ};
            NUVEC transition_offset = {0.0f, GIZPANEL_PICY, GIZPANEL_PICZ - 0.0005f};
            NuVecRotateY(&picture_offset, &picture_offset, panel.y_rotation);
            NuVecRotateY(&transition_offset, &transition_offset, panel.y_rotation);

            NUMTX animation_base = base_matrix;
            NUMTX transition_base = base_matrix;
            NuMtxTranslate(&animation_base, &picture_offset);
            NuMtxTranslate(&transition_base, &transition_offset);

            NUMTX animated_matrix;
            if (panel.activation_time < 1.2f && editor_active == 0) {
                const f32 transition_progress = panel.activation_time / 1.2f;
                EvalAnim(&world->lev_objs[models.animated_transition].special, transition_progress * 23.0f + 1.0f,
                         &animated_matrix, 0);
                animated_matrix.m30 = 0.0f;
                animated_matrix.m31 = 0.0f;
                animated_matrix.m32 = 0.0f;
                NuMtxMulVU0(&animated_matrix, &animated_matrix, &transition_base);
                NuSpecialDrawAtAlpha(&world->lev_objs[models.animated_transition].special, &animated_matrix,
                                     1.0f - transition_progress);
            }

            const f32 frame = panel.activation_time / 0.8f * 24.0f + 1.0f;
            EvalAnim(&world->lev_objs[models.animated].special, frame, &animated_matrix, 0);
            animated_matrix.m30 = 0.0f;
            animated_matrix.m31 = 0.0f;
            animated_matrix.m32 = 0.0f;
            NuMtxMulVU0(&animated_matrix, &animated_matrix, &animation_base);
            NuSpecialDrawAtAlpha(&world->lev_objs[models.animated].special, &animated_matrix, 1.0f);
        }

        if ((panel.flags & (GIZPANEL_FLAG_TRACK_PLAYER | GIZPANEL_FLAG_HIDE_TARGET)) == GIZPANEL_FLAG_TRACK_PLAYER &&
            panel.target_offset.y != 2000000.0f && models.target != -1 && world->lev_objs[models.target].active != 0) {
            NUVEC target_position;
            GizPanel_GetAbsTargetPos(&panel, &target_position, 0);
            NUMTX target_matrix;
            NuMtxSetRotationY(&target_matrix, target_spin);
            if (panel.target_roll != 0) {
                NuMtxRotateZ(&target_matrix, panel.target_roll);
            }
            if (panel.target_pitch != 0) {
                NuMtxRotateX(&target_matrix, panel.target_pitch);
            }
            NuMtxTranslate(&target_matrix, &target_position);
            NuMtxPreScaleU(&target_matrix, panel.target_scale);
            NuSpecialDrawAtAlpha(&world->lev_objs[models.target].special, &target_matrix,
                                 (panel.flags & 1) != 0 ? 0.0f : target_alpha);
        }

        if ((panel.flags & GIZPANEL_FLAG_HIDE_BASE) == 0 && models.arm != -1 &&
            world->lev_objs[models.arm].active != 0) {
            NUVEC arm_offset = models.arm_offset;
            NuVecRotateX(&arm_offset, &arm_offset, panel.arm_x_rotation);
            NuVecRotateY(&arm_offset, &arm_offset, panel.y_rotation);
            NUMTX arm_matrix;
            NuMtxSetRotationX(&arm_matrix, panel.arm_x_rotation);
            NuMtxRotateY(&arm_matrix, panel.y_rotation);
            NuMtxPreRotateX(&arm_matrix, panel.target_x_rotation);
            NuMtxRotateY(&arm_matrix, panel.target_y_rotation);
            NuMtxTranslate(&arm_matrix, &panel.position);
            NuMtxTranslate(&arm_matrix, &arm_offset);
            NuSpecialDrawAt(&world->lev_objs[models.arm].special, &arm_matrix);
        }
        if (flashing) {
            NuSpecialConstTint(0, NULL);
        }
    }
    ResetShadowMapRendering();
}

static char *GizPanel_GetGizmoName(GIZMO *gizmo) {
    if (gizmo != NULL && gizmo->object != NULL) {
        return static_cast<GIZPANEL *>(gizmo->object)->name;
    }
    return NULL;
}

static i32 GizPanel_GetOutput(GIZMO *gizmo, i32, i32) {
    u8 flags = static_cast<GIZPANEL *>(gizmo->object)->flags;
    if ((flags & (GIZPANEL_FLAG_VISIBLE | GIZPANEL_FLAG_TRACK_PLAYER)) !=
        (GIZPANEL_FLAG_VISIBLE | GIZPANEL_FLAG_TRACK_PLAYER)) {
        return 0;
    }
    flags >>= 1;
    return flags & 1;
}

static char *GizPanel_GetOutputName(GIZMO *, i32 output_index) {
    return output_index == 0 ? const_cast<char *>("Finished") : NULL;
}

static i32 GizPanel_GetNumOutputs(GIZMO *) {
    return 1;
}

static void GizPanel_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL && gizmo->object != NULL) {
        GIZPANEL *panel = static_cast<GIZPANEL *>(gizmo->object);
        if (active == 0) {
            panel->flags = static_cast<GIZPANEL_FLAGS>(panel->flags & ~GIZPANEL_FLAG_TRACK_PLAYER);
            return;
        }
        panel->flags = static_cast<GIZPANEL_FLAGS>(panel->flags | GIZPANEL_FLAG_TRACK_PLAYER);
        GizPanel_Reset(panel);
    }
}

static void GizPanel_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    GIZPANEL *panel = static_cast<GIZPANEL *>(gizmo->object);
    const bool was_visible = (panel->flags & GIZPANEL_FLAG_VISIBLE) != 0;
    panel->flags = static_cast<GIZPANEL_FLAGS>((panel->flags & ~GIZPANEL_FLAG_VISIBLE) |
                                               ((visible != 0) ? GIZPANEL_FLAG_VISIBLE : 0));
    if (visible != 0) {
        if (!was_visible) {
            GizPanel_CreateTerrain(panel);
        }
    } else if (was_visible) {
        DeletePlatinst(panel->platform_id);
    }
}

static void *GizPanels_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(GIZPANELPROGRESS));
}

static void GizPanels_ClearProgress(void *, void *progress_data) {
    GIZPANELPROGRESS *progress = (GIZPANELPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->state = 0;
    progress->goodie_state = -1;
    progress->baddie_state = -1;
}

static void GizPanels_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZPANELPROGRESS *progress = static_cast<GIZPANELPROGRESS *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    progress->state = 0;
    progress->goodie_state = -1;
    progress->baddie_state = -1;
    GIZPANELSYS_s *panel_sys;
    GIZPANEL *panel;
    if (world != NULL && (panel_sys = world->giz_panel_sys) != NULL && (panel = panel_sys->panels) != NULL &&
        panel_sys->count > 0) {
        i32 index = 0;
        do {
            const u32 bit = 1u << (index & 31);
            const i32 word_index = index >> 5;
            u8 flags = panel->flags;
            if ((flags & GIZPANEL_FLAG_STATE) != 0) {
                reinterpret_cast<u32 *>(&progress->state)[word_index] |= bit;
                flags = panel->flags;
            }
            if ((flags & GIZPANEL_FLAG_VISIBLE) == 0) {
                reinterpret_cast<u32 *>(&progress->baddie_state)[word_index] &= ~bit;
                flags = panel->flags;
            }
            if ((flags & GIZPANEL_FLAG_TRACK_PLAYER) == 0) {
                reinterpret_cast<u32 *>(&progress->goodie_state)[word_index] &= ~bit;
            }
            ++index;
            ++panel;
        } while (index != panel_sys->count && index != 32);
    }
}

static void GizPanels_Reset(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZPANELPROGRESS *progress = static_cast<GIZPANELPROGRESS *>(progress_ptr);
    GIZPANELSYS_s *panel_sys;
    GIZPANEL *panels;
    if (world != NULL && (panel_sys = world->giz_panel_sys) != NULL && (panels = panel_sys->panels) != NULL &&
        panel_sys->count > 0) {
        i32 index = 0;
        while (true) {
            GIZPANEL *panel = &panels[index];
            GizPanel_Reset(panel);
            if (index < 32 && progress != NULL) {
                const u32 bit = 1u << index;
                u8 flags = panel->flags;
                if ((progress->state & bit) != 0) {
                    flags |= GIZPANEL_FLAG_STATE;
                    panel->flags = static_cast<GIZPANEL_FLAGS>(flags);
                }
                const u8 visible = ((progress->baddie_state & bit) != 0) << 2;
                panel->flags = static_cast<GIZPANEL_FLAGS>((flags & ~GIZPANEL_FLAG_VISIBLE) | visible);
                panel->flags =
                    static_cast<GIZPANEL_FLAGS>((flags & ~(GIZPANEL_FLAG_TRACK_PLAYER | GIZPANEL_FLAG_VISIBLE)) |
                                                visible | (((progress->goodie_state & bit) != 0) << 3));
            }
            ++index;
            if (panel_sys->count <= index) {
                break;
            }
            panels = panel_sys->panels;
        }
    }
}

static void *GizPanels_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    world->giz_panel_sys = NULL;
    if (world->current_level->max_levers == 0) {
        return NULL;
    }

    world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 16);
    world->giz_panel_sys = static_cast<GIZPANELSYS_s *>(world->giz_buffer.void_ptr);
    world->giz_buffer.addr += sizeof(GIZPANELSYS_s);
    memset(world->giz_panel_sys, 0, sizeof(*world->giz_panel_sys));

    world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 16);
    world->giz_panel_sys->panels = static_cast<GIZPANEL *>(world->giz_buffer.void_ptr);
    world->giz_buffer.addr += world->current_level->max_giz_panels * sizeof(GIZPANEL);
    memset(world->giz_panel_sys->panels, 0, world->current_level->max_giz_panels * sizeof(GIZPANEL));
    return world->giz_panel_sys;
}

static i32 GizPanel_Load(void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL) {
        return 0;
    }
    GIZPANELSYS_s *panel_sys = world->giz_panel_sys;
    if (panel_sys == NULL || panel_sys->count != 0) {
        return 0;
    }
    const i32 version = EdFileReadInt();
    i32 index = 0;
    panel_sys->count = EdFileReadInt();
    if (panel_sys->count <= 0) {
        return 1;
    }
    do {
        const i32 name_length = EdFileReadInt();
        EdFileRead(panel_sys->panels[index].name, name_length);
        EdFileReadNuVec(&panel_sys->panels[index].position);
        panel_sys->panels[index].y_rotation = EdFileReadUnsignedShort();
        panel_sys->panels[index].model_variant = EdFileReadChar();

        if (version <= 2) {
            GIZPANEL *panel = &panel_sys->panels[index];
            panel->target_offset.x = 0.0f;
            panel->target_offset.y = 0.0f;
            panel->target_offset.z = -0.4f;
            panel->target_scale = 1.0f;
        } else {
            const u8 hide_base = static_cast<u8>(EdFileReadChar()) & 1;
            GIZPANEL &panel = panel_sys->panels[index];
            panel.flags = static_cast<GIZPANEL_FLAGS>((panel.flags & ~GIZPANEL_FLAG_HIDE_BASE) | (hide_base << 5));
            if (version != 3) {
                EdFileReadNuVec(&panel_sys->panels[index].target_offset);
                panel_sys->panels[index].target_scale = EdFileReadFloat();
                if (version != 4) {
                    const u8 hide_target = static_cast<u8>(EdFileReadChar()) & 1;
                    GIZPANEL &current_panel = panel_sys->panels[index];
                    current_panel.flags = static_cast<GIZPANEL_FLAGS>(
                        (current_panel.flags & ~GIZPANEL_FLAG_HIDE_TARGET) | (hide_target << 6));
                    if (version != 5) {
                        const i8 baddie = EdFileReadChar();
                        GIZPANEL &baddie_panel = panel_sys->panels[index];
                        baddie_panel.flags =
                            static_cast<GIZPANEL_FLAGS>((baddie_panel.flags & ~GIZPANEL_FLAG_BADDIE) | (baddie << 7));
                        const u8 alt_model = static_cast<u8>(EdFileReadChar()) & 1;
                        GIZPANEL &model_panel = panel_sys->panels[index];
                        model_panel.draw_flags = static_cast<GIZPANEL_DRAW_FLAGS>(
                            (model_panel.draw_flags & ~GIZPANEL_DRAW_FLAG_ALT_MODEL) | alt_model);
                        if (version != 6) {
                            const u8 no_shadow_reset = static_cast<u8>(EdFileReadChar()) & 1;
                            GIZPANEL &shadow_panel = panel_sys->panels[index];
                            shadow_panel.draw_flags = static_cast<GIZPANEL_DRAW_FLAGS>(
                                (shadow_panel.draw_flags & ~GIZPANEL_DRAW_FLAG_NO_SHADOW_RESET) |
                                (no_shadow_reset << 1));
                            if (version != 7) {
                                GIZPANEL &legacy_panel = panel_sys->panels[index];
                                legacy_panel.draw_flags = static_cast<GIZPANEL_DRAW_FLAGS>(
                                    (legacy_panel.draw_flags & ~GIZPANEL_DRAW_FLAG_LEGACY) |
                                    ((EdFileReadChar() != 0) ? GIZPANEL_DRAW_FLAG_LEGACY : 0));
                            }
                        }
                    }
                }
            } else {
                GIZPANEL *default_panel = &panel_sys->panels[index];
                default_panel->target_offset.x = 0.0f;
                default_panel->target_offset.y = 0.0f;
                default_panel->target_offset.z = -0.4f;
                default_panel->target_scale = 1.0f;
            }
        }
        GIZPANEL &panel = panel_sys->panels[index];
        panel.draw_flags = static_cast<GIZPANEL_DRAW_FLAGS>(panel.draw_flags & ~GIZPANEL_DRAW_FLAG_LEGACY);
        ++index;
    } while (index < panel_sys->count);
    return 1;
}

ADDGIZMOTYPE *GizPanel_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;

    addtype.name = "Panel";
    addtype.fns.unknown1 = 0xc;
    addtype.prefix = "";
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizPanel_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizPanel_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizPanel_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizPanel_Draw;
    addtype.fns.get_gizmo_name_fn = GizPanel_GetGizmoName;
    addtype.fns.get_output_fn = GizPanel_GetOutput;
    addtype.fns.get_output_name_fn = GizPanel_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizPanel_GetNumOutputs;
    addtype.fns.activate_fn = GizPanel_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizPanel_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizPanels_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizPanels_ClearProgress;
    addtype.fns.store_progress_fn = GizPanels_StoreProgress;
    addtype.fns.reset_fn = GizPanels_Reset;
    addtype.fns.reserve_buffer_space_fn = GizPanels_ReserveBufferSpace;
    addtype.fns.load_fn = GizPanel_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    gizpanel_gizmotype_id = type_id;

    return &addtype;
}
