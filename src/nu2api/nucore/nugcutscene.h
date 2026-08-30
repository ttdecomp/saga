#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/numath/numtx.h"

struct nugscn_s;
typedef struct nugscn_s NUGSCN;
struct NUGCUTRIGID_s;
struct instNUGCUTSCENE_s;

struct NUGCUTLOCATOR_s {
    NUMTX base_matrix;
    NUVEC pivot;
    f32 locator_scale;
    nuanimdata2_s *animation;
    u32 field_54;
    u8 flags;
    u8 type_index;
    u8 field_5a;
    u8 field_5b;
    u8 pad_5c[8];
};

struct NUGCUTLOCATORTYPE_s {
    char *name;
    u8 flags;
    u8 field_05;
    u16 function_index;
    u32 field_08;
};

struct NUGCUTLOCATORSYS_s {
    NUGCUTLOCATOR_s *locators;
    NUGCUTLOCATORTYPE_s *types;
    u8 locator_count;
    u8 type_count;
    u8 pad_0a[2];
};

struct instNUGCUTLOCATOR_s {
    u32 field_00;
    i32 effect_handle;
};

typedef void (*NUGCUTLOCATORFN)(instNUGCUTSCENE_s *, NUGCUTLOCATORSYS_s *, instNUGCUTLOCATOR_s *,
                                NUGCUTLOCATOR_s *, f32, NUMTX *, i32);

struct NUGCUTLOCATORFNENTRY_s {
    const char *name;
    i32 field_04;
    i32 field_08;
    i32 field_0c;
    NUGCUTLOCATORFN function;
};

struct instNUGCUTLOCATORSYS_s {
    instNUGCUTLOCATOR_s *locators;
};

struct StateAnim {
    u16 count;
    u16 unused;
    f32 *times;
    u8 *values;
};

struct NUGCUTRIGIDSYS_s {
    NUGCUTRIGID_s *rigids;
    u16 count;
    u16 unused;
};

struct NUGCUTRIGID_s {
    NUMTX base_matrix;
    char *name;
    NUGSCN *scene;
    void *special_object;
    nuanimdata2_s *animation;
    StateAnim *state_animation;
    union {
        NUGCUTLOCATOR_s *locator;
        u32 serialized_locator_index;
    };
    u8 flags;
    u8 locator_count;
    u8 locator_index;
    u8 field_5b;
};

struct NUGCUTSCENE_s {
    i32 version;
    i32 string_delta;
    f32 duration;
    char *strings;
    void *camera_system;
    NUGCUTRIGIDSYS_s *rigid_system;
    void *character_system;
    NUGCUTLOCATORSYS_s *locator_system;
    void *bounds;
    void *trigger_system;
    i32 relocation_delta;
    char *filename;
    void *stream_buffer_0;
    void *stream_buffer_1;
    u32 flags;
    u8 pad_3c[8];
    NUGSCN *scene;
    void *extra_scene;
    u8 pad_4c[0x58 - 0x4c];
    i32 loaded_size;
};

struct instNUGCUTRIGID_s {
    NUGSCN *scene;
    void *special;
    void *display_special;
    u8 state_index;
    u8 visible;
    u8 pad_0e[2];
};

struct instNUGCUTRIGIDSYS_s {
    instNUGCUTRIGID_s *rigids;
};

struct instNUGCUTSCENE_s {
    instNUGCUTSCENE_s *next;
    instNUGCUTSCENE_s *previous;
    char name[0x10];
    NUMTX matrix;
    NUGCUTSCENE_s *cutscene;
    NUGCUTSCENE_s *cutscene_copy;
    u8 pad_60[0x88 - 0x60];
    u8 flags_88;
    u8 flags_89;
    u8 flags_8a;
    u8 flags_8b;
    u8 flags_8c;
    u8 flags_8d;
    u8 pad_8e[2];
    f32 current_frame;
    f32 render_frame;
    f32 rate;
    void *camera_instance;
    instNUGCUTRIGIDSYS_s *rigid_instance;
    void *character_instance;
    instNUGCUTLOCATORSYS_s *locator_instance;
    void *trigger_instance;
    u8 pad_b0[0xe8 - 0xb0];
    f32 alpha;
    u8 pad_ec[4];
    i32 allocation_size;
    u8 pad_f4[4];
};

DECOMP_ASSERT(sizeof(StateAnim) == 0x0c, "StateAnim must match the original x86 layout");
DECOMP_ASSERT(sizeof(NUGCUTLOCATOR_s) == 0x64, "NUGCUTLOCATOR_s must match the original x86 layout");
DECOMP_ASSERT(sizeof(NUGCUTLOCATORTYPE_s) == 0x0c, "NUGCUTLOCATORTYPE_s must match the original x86 layout");
DECOMP_ASSERT(sizeof(NUGCUTLOCATORSYS_s) == 0x0c, "NUGCUTLOCATORSYS_s must match the original x86 layout");
DECOMP_ASSERT(sizeof(instNUGCUTLOCATOR_s) == 0x08, "instNUGCUTLOCATOR_s must match the original x86 layout");
DECOMP_ASSERT(sizeof(NUGCUTLOCATORFNENTRY_s) == 0x14,
              "NUGCUTLOCATORFNENTRY_s must match the original x86 layout");
DECOMP_ASSERT(sizeof(NUGCUTRIGID_s) == 0x5c, "NUGCUTRIGID_s must match the original x86 layout");
DECOMP_ASSERT(sizeof(instNUGCUTSCENE_s) == 0xf8, "instNUGCUTSCENE_s must match the original x86 layout");
