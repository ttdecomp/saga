#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nuvec4.h"

namespace {

    struct NuSpecialLegacyLayout {
        u8 pad_00[0x40];
        void *instance;
        char *name;
        u32 flags;
    };

    struct NuLegacyInstanceLayout {
        u8 pad_00[0x44];
        union {
            u8 flags;
            struct {
                u8 visible : 1;
                u8 unused_flags_1_2 : 2;
                u8 no_visibility_test : 1;
                u8 unused_flags_4_7 : 4;
            };
        };
        u8 pad_45[3];
        nuinstanim_s *animation;
    };

    DECOMP_ASSERT(offsetof(NuSpecialLegacyLayout, instance) == 0x40, "legacy special instance offset");
    DECOMP_ASSERT(offsetof(NuSpecialLegacyLayout, name) == 0x44, "legacy special name offset");
    DECOMP_ASSERT(offsetof(NuSpecialLegacyLayout, flags) == 0x48, "legacy special flags offset");
    DECOMP_ASSERT(offsetof(NuLegacyInstanceLayout, flags) == 0x44, "legacy instance flags offset");
    DECOMP_ASSERT(offsetof(NuLegacyInstanceLayout, animation) == 0x48, "legacy instance animation offset");
} // namespace

extern "C" i32 NuSpecialExistsFn(void *special_ptr) {
    if (special_ptr != NULL) {
        nuhspecial_s *special = static_cast<nuhspecial_s *>(special_ptr);
        if (special->special != NULL) {
            return 1;
        }
        if (special->display_special != NULL) {
            return 1;
        }
    }
    return 0;
}

extern "C" nuinstanim_s *NuSpecialGetInstAnim(nuhspecial_s *special) {
    NuSpecialLegacyLayout *legacy = static_cast<NuSpecialLegacyLayout *>(special->special);
    if (legacy != NULL) {
        NuLegacyInstanceLayout *instance = static_cast<NuLegacyInstanceLayout *>(legacy->instance);
        return instance->animation;
    }

    NUDISPLAYSPECIAL *display = static_cast<NUDISPLAYSPECIAL *>(special->display_special);
    if (display != NULL && display->instance_animation != reinterpret_cast<nuinstanim_s *>(-1)) {
        return display->instance_animation;
    }
    return NULL;
}

extern "C" NUMTX *NuSpecialGetInstanceMtx(nuhspecial_s *special) {
    NuSpecialLegacyLayout *legacy = static_cast<NuSpecialLegacyLayout *>(special->special);
    if (legacy != NULL) {
        return static_cast<NUMTX *>(legacy->instance);
    }

    NUDISPLAYSPECIAL *display = static_cast<NUDISPLAYSPECIAL *>(special->display_special);
    if (display != NULL) {
        return &display->draw_mtx;
    }
    return NULL;
}

extern "C" f32 NuSpecialGetAnimEndFrame(nuhspecial_s *special) {
    if (special == NULL) {
        return 0.0f;
    }
    NUGSCN *scene = special->scene;
    if (scene == NULL) {
        return 0.0f;
    }

    nuinstanim_s *instance_animation = NuSpecialGetInstAnim(special);
    if (instance_animation == NULL) {
        return 0.0f;
    }

    nuanimdata_s *animation = scene->instance_animation_data[instance_animation->anim_ix];
    if (animation != NULL) {
        return NuAnimEndFrameOld(animation);
    }

    if ((instance_animation->end_frame_lookup_bits & NUINSTANIM_END_FRAME_LOOKUP_MASK) == 0 ||
        scene->animation_end_frames == NULL) {
        return 0.0f;
    }

    const u16 lookup_index = instance_animation->end_frame_lookup_index;
    const u32 end_frame = scene->animation_end_frames[lookup_index - 1].end_frame;
    return static_cast<f32>(end_frame);
}

extern "C" char *NuSpecialGetName(nuhspecial_s *special) {
    if (special == NULL) {
        return NULL;
    }

    NUDISPLAYSPECIAL *display = static_cast<NUDISPLAYSPECIAL *>(special->display_special);
    if (display != NULL) {
        return display->name;
    }
    NuSpecialLegacyLayout *legacy = static_cast<NuSpecialLegacyLayout *>(special->special);
    return legacy != NULL ? legacy->name : NULL;
}

