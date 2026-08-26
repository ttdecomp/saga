/*
 * Shader manager — plain host implementation.
 *
 * Faithful, readable reimplementation of the original iOS shader manager.
 * The original built shader keys from a 104-byte material block hashed with
 * a CRC-16 pair, remapped them through redirect tables and cached compiled
 * GL programs in a fixed-size slot array.
 *
 * This file keeps the same hashing, remapping and caching behaviour but
 * presents it as ordinary C++: named constants, typed storage helpers and
 * linear control flow instead of goto/union tricks and hex-offset comments.
 */

#include "nushader_plain.h"
#include "nu2api/nu3d/nu2api_nu3d_types.h"
#include "nushader.h"

// NuShaderObjectInit overload used by the manager. The shader-version tag is
// unused on the host but kept for ABI compatibility.
void NuShaderObjectInit(nushaderobject_s *, const nushaderobjectkey_s *, i32, u32, u32, eSHADERVERSION);

using nu2api::HashRedirect;
using nu2api::LoadedUniqueShaderRecord;
using nu2api::ShaderMtlDescFilterPlain;
using nu2api::ShaderObjectKey;

#include <GLES2/gl2.h>
#include <cstdio>
#include <cstring>

#include "nu2api/nufile/nufile.h"

extern i32 file_criticalsection;
#include "nu2api/nucore/nuthread.h"

namespace nu2api {
namespace detail {

// ---------------------------------------------------------------------------
// CRC-16 (CCITT, poly 0x1021, init 0xFFFF)
// ---------------------------------------------------------------------------

static u32 s_crcTable[256];

static void BuildCrcTable() {
    for (u32 i = 0; i < 256; ++i) {
        u32 value = 0;
        u32 input = i << 8;
        for (int bit = 0; bit < 8; ++bit) {
            const bool carry = ((value ^ input) & 0x8000) != 0;
            value = carry ? ((value << 1) ^ 0x1021) & 0xffff
                          : (value << 1) & 0xffff;
            input <<= 1;
        }
        s_crcTable[i] = value;
    }
}

// Ensure the table is built before first use, matching the original global
// constructor (_GLOBAL__sub_I_CRC16.cpp).
static const bool s_crcInitialised = (BuildCrcTable(), true);

static u32 HashForward(const u8 *data, i32 len) {
    if (len <= 0) {
        return 0xffffffffu;
    }
    u32 crc = 0xffff;
    for (const u8 *end = data + len; data != end; ++data) {
        crc = ((crc << 8) ^ s_crcTable[((crc >> 8) ^ *data) & 0xff]) & 0xffff;
    }
    return crc;
}

static u32 HashReverse(const u8 *data, i32 len) {
    if (len <= 0) {
        return 0xffffffffu;
    }
    u32 crc = 0xffff;
    const u8 *p = data + len;
    while (p != data) {
        --p;
        crc = ((crc << 8) ^ s_crcTable[((crc >> 8) ^ *p) & 0xff]) & 0xffff;
    }
    return crc;
}

// ---------------------------------------------------------------------------
// Sorted-table lookups
//
// All redirect / preloaded tables are sorted by key and queried with the
// same binary search. Returning false means "no entry".
// ---------------------------------------------------------------------------

static bool FindRedirect(u32 key, u32 *outValue, const HashRedirect *table, u32 count) {
    if (count == 0) {
        return false;
    }
    i32 lo = 0;
    i32 hi = static_cast<i32>(count) - 1;
    i32 mid = hi >> 1;

    while (true) {
        const u32 midKey = table[mid].key;
        if (midKey == key) {
            *outValue = table[mid].value;
            return true;
        }
        if (midKey < key) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
        if (hi < lo) {
            return false;
        }
        mid = (lo + hi) / 2;
    }
}

static bool FindPreloaded(u32 key, GLuint **outSlot, LoadedUniqueShaderRecord *table, u32 count) {
    if (count == 0) {
        return false;
    }
    i32 lo = 0;
    i32 hi = static_cast<i32>(count) - 1;
    i32 mid = hi >> 1;

    while (true) {
        const u32 midKey = table[mid].key;
        if (midKey == key) {
            *outSlot = &table[mid].gl_shader;
            return true;
        }
        if (midKey < key) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
        if (hi < lo) {
            return false;
        }
        mid = (lo + hi) / 2;
    }
}

// ---------------------------------------------------------------------------
// Shader source loading
// ---------------------------------------------------------------------------

static constexpr u32 kShaderSourceCapacity = 0x4000;
static char s_shaderSourceBuffer[kShaderSourceCapacity];

static bool TryLoadShaderSource(char **outSource, i32 *outSize, u32 sourceKey, bool pixelStage) {
    *outSource = nullptr;
    *outSize = 0;

    char path[268];
    std::snprintf(path, sizeof(path), "%s/0x%08x.ios_%s",
                  "builtshaders/ios", sourceKey, pixelStage ? "pcode" : "vcode");

    NUFILE fh = NuFileOpen(path, NUFILE_READ);
    if (fh == 0) {
        return false;
    }

    const i32 size = NuFileOpenSize(fh);
    *outSize = size;
    NuFileRead(fh, s_shaderSourceBuffer, size);
    NuFileClose(fh);

    // Shipped iOS sources use a fixed precision qualifier that does not
    // validate on every host driver. Patch it in place.
    char *hit = nullptr;
    if (pixelStage) {
        hit = std::strstr(s_shaderSourceBuffer, "precision lowp float;");
        if (hit != nullptr && std::strstr(s_shaderSourceBuffer, "_envmap_samplerCube") != nullptr) {
            constexpr const char *kReplacement = "precision mediump float;";
            std::memcpy(hit, kReplacement, std::strlen(kReplacement));
        }
    } else {
        hit = std::strstr(s_shaderSourceBuffer, "precision mediump float;");
        if (hit != nullptr) {
            constexpr const char *kReplacement = "precision highp float;  ";
            std::memcpy(hit, kReplacement, std::strlen(kReplacement));
        }
    }

    s_shaderSourceBuffer[size] = '\0';
    *outSource = s_shaderSourceBuffer;
    return true;
}

} // namespace detail
} // namespace nu2api

