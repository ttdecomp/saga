#include "nu2api/nucore/common.h"

struct nutexanimenv_s {
    u8 reserved_00[0x04];
    void *texture;
    u8 reserved_08[0x80];
    void *program;
    u8 reserved_8c[0x40];
    void *field_cc;
    void *field_d0;
    void *field_d4;
    void *field_d8;
    u8 reserved_dc[0x08];
    void *field_e4;
};

struct nutexanim_s {
    struct nutexanim_s *next;
    u8 reserved_04[0x10];
    struct nutexanimenv_s *env;
};

#ifdef __cplusplus
DECOMP_ASSERT(offsetof(nutexanim_s, next) == 0x00, "texture animation next offset");
DECOMP_ASSERT(offsetof(nutexanim_s, env) == 0x14, "texture animation environment offset");
DECOMP_ASSERT(sizeof(nutexanim_s) == 0x18, "texture animation size");
DECOMP_ASSERT(offsetof(nutexanimenv_s, texture) == 0x04, "texture animation environment texture offset");
DECOMP_ASSERT(offsetof(nutexanimenv_s, program) == 0x88, "texture animation environment program offset");
DECOMP_ASSERT(offsetof(nutexanimenv_s, field_cc) == 0xcc, "texture animation environment field cc offset");
DECOMP_ASSERT(offsetof(nutexanimenv_s, field_e4) == 0xe4, "texture animation environment field e4 offset");
DECOMP_ASSERT(sizeof(nutexanimenv_s) == 0xe8, "texture animation environment size");
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 g_texAnimCriticalSection;
#ifdef __cplusplus
}
#endif