extern "C" i32 NuSpecialGetNoVisiTestFn(nuhspecial_s *special) {
    if (special->scene == NULL) {
        return 0;
    }

    NuSpecialLegacyLayout *legacy = static_cast<NuSpecialLegacyLayout *>(special->special);
    if (legacy != NULL) {
        NuLegacyInstanceLayout *instance = static_cast<NuLegacyInstanceLayout *>(legacy->instance);
        return instance->no_visibility_test;
    }

    NUDISPLAYSPECIAL *display = static_cast<NUDISPLAYSPECIAL *>(special->display_special);
    return display != NULL ? display->flags & NUDISPLAYSPECIAL_FLAG_NO_VISIBILITY_TEST : 0;
}

extern "C" void NuSpecialSetNoVisiTest(nuhspecial_s *special, i32 enabled) {
    if (special->scene == NULL) {
        return;
    }

    NuSpecialLegacyLayout *legacy = static_cast<NuSpecialLegacyLayout *>(special->special);
    if (legacy != NULL) {
        NuLegacyInstanceLayout *instance = static_cast<NuLegacyInstanceLayout *>(legacy->instance);
        instance->no_visibility_test = enabled;
        return;
    }

    NUDISPLAYSPECIAL *display = static_cast<NUDISPLAYSPECIAL *>(special->display_special);
    if (display == NULL) {
        return;
    }
    if (enabled != 0) {
        display->flags |= NUDISPLAYSPECIAL_FLAG_NO_VISIBILITY_TEST;
    } else {
        display->flags &= ~NUDISPLAYSPECIAL_FLAG_NO_VISIBILITY_TEST;
    }
}

extern "C" i32 NuSpecialGetVisibilityFn(void *special_ptr) {
    nuhspecial_s *special = static_cast<nuhspecial_s *>(special_ptr);
    if (special->scene == NULL) {
        return 0;
    }

    NuSpecialLegacyLayout *legacy = static_cast<NuSpecialLegacyLayout *>(special->special);
    if (legacy != NULL) {
        NuLegacyInstanceLayout *instance = static_cast<NuLegacyInstanceLayout *>(legacy->instance);
        return instance->visible;
    }

    NUDISPLAYSPECIAL *display = static_cast<NUDISPLAYSPECIAL *>(special->display_special);
    return display != NULL ? (display->flags >> 1) & 1 : 0;
}

extern "C" void NuSpecialSetVisibility(void *special_ptr, i32 visible) {
    if (special_ptr == NULL) {
        return;
    }
    nuhspecial_s *special = static_cast<nuhspecial_s *>(special_ptr);
    NUGSCN *scene = special->scene;
    if (scene == NULL) {
        return;
    }

    NuSpecialLegacyLayout *legacy = static_cast<NuSpecialLegacyLayout *>(special->special);
    if (legacy != NULL) {
        NuLegacyInstanceLayout *instance = static_cast<NuLegacyInstanceLayout *>(legacy->instance);
        if (instance != NULL) {
            instance->visible = visible;
        }
        if (visible != 0) {
            legacy->flags |= NULEGACYSPECIAL_FLAG_COLLISION;
        } else {
            legacy->flags &= ~NULEGACYSPECIAL_FLAG_COLLISION;
        }
        return;
    }

    NUDISPLAYSPECIAL *display = static_cast<NUDISPLAYSPECIAL *>(special->display_special);
    if (display == NULL) {
        return;
    }

    if (visible != 0) {
        display->flags |= NUDISPLAYSPECIAL_FLAG_VISIBLE | NUDISPLAYSPECIAL_FLAG_COLLISION;
        NUDLDLISTSCENE *display_scene = reinterpret_cast<NUDLDLISTSCENE *>(scene->display_list);
        if ((display_scene->instance_visibility_enabled & NUDL_SCENE_INSTANCE_VISIBILITY_ENABLED) != 0) {
            display_scene->visibility_flags[display->instance_ix] |= NUDL_INSTANCE_FLAG_VISIBLE;
        }
        return;
    }

    display->flags &= ~(NUDISPLAYSPECIAL_FLAG_VISIBLE | NUDISPLAYSPECIAL_FLAG_COLLISION);
    NUDLDLISTSCENE *display_scene = reinterpret_cast<NUDLDLISTSCENE *>(scene->display_list);
    if ((display_scene->instance_visibility_enabled & NUDL_SCENE_INSTANCE_VISIBILITY_ENABLED) != 0) {
        display_scene->visibility_flags[display->instance_ix] &= ~NUDL_INSTANCE_FLAG_VISIBLE;
    }
}