// Redirect / preloaded tables are generated byte-exact from the original
// binary. They define g_vertexShaderRedirects, g_pixelShaderRedirects,
// g_shaderProgramRedirects, g_loadedUniqueVertexShaders and
// g_loadedUniquePixelShaders.
#include "nudlist_tables_gen.inc"

namespace nu2api {

// ---------------------------------------------------------------------------
// Manager storage
//
// Original layout is a 0x4bc90-byte block:
//
//   [0x000 .. 0x003]  vtable placeholder
//   [0x004 .. ...]    401 slots, stride 0x308
//     slot+0x04  i32   refcount
//     slot+0x08  u32   program key
//     slot+0x10  u32   GL program name
//   [0x4bc84]         last-allocated slot index
//   [0x4bc88]         currently bound slot (stored as address)
// ---------------------------------------------------------------------------

static constexpr u32 kSlotStride        = 0x308;
static constexpr u32 kSlotCount         = 0x191; // 401
static constexpr u32 kOffsetLastSlot    = 0x4bc84;
static constexpr u32 kOffsetBoundSlot   = 0x4bc88;
static constexpr u32 kManagerBlockSize  = 0x4bc90;

static u8 s_managerBlock[kManagerBlockSize];

inline u8 *SlotPtr(i32 id) {
    return s_managerBlock + 4 + static_cast<u32>(id) * kSlotStride;
}
inline i32 &SlotRefCount(u8 *slot) {
    return *reinterpret_cast<i32 *>(slot + 4);
}
inline u32 &SlotProgramKey(u8 *slot) {
    return *reinterpret_cast<u32 *>(slot + 8);
}
inline GLuint &SlotGlProgram(u8 *slot) {
    return *reinterpret_cast<GLuint *>(slot + 0x10);
}
inline i32 &ManagerLastAllocated() {
    return *reinterpret_cast<i32 *>(s_managerBlock + kOffsetLastSlot);
}
inline i32 &ManagerBoundSlotAddr() {
    return *reinterpret_cast<i32 *>(s_managerBlock + kOffsetBoundSlot);
}

void *g_shaderManager = s_managerBlock;

// ---------------------------------------------------------------------------
// Uniform table
// ---------------------------------------------------------------------------

struct ShaderUniformRecord {
    u8 raw[0x5c];
};

// 155 semantic records (0x244c bytes total in the original .data).
ShaderUniformRecord g_shaderUniforms[155];

// Uber-shader identity used by key generation (first 8 bytes of the MD5).
static const u8 kUberShaderHash[16] = {
    0x38, 0x2a, 0x9d, 0x15, 0xf8, 0xfa, 0xbf, 0x09,
    0xcb, 0xcc, 0x9b, 0xec, 0x5e, 0xb7, 0x62, 0x40,
};

extern "C" void glUniform1fv(GLint, GLsizei, const GLfloat *);
extern "C" void glUniform2fv(GLint, GLsizei, const GLfloat *);
extern "C" void glUniform3fv(GLint, GLsizei, const GLfloat *);
extern "C" void glUniform4fv(GLint, GLsizei, const GLfloat *);

} // namespace nu2api

