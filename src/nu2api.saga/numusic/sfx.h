#pragma once

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nusound/nusound.h"

typedef struct nusound_info_s {
    const char *sfx_name;
    i32 index : 15;
    u32 loop : 1;
    u32 seq : 1;
    u32 global : 1;
    u32 disabled : 1;
    u32 comment : 1;
    u32 dirty : 1;
    u32 revertable : 1;
    i32 pitch;
    u8 priority;
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

#ifdef __cplusplus
extern "C" {
#endif

    extern nusound_filename_info_s *SfxInfo;

    extern NUSOUNDINFO *g_soundInfo;
    extern NUSOUNDINFO *g_revertSoundInfo;

    void InitSfx(variptr_u *buffer_start, variptr_u buffer_end, const char *file);

    void LoadSfx(const char *file, variptr_u *buffer_start, variptr_u buffer_end);

    i32 GetSfxId(const char *name);

#ifdef __cplusplus
}
#endif
