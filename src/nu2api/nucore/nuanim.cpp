#include "nu2api/nucore/nuanim3.h"

extern "C" f32 NuAnimEndFrame(void *animation_data) {
    ani3_animheader_s *animation = static_cast<ani3_animheader_s *>(animation_data);
    if (animation->magic != ANI3_MAGIC_VERSION_4 && animation->magic != ANI3_MAGIC_VERSION_5) {
        return *static_cast<f32 *>(animation_data);
    }

    return static_cast<f32>(animation->frame_count) + static_cast<f32>(animation->first_frame);
}

extern "C" f32 NuAnimEndFrameOld(void *animation_data) {
    ani3_animheader_s *animation = static_cast<ani3_animheader_s *>(animation_data);
    if (animation->magic != ANI3_MAGIC_VERSION_4 && animation->magic != ANI3_MAGIC_VERSION_5) {
        return *static_cast<f32 *>(animation_data);
    }

    const u32 declared_end_frame = animation->declared_end_frame;
    if (declared_end_frame == 0) {
        return static_cast<f32>(animation->frame_count + animation->first_frame);
    }
    return static_cast<f32>(declared_end_frame);
}