// ---------------------------------------------------------------------------
// Public C API
// ---------------------------------------------------------------------------

extern "C" i32 NuShaderManagerGetShaderById(i32 id) {
    if (static_cast<u32>(id) >= nu2api::kSlotCount) {
        return 0;
    }
    u8 *slot = nu2api::SlotPtr(id);
    union { u8 *ptr; i32 id; } conv{slot};
    return conv.id;
}

extern "C" void NuShaderManagerReleaseShader(i32 slotAddr) {
    if (slotAddr == 0) {
        return;
    }
    union { i32 id; u8 *ptr; } conv{slotAddr};
    nu2api::SlotRefCount(conv.ptr)--;
}

static GLuint s_currentGlProgram = 0;

extern "C" void NuShaderManagerBindShader(i32 slotAddr) {
    (void)nu2api::g_shaderManager;
    nu2api::ManagerBoundSlotAddr() = slotAddr;
    if (slotAddr == 0) {
        return;
    }
    union { i32 id; u8 *ptr; } conv{slotAddr};
    const GLuint program = nu2api::SlotGlProgram(conv.ptr);
    if (program != s_currentGlProgram) {
        glUseProgram(program);
        s_currentGlProgram = program;
    }
}

extern "C" void NuShaderManagerSetfv(i32 semantic, const f32 *values) {
    using namespace nu2api;
    const i32 vec4Count = *reinterpret_cast<const i32 *>(g_shaderUniforms[semantic].raw + 8);
    if (vec4Count < 5) {
        std::memcpy(g_shaderUniforms[semantic].raw + 0x1c, values,
                    static_cast<size_t>(vec4Count) << 4);
    }
}