extern "C" void NuDisplayListUpdateSpecial(nuhspecial_s *special) {
    NUDISPLAYSPECIAL *display = static_cast<NUDISPLAYSPECIAL *>(special->display_special);
    NUDLDLISTSCENE *display_scene = reinterpret_cast<NUDLDLISTSCENE *>(special->scene->display_list);
    NUMTX draw_matrix;
    draw_matrix = *NuSpecialGetDrawMtx(special);

    NUVEC4 local_center;
    NUVEC half_extent;
    half_extent.x = (display->bounds_max.x - display->bounds_min.x) * 0.5f;
    half_extent.y = (display->bounds_max.y - display->bounds_min.y) * 0.5f;
    half_extent.z = (display->bounds_max.z - display->bounds_min.z) * 0.5f;

    local_center.x = (display->bounds_max.x + display->bounds_min.x) * 0.5f;
    local_center.y = (display->bounds_max.y + display->bounds_min.y) * 0.5f;
    local_center.z = (display->bounds_max.z + display->bounds_min.z) * 0.5f;

    NUVEC world_extent;
    world_extent.x = NuFabs(draw_matrix.m00) * half_extent.x + NuFabs(draw_matrix.m10) * half_extent.y +
                     NuFabs(draw_matrix.m20) * half_extent.z;
    world_extent.y = NuFabs(draw_matrix.m01) * half_extent.x + NuFabs(draw_matrix.m11) * half_extent.y +
                     NuFabs(draw_matrix.m21) * half_extent.z;
    world_extent.z = NuFabs(draw_matrix.m02) * half_extent.x + NuFabs(draw_matrix.m12) * half_extent.y +
                     NuFabs(draw_matrix.m22) * half_extent.z;

    NuVecMtxTransform(reinterpret_cast<NUVEC *>(&local_center), reinterpret_cast<NUVEC *>(&local_center), &draw_matrix);
    local_center.w = NuFsqrt(world_extent.x + world_extent.y + world_extent.z);

    if ((display_scene->render_buffer & NUDL_SCENE_RENDER_FLAG_CENTER_EXTENT_BOUNDS) != 0) {
        NUVEC4 *clip_bounds = reinterpret_cast<NUVEC4 *>(display_scene->clip_bounds);
        const i32 bounds_index = display->instance_ix * 2;
        clip_bounds[bounds_index] = local_center;
        clip_bounds[bounds_index + 1].x = world_extent.x;
        clip_bounds[bounds_index + 1].y = world_extent.y;
        clip_bounds[bounds_index + 1].z = world_extent.z;
    } else {
        NUVEC4 *clip_bounds = reinterpret_cast<NUVEC4 *>(display_scene->clip_bounds);
        const i32 bounds_index = display->instance_ix * 2;
        clip_bounds[bounds_index].x = local_center.x - world_extent.x;
        clip_bounds[bounds_index].y = local_center.y - world_extent.y;
        clip_bounds[bounds_index].z = local_center.z - world_extent.z;
        clip_bounds[bounds_index + 1].x = local_center.x + world_extent.x;
        clip_bounds[bounds_index + 1].y = local_center.y + world_extent.y;
        clip_bounds[bounds_index + 1].z = local_center.z + world_extent.z;
    }

    DisplayListUpdateSpecialTransformPS(special, &draw_matrix);
}

extern "C" void NuSpecialUpdate(nuhspecial_s *special) {
    if (special == NULL) {
        return;
    }

    NUDISPLAYSPECIAL *display = static_cast<NUDISPLAYSPECIAL *>(special->display_special);
    if (display == NULL || special->scene == NULL) {
        return;
    }

    NUDLDLISTSCENE *display_scene = reinterpret_cast<NUDLDLISTSCENE *>(special->scene->display_list);
    if (display_scene == NULL ||
        (display_scene->instance_visibility_enabled & NUDL_SCENE_INSTANCE_VISIBILITY_ENABLED) == 0) {
        return;
    }

    display->flags |= NUDISPLAYSPECIAL_FLAG_MATRIX_UPDATED;
    NuDisplayListUpdateSpecial(special);
}
