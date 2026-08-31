#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nusound/nusound.h"

#include <cstddef>

typedef struct nusound_info_s {
    const char *sfx_name;
    union {
        struct {
            i32 index : 15;
            u32 loop : 1;
            u32 seq : 1;
            u32 global : 1;
            u32 disabled : 1;
            u32 comment : 1;
            u32 dirty : 1;
            u32 revertable : 1;
            u32 nofade : 1;
            u32 unused_flags : 9;
        };
        u8 flag_bytes[4];
    };
    i32 pitch;
    i8 priority;
    u8 padding_0xd[3];
    i16 next;
    i16 volume;
    i16 group;
    u16 category;
    f32 pan;
    f32 pitch_rnd;
    f32 volume_rnd;
    f32 falloff_near;
    f32 falloff_far;
    f32 buzz_timer;
    i32 rumble_strength;
    f32 rumble_sustain;
    f32 rumble_release;
    const char *filename;
    i32 field29_0x40;
} NUSOUNDINFO;

DECOMP_ASSERT(sizeof(NUSOUNDINFO) == 0x44, "NUSOUNDINFO size");
DECOMP_ASSERT(offsetof(NUSOUNDINFO, next) == 0x10, "NUSOUNDINFO next offset");
DECOMP_ASSERT(offsetof(NUSOUNDINFO, volume) == 0x12, "NUSOUNDINFO volume offset");
DECOMP_ASSERT(offsetof(NUSOUNDINFO, group) == 0x14, "NUSOUNDINFO group offset");
DECOMP_ASSERT(offsetof(NUSOUNDINFO, category) == 0x16, "NUSOUNDINFO category offset");

#ifdef __cplusplus
extern "C" {
#endif

    extern nusound_filename_info_s *SfxInfo;

    extern NUSOUNDINFO *g_soundInfo;
    extern NUSOUNDINFO *g_revertSoundInfo;
    extern u16 GlobalSfxBits[100];
    extern u16 SfxBits[100];

    void InitSfx(variptr_u *buffer_start, variptr_u buffer_end, const char *file);
    void ResetSounds(void);

    i32 GetSfxId(const char *name);

#ifdef __cplusplus
}
#endif