namespace nu2api {

// ---------------------------------------------------------------------------
// Material filter helpers
// ---------------------------------------------------------------------------

static bool FilterHasDiffuseMap(const ShaderMtlDescFilterPlain *filter, i32 which) {
    const bool extended = (filter->variant == 0) || (filter->desc->unknown_1b4 != 0);
    if (!extended) {
        return false;
    }
    const i32 threshold = filter->param4;
    switch (which) {
        case 0: return threshold < filter->desc->diffuse_map_tex_id[0];
        case 1: return threshold < filter->desc->diffuse_map_tex_id[1];
        case 2: return threshold < filter->desc->diffuse_map_tex_id[2];
        case 3: return threshold < filter->desc->diffuse_map_tex_id[3];
        default: return false;
    }
}

static bool FilterHasLayer(const ShaderMtlDescFilterPlain *filter, i32 layer) {
    const NUSHADERMTLDESC &d = *filter->desc;
    if (filter->variant == 0) {
        switch (layer) {
            case 0: return true;
            case 1: return (d.flagsbits_1b8 & 0x40) != 0 && d.blend_op2 != 0xff;
            case 2: return (static_cast<i8>(d.flagsbits_1b8) < 0) && d.blend_op3 != 0xff;
            case 3: return (d.byte4 & 1) != 0 && d.blend_op4 != 0xff;
            default: return false;
        }
    }
    // Variant path only cares about layer 0.
    return layer == 0 && (d.unknown_1b4 & 1) != 0;
}

static u32 FilterGetVertexFlags(const ShaderMtlDescFilterPlain *filter) {
    const NUSHADERMTLDESC *d = filter->desc;
    const u8 *vb = reinterpret_cast<const u8 *>(&d->vtx_desc);

    const u8 attr0 = vb[1]; // 0x13D – texture unit count + low bit
    const u8 attr1 = vb[2]; // 0x13E – packed vertex attribute flags
    const u8 attr2 = vb[3]; // 0x13F – additional attribute bits

    const bool hasPackedPair = (attr1 & 3) != 0;
    const bool hasExtendedPair = (attr1 & 0x40) != 0;
    const u8 texUnitCount = (attr0 >> 1) & 3;
    const bool extended = (filter->variant == 0) || (d->unknown_1b4 != 0);

    u32 flags = (attr1 >> 2) & 1;
    if (hasPackedPair && !hasExtendedPair) {
        flags |= 2;
    }

    if (extended) {
        if (texUnitCount != 0) {
            flags |= 4;
        }
        if (texUnitCount >= 2) {
            flags |= 8;
        }
        if (texUnitCount == 3) {
            flags |= 0x10;
        }
    }

    if (hasPackedPair && hasExtendedPair) {
        flags |= 0x40;
    }
    if (attr1 & 8) {
        flags |= 0x80;
    }
    if (attr2 & 1) {
        flags |= 0x100;
    }
    if (attr2 & 2) {
        flags |= 0x400;
    }

    if (extended) {
        if (static_cast<i8>(d->flagsbits_1bb) < 0) {
            flags |= 0x800;
        }
        if (attr0 & 1) {
            flags |= 0x1000;
        }
    }
    return flags;
}

// ---------------------------------------------------------------------------
// Filter initialisation
// ---------------------------------------------------------------------------

static void FilterInternalInit(ShaderMtlDescFilterPlain *filter,
                               const NUSHADERMTLDESC *desc,
                               const void *mtl,
                               i32 flagsIn,
                               i32 param4) {
    const u8 *descBytes = reinterpret_cast<const u8 *>(desc);
    const u8 *mtlBytes  = reinterpret_cast<const u8 *>(mtl);

    filter->mtl      = mtl;
    filter->param4   = param4;
    filter->desc     = desc;
    filter->flags_in = flagsIn;
    filter->variant  = flagsIn & 3;

    const bool hasVtxFlag4      = (reinterpret_cast<const u8 *>(&desc->vtx_desc)[2] & 4) != 0;
    const bool hasMtlFlag40     = (mtlBytes[0x41] & 0x40) != 0;
    const bool anyVtxMtlFlag    = hasVtxFlag4 || hasMtlFlag40;

    // field4 / field6 describe whether the high-precision / compressed paths
    // are required. The original had a shared "zeroed" early-out for the
    // variant != 0 case.
    if (filter->variant != 0) {
        filter->field4_0x10 = 0;
        filter->field6_0x18 = anyVtxMtlFlag ? 1 : 0;
    } else {
        const bool flag10Set = (flagsIn & 0x10) != 0;
        if (flag10Set && anyVtxMtlFlag) {
            filter->field4_0x10 = 0;
            filter->field6_0x18 = anyVtxMtlFlag ? 1 : 0;
        } else if (flag10Set) {
            filter->field4_0x10 = 1;
            filter->field6_0x18 = anyVtxMtlFlag ? 1 : 0;
        } else {
            filter->field4_0x10 = 0;
            if (!hasVtxFlag4 && !hasMtlFlag40) {
                filter->field6_0x18 = 0;
            } else {
                filter->field6_0x18 = 1;
            }
            // When the straightforward path was taken, field6 already encodes
            // the flag; otherwise it mirrors the combined test above.
            if (flag10Set || anyVtxMtlFlag) {
                filter->field6_0x18 = anyVtxMtlFlag ? 1 : 0;
            }
        }
    }

    // field5 / field7 track deferred blending and extra stage counts.
    if (filter->variant == 0) {
        const bool canDefer = (flagsIn & 0x20) != 0 && !hasVtxFlag4 && !hasMtlFlag40;
        filter->field5_0x14 = canDefer ? 1 : 0;

        const bool needsHighPrecision = (filter->field4_0x10 != 0) || hasVtxFlag4;
        if (needsHighPrecision) {
            filter->field7_0x1c = 2;
        } else if (descBytes[0x1ba] & 1) {
            filter->field7_0x1c = 2;
        } else {
            filter->field7_0x1c = 1;
        }
        if (canDefer) {
            filter->field7_0x1c++;
        }
    } else {
        filter->field5_0x14 = 0;
        filter->field7_0x1c = (filter->field4_0x10 != 0) ? 1 : 0;
    }
}

} // namespace nu2api

