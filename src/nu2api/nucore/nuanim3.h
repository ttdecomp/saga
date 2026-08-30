#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

struct ani3_animheader_s;
struct nuanimbuff_s;

struct ani3_scalemin_s {
    float scale;
    float minimum;
};

struct nuanimcurvedata_s {
    u32 *key_mask;
    u16 *key_offsets;
    void *key_data;
};

union nuanimcurve2data_u {
    float constant;
    nuanimcurvedata_s *curvedata;
};

struct nuanimcurve2_s {
    nuanimcurve2data_u data;
};

struct nuanimdata2_s {
    float duration;
    u16 node_count;
    u16 curve_count;
    i16 chunk_count;
    u16 field_0a;
    nuanimcurve2_s *curves;
    u8 *curve_types;
    u8 *node_flags;
};

#ifdef __cplusplus
extern "C" {
#endif
    void *NuAnimData2Fixup(i32 file_size, void **data);
    void *NuAnimData2LoadBuffEx(char *path, VARIPTR *buf, VARIPTR *buf_end, void **result);
    void *NuAnimData2LoadBuff(char *path, VARIPTR *buf, VARIPTR *buf_end);
    void *NuAnimData2LoadBuffFromPAK(void *data, i32 file_size);
    void ANI_Ani3ExtractAllNodeCurves(ani3_animheader_s *anim, f32 frame, f32 *values, i32 node, char *curve_mask);
    void ANI_SimpleAni3PlayerV4Joint(ani3_animheader_s *anim, f32 frame, nuanimbuff_s *buffer, i32 first_joint,
                                     i32 joint_count);
    void ANI_SimpleAni3PlayerV4Joint_Blend(ani3_animheader_s *anim, f32 frame, nuanimbuff_s *buffer, f32 blend,
                                           i32 first_joint, i32 joint_count, NUVEC *root_translation);
#ifdef __cplusplus
}
#endif

struct nuanimtime_s {
    float time;
    float time_offset;
    i32 chunk;
    u32 time_mask;
    u32 time_byte;
    u32 chunk_frame;
};

struct ani3_animheader_s {
    u32 magic;
    u16 node_count;
    u16 key_count;
    u16 key_stride;
    u16 frame_count;
    u16 curve_count;
    u16 first_frame;
    u8 end_frame;
    u8 constant_index;
    u16 field_12;
    u16 next_block;
    u8 pad_16[6];
    float minimum;
    float scale;
    ani3_scalemin_s *scale_min;
    i16 *constants;
    u16 *curve_types;
    u8 *keys;
    u8 *node_flags;
    void *field_38;
};

// One decompressed joint sample in the ANI4/ANI5 animation buffer.  The
// player stores translation, Euler rotation and scale as three aligned
// vectors; NuAnimBuffEvaluate_3 consumes the same 0x30-byte stride.
struct nuanimbuffjoint_s {
    NUVEC translation;
    f32 translation_w;
    NUVEC rotation;
    f32 rotation_w;
    NUVEC scale;
    f32 scale_w;
};

struct nuanimbuff_s {
    i32 joint_count;
    i16 max_joints;
    u8 use_quaternions;
    u8 pad_07;
    nuanimbuffjoint_s *joints;
    u8 *joint_flags;
};

DECOMP_ASSERT(sizeof(ani3_animheader_s) == 0x3c, "ANI4 header must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimcurvedata_s) == 0x0c, "animation curve data must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimcurve2_s) == 0x04, "animation curve must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimdata2_s) == 0x18, "animation header must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimtime_s) == 0x18, "animation time must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimbuffjoint_s) == 0x30, "animation-buffer joint must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimbuff_s) == 0x10, "animation buffer must match the original x86 layout");
