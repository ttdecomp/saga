#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/fixed_width.h"

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

DECOMP_ASSERT(sizeof(ani3_animheader_s) == 0x3c, "ANI4 header must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimcurvedata_s) == 0x0c, "animation curve data must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimcurve2_s) == 0x04, "animation curve must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimdata2_s) == 0x18, "animation header must match the original x86 layout");
DECOMP_ASSERT(sizeof(nuanimtime_s) == 0x18, "animation time must match the original x86 layout");