namespace nu2api {

// ---------------------------------------------------------------------------
// Key generation
//
// Builds the 104-byte descriptor block that is double-hashed (forward +
// reverse CRC-16) to produce the 32-bit program key. Field names below
// mirror the Ghidra block layout but are documented by purpose.
// ---------------------------------------------------------------------------

static void BuildShaderKey(u32 *outKey, const ShaderMtlDescFilterPlain *filter, bool pixelStage) {
    const NUSHADERMTLDESC *desc = filter->desc;
    const u8 *descBytes = reinterpret_cast<const u8 *>(desc);

    // Fixed 104-byte hash input; zero-initialised then sparsely populated.
    u8 block[0x68] = {};
    block[0x14] = 1;

    u32 flagsWord = 0;
    u8  nibble    = 0;

    if (!pixelStage) {
        // Vertex path carries the uber-shader identity.
        block[0x0c] = kUberShaderHash[0];
        block[0x0d] = kUberShaderHash[1];
        block[0x0e] = kUberShaderHash[2];
        block[0x0f] = kUberShaderHash[3];
        block[0x10] = kUberShaderHash[4];
        block[0x11] = kUberShaderHash[5];
        block[0x12] = kUberShaderHash[6];
        block[0x13] = kUberShaderHash[7];

        if (filter->variant != 0) {
            flagsWord = 0x1000;
            nibble = 0;
        } else {
            flagsWord = *reinterpret_cast<const u32 *>(descBytes + 0x1b8) & 0xfffffe3fu;
            nibble = descBytes[0x1bc];
        }
    } else {
        if (filter->variant == 0) {
            flagsWord = *reinterpret_cast<const u32 *>(descBytes + 0x1b8) & 0xfffffe3fu;
            nibble = descBytes[0x1bc];
        } else {
            flagsWord = 0x1000;
            nibble = 0;
        }
    }

    // Filtered flag word (bytes 0x04..0x07, nibble at 0x08, copy at 0x09..0x0b).
    block[0x04] = static_cast<u8>(flagsWord);
    block[0x05] = static_cast<u8>(flagsWord >> 8);
    block[0x06] = static_cast<u8>(flagsWord >> 16);
    block[0x07] = static_cast<u8>(flagsWord >> 24);
    block[0x08] = nibble & 0xf;
    block[0x09] = block[0x04];
    block[0x0a] = block[0x05];
    block[0x0b] = block[0x06];

    // Vertex flags
    const u32 vtxFlags = FilterGetVertexFlags(filter);
    block[0x00] = static_cast<u8>(vtxFlags);
    block[0x01] = static_cast<u8>(vtxFlags >> 8);
    block[0x02] = static_cast<u8>(vtxFlags >> 16);
    block[0x03] = static_cast<u8>(vtxFlags >> 24);

    // Filter-derived booleans
    block[0x21] = filter->field4_0x10 != 0;
    block[0x22] = filter->field5_0x14 != 0;
    block[0x23] = filter->field6_0x18 != 0;

    // Auxiliary texture ids: selected only when the base flag / threshold says
    // the texture is actually bound.
    if (filter->variant == 0) {
        const u8 baseFlag = descBytes[0x1b8];
        block[0x15] = ((baseFlag & 1) || (baseFlag & 2)) ? descBytes[0xa9] : 0;
        block[0x16] = filter->param4 < desc->specular_map_tid ? descBytes[0xaa] : 0;
    }

    // Diffuse-map presence + ids
    block[0x17] = static_cast<u8>(FilterHasDiffuseMap(filter, 0));
    block[0x18] = FilterHasDiffuseMap(filter, 1) ? descBytes[0xab] : 0;
    block[0x19] = FilterHasDiffuseMap(filter, 2) ? descBytes[0xac] : 0;
    block[0x1a] = FilterHasDiffuseMap(filter, 3) ? descBytes[0xad] : 0;

    // Optional extra map id (0xae)
    if (filter->variant == 0) {
        const u8 b1 = descBytes[0x1b9];
        if ((b1 & 0x20) || (b1 & 0x40)) {
            block[0x1b] = descBytes[0xae];
        }
    }

    // Blend ops for layers 1..3
    block[0x1c] = FilterHasLayer(filter, 1) ? desc->blend_op2 : 0;
    block[0x1d] = FilterHasLayer(filter, 2) ? desc->blend_op3 : 0;
    block[0x1e] = FilterHasLayer(filter, 3) ? desc->blend_op4 : 0;

    // Constants required by the original hashing contract
    block[0x1f] = 1;
    block[0x20] = 1;

    // Shadow / animation variant selectors
    if (filter->variant == 0) {
        const i32 thresholdA = *reinterpret_cast<const i32 *>(descBytes + 0x34);
        const i32 thresholdB = *reinterpret_cast<const i32 *>(descBytes + 0x38);
        const u8 baseFlag = descBytes[0x1b8];

        u8 v24 = 0;
        if ((baseFlag & 1) == 0 || filter->param4 >= thresholdB) {
            if ((baseFlag & 2) != 0) {
                v24 = filter->param4 < thresholdB;
            }
        } else {
            v24 = 2;
        }
        block[0x24] = v24;

        const u8 b2 = descBytes[0x1ba];
        block[0x25] = (b2 & 0x10) ? static_cast<u8>((filter->param4 < thresholdA) + 1) : 0;
    }

    block[0x26] = *reinterpret_cast<const i32 *>(descBytes + 0x1e4) > 0;
    block[0x27] = *reinterpret_cast<const i32 *>(descBytes + 0x1e8) > 0;

    // Texcoord type heuristic over the material attribute bytes at mtl+0x40..0x42.
    const u8 *mtlBytes = reinterpret_cast<const u8 *>(filter->mtl);
    const u8 unitsLow = (mtlBytes[0x42] >> 4) & 7;
    const u8 vtxType  = mtlBytes[0x40] & 0xf;

    bool texcoordIsLong = false;
    if (unitsLow == 3 || unitsLow == 5) {
        texcoordIsLong = true;
    } else {
        texcoordIsLong = (vtxType == 10) || ((unitsLow & 3) == 2);
    }
    const u8 attribC0 = mtlBytes[0x41] & 0xc0;
    if (attribC0 == 0 || attribC0 == 0x80 || vtxType == 0 || vtxType == 10) {
        block[0x28] = texcoordIsLong ? 1 : 0;
    }

    block[0x29] = desc->unknown_a8;

    const u32 hi = detail::HashReverse(block, 0x68);
    const u32 lo = detail::HashForward(block, 0x68);
    outKey[0] = (hi << 16) | (lo & 0xffff);
}

} // namespace nu2api

