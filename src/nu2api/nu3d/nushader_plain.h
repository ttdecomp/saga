// Shader-manager core types — see nushadermanager_plain.cpp.
#pragma once

#include "decomp.h"
#include "nu2api/nu3d/numtl.h"

#include <GLES2/gl2.h>

struct nushaderobject_s;
typedef nushaderobject_s NUSHADEROBJECT;

// Combined shader key (ShaderObjectKey): first dword is the program key the
// redirect tables and slot cache are keyed on.
namespace nu2api {
    struct ShaderObjectKey {
        u32 key[4];
    };

    // { u32 from; u32 to; } binary-search record.
    struct HashRedirect {
        u32 key;
        u32 value;
    };

    // { u32 key; GLuint shader; } binary-search record (GL id filled on demand).
    struct LoadedUniqueShaderRecord {
        u32 key;
        GLuint gl_shader;
    };

    // ShaderMtlDescFilter (original layout: 0x24 bytes) — plain data view.
    // Named FilterPlain to avoid collision with legoapi's ShaderMtlDescFilter class.
    struct ShaderMtlDescFilterPlain {
        const NUSHADERMTLDESC *desc; // 0x00
        const void *mtl;             // 0x04
        i32 flags_in;                // 0x08 (param_3)
        i32 variant;                 // 0x0c (param_3 & 3)
        i32 field4_0x10;
        i32 field5_0x14;
        i32 field6_0x18;
        i32 field7_0x1c;
        i32 param4; // 0x20 (field8_0x20)
    };

    extern void *g_shaderManager;
} // namespace nu2api

extern "C" NUSHADEROBJECT *NuShaderManagerGetShaderById(i32 id);
extern "C" void NuShaderManagerReleaseShader(NUSHADEROBJECT *shader);
extern "C" void NuShaderManagerBindShader(NUSHADEROBJECT *slot);
extern "C" void *NuShaderManagerRetrieveShader(NUSHADERMTLDESC *desc, void *mtl);
extern "C" void *NuShaderManagerRetrieveShaderVariant(NUSHADERMTLDESC *desc, void *mtl, i32 variant);
extern "C" void NuShaderManagerSetfv(i32 semantic, const f32 *values);
extern "C" void NuShaderManagerSetElementsfv(i32 semantic, i32 first_element, i32 count, const f32 *values);
extern "C" NUSHADEROBJECT *NuShaderManagerGetCurrentShader(void);
extern "C" void NuShaderManagerInit(VARIPTR *arena, VARIPTR arena_end);