namespace nu2api {

// ---------------------------------------------------------------------------
// Program creation and cache lookup
// ---------------------------------------------------------------------------

static bool CreateGlProgramForKey(const ShaderObjectKey &key, void *outObject, i32 param) {
    using namespace detail;

    u32 vertexKey = key.key[0];
    u32 pixelKey  = key.key[0];

    u32 remapped = 0;
    if (FindRedirect(vertexKey, &remapped, g_vertexShaderRedirects, 0x318)) {
        vertexKey = remapped;
    }
    if (FindRedirect(pixelKey, &remapped, g_pixelShaderRedirects, 0x25a)) {
        pixelKey = remapped;
    }

    GLuint *vertexSlot = nullptr;
    GLuint *pixelSlot  = nullptr;
    FindPreloaded(vertexKey, &vertexSlot, g_loadedUniqueVertexShaders, 0x170);
    FindPreloaded(pixelKey,  &pixelSlot,  g_loadedUniquePixelShaders,  0x22e);

    if (vertexSlot == nullptr || pixelSlot == nullptr) {
        return false;
    }

    // Compile on demand, guarded by the file critical section like the original.
    if (*vertexSlot == 0) {
        NuThreadCriticalSectionBegin(file_criticalsection);
        char *src = nullptr;
        i32 size = 0;
        const bool loaded = TryLoadShaderSource(&src, &size, vertexKey, false);
        const bool compiled = loaded && NuShaderObjectGenerateGLSLShader(vertexSlot, GL_VERTEX_SHADER, src, size);
        NuThreadCriticalSectionEnd(file_criticalsection);
        if (!compiled) {
            return false;
        }
    }

    if (*pixelSlot == 0) {
        NuThreadCriticalSectionBegin(file_criticalsection);
        char *src = nullptr;
        i32 size = 0;
        const bool loaded = TryLoadShaderSource(&src, &size, pixelKey, true);
        const bool compiled = loaded && NuShaderObjectGenerateGLSLShader(pixelSlot, GL_FRAGMENT_SHADER, src, size);
        NuThreadCriticalSectionEnd(file_criticalsection);
        if (!compiled) {
            return false;
        }
    }

    NuShaderObjectInit(reinterpret_cast<nushaderobject_s *>(outObject),
                       reinterpret_cast<const nushaderobjectkey_s *>(&key),
                       param,
                       static_cast<u32>(*vertexSlot),
                       static_cast<u32>(*pixelSlot),
                       eSHADERVERSION{});
    return true;
}

static void *RetrieveShader(void *manager,
                            NUSHADERMTLDESC *desc,
                            void *mtl,
                            i32 variant,
                            i32 flagsIn,
                            bool pixelStage) {
    ShaderMtlDescFilterPlain filter{};
    FilterInternalInit(&filter, desc, mtl, variant, flagsIn);

    u32 rawKey[4] = {};
    BuildShaderKey(rawKey, &filter, pixelStage ? 1 : 0);

    ShaderObjectKey programKey{};
    u32 redirected = 0;
    if (detail::FindRedirect(rawKey[0], &redirected, g_shaderProgramRedirects, 0x1a1)) {
        programKey.key[0] = redirected;
    } else {
        programKey.key[0] = rawKey[0];
    }

    u8 *base = static_cast<u8 *>(manager);

    // Cache hit: bump refcount and return existing slot.
    for (u32 id = 0; id < 0x190; ++id) {
        u8 *slot = base + 4 + id * kSlotStride;
        if (SlotProgramKey(slot) == programKey.key[0]) {
            SlotRefCount(slot)++;
            return slot;
        }
    }

    // Cache miss: round-robin over the fixed slot pool.
    const i32 last = ManagerLastAllocated();
    for (i32 step = 1; step <= 0x190; ++step) {
        const i32 id = (last + step) % static_cast<i32>(kSlotCount);
        u8 *slot = base + 4 + static_cast<u32>(id) * kSlotStride;
        if (SlotRefCount(slot) > 0) {
            continue;
        }
        ManagerLastAllocated() = id;
        if (!CreateGlProgramForKey(programKey, slot, 0)) {
            return nullptr;
        }
        SlotRefCount(slot)++;
        return slot;
    }
    return nullptr;
}

} // namespace nu2api

// ---------------------------------------------------------------------------
// Wrappers used by the engine
// ---------------------------------------------------------------------------

extern "C" void *NuShaderManagerRetrieveShader(NUSHADERMTLDESC *desc, void *mtl) {
    return nu2api::RetrieveShader(nu2api::g_shaderManager, desc, mtl, 0, 0, false);
}

extern "C" void *NuShaderManagerRetrieveShaderVariant(NUSHADERMTLDESC *desc, void *mtl, i32 variant) {
    return nu2api::RetrieveShader(nu2api::g_shaderManager, desc, mtl, variant, 0x10, false);
}

// Material setup is not yet ported. The original walked the program's uniform
// list and pushed values through the constant setters. Until that is
// transcribed the pipeline runs with engine defaults.
extern "C" void NuShaderObjectGLSLSetupMaterial(i32 program, struct numtl_s *mtl) {
    (void)program;
    (void)mtl;
}

// GL uniform dispatch table — matches the original .data at 0x65e0b8.
extern "C" void (*g_glConstantSetterTable[4])(u32 loc, i32 count, const void *vals) = {
    reinterpret_cast<void (*)(u32, i32, const void *)>(nu2api::glUniform1fv),
    reinterpret_cast<void (*)(u32, i32, const void *)>(nu2api::glUniform2fv),
    reinterpret_cast<void (*)(u32, i32, const void *)>(nu2api::glUniform3fv),
    reinterpret_cast<void (*)(u32, i32, const void *)>(nu2api::glUniform4fv),
};
