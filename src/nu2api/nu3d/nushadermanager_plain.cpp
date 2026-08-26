// Shader-manager core — faithful transcriptions from libTTapp.so.
//
//   CRC16 (ctor/hash/hashInverse)      0x30e1e0 / 0x30e2f0 / 0x30e350
//   LookupHash                          0x30d910
//   LookupPreloadedShaderObject         0x30dc60
//   LoadShaderSource                    0x30dac0
//   NuIOS_GetShaderProgramKey           0x30d990
//   ShaderMtlDescFilter (+helpers)      0x309240 / 0x309460 / 0x3094e0 / 0x3093c0
//   NuShaderObjectKeyGenerate3          0x309620
//   NuShaderManagerGetShaderById        0x308da0
//   NuShaderManagerReleaseShader        0x308d00
//   NuShaderManagerBindShader           0x308de0
//   ShaderManagerOpenGL::setfv          0x30dfb0
//   ShaderManagerOpenGL::createShader(K) 0x30dce0
//   retrieveShader template             0x309090
//
// The redirect/preload tables are byte-exact extractions from the binary.

#include "nushader_plain.h"
#include "nushader.h"

// (obj, key, i32, u32 vs, u32 ps, eSHADERVERSION) overload — body in nushader.cpp
struct eSHADERVERSION {};
void NuShaderObjectInit(nushaderobject_s *, const nushaderobjectkey_s *, i32, u32, u32, eSHADERVERSION);

#include <GLES2/gl2.h>
#include <stdio.h>
#include <string.h>

#include "nu2api/nufile/nufile.h"

extern i32 file_criticalsection;
#include "nu2api/nucore/nuthread.h"

namespace nu2api {

    // --- manager state -----------------------------------------------------------

} // namespace nu2api

namespace nu2api {

    // --- CRC16 (originals 0x30e1e0/0x30e2f0/0x30e350; table B @0x11dec60) --------

    static u32 crcTable[256];

    // original: CRC16::CRC16 runs as a global static constructor
    // (_GLOBAL__sub_I_CRC16.cpp @0xdd9f0)
    static void CRC16Init(void);
    static const bool g_crc16_initialised = (CRC16Init(), true);

    static void CRC16Init(void) {
        for (u32 i = 0; i < 0x100; i++) {
            u32 value = 0;
            u32 input = i << 8;
            for (int bit = 0; bit < 8; bit++) {
                if (((value ^ input) & 0x8000) != 0) {
                    value = ((value << 1) ^ 0x1021) & 0xffff;
                } else {
                    value = (value << 1) & 0xffff;
                }
                input <<= 1;
            }
            crcTable[i] = value;
        }
    }

    static u32 CRC16Hash(const u8 *data, i32 len) {
        if (len < 1) {
            return 0xffffffffu;
        }
        u32 crc = 0xffff;
        const u8 *end = data + len;
        for (; data != end; data++) {
            crc = ((crc << 8) ^ crcTable[((crc >> 8) ^ *data) & 0xff]) & 0xffff;
        }
        return crc;
    }

    static u32 CRC16HashInverse(const u8 *data, i32 len) {
        if (len < 1) {
            return 0xffffffffu;
        }
        u32 crc = 0xffff;
        const u8 *p = data + len;
        while (p != data) {
            p--;
            crc = ((crc << 8) ^ crcTable[((crc >> 8) ^ *p) & 0xff]) & 0xffff;
        }
        return crc;
    }

    // --- binary-search lookups (originals 0x30d910 / 0x30dc60) -------------------

    static i32 LookupHash(u32 key, u32 *out, const HashRedirect *table, u32 count) {
        i32 hi = (i32)count - 1;
        if (hi < 0) {
            return 0;
        }
        i32 lo = 0;
        i32 mid = hi >> 1;
        u32 mid_key = table[mid].key;
        while (mid_key != key) {
            if (mid_key < key) {
                lo = mid + 1;
                if (hi < lo)
                    return 0;
            } else {
                hi = mid - 1;
                if (hi < lo)
                    return 0;
            }
            mid = (lo + hi) / 2;
            mid_key = table[mid].key;
        }
        *out = table[mid].value;
        return 1;
    }

    static i32 LookupPreloaded(u32 key, GLuint **out, LoadedUniqueShaderRecord *table, u32 count) {
        i32 hi = (i32)count - 1;
        if (hi < 0) {
            return 0;
        }
        i32 lo = 0;
        i32 mid = hi >> 1;
        u32 mid_key = table[mid].key;
        while (mid_key != key) {
            if (mid_key < key) {
                lo = mid + 1;
                if (hi < lo)
                    return 0;
            } else {
                hi = mid - 1;
                if (hi < lo)
                    return 0;
            }
            mid = (lo + hi) / 2;
            mid_key = table[mid].key;
        }
        *out = &table[mid].gl_shader;
        return 1;
    }

    // --- LoadShaderSource (original 0x30dac0) ------------------------------------
    // storage is the function-local static bss buffer @0x11dabe0, 0x4000 bytes.

    static char shaderSourceStorage[0x4000];

    static i32 LoadShaderSource(char **out_src, i32 *out_size, u32 source_key, bool pixel) {
        *out_src = NULL;
        *out_size = 0;

        char path[268];
        sprintf(path, "%s/0x%08x.ios_%s", "builtshaders/ios", source_key, pixel ? "pcode" : "vcode");

        NUFILE fh = NuFileOpen(path, NUFILE_READ);
        if (fh == 0) {
            return 0;
        }
        i32 size = NuFileOpenSize(fh);
        *out_size = size;
        NuFileRead(fh, shaderSourceStorage, size);
        NuFileClose(fh);

        // The shipped sources carry iOS precision qualifiers; swap them to match
        // what this build's drivers accept.
        char *hit;
        const char *replacement;
        if (pixel) {
            hit = strstr(shaderSourceStorage, "precision lowp float;");
            replacement = "precision mediump float;";
            if (hit == NULL || strstr(shaderSourceStorage, "_envmap_samplerCube") == NULL) {
                goto done;
            }
        } else {
            hit = strstr(shaderSourceStorage, "precision mediump float;");
            replacement = "precision highp float;  ";
            if (hit == NULL) {
                goto done;
            }
        }
        memcpy(hit, replacement, strlen(replacement));

    done:
        shaderSourceStorage[size] = '\0';
        *out_src = shaderSourceStorage;
        return 1;
    }

} // namespace nu2api

// --- redirect / preload tables ------------------------------------------------
#include "nudlist_tables_gen.inc"

namespace nu2api {

    // original .data @0x65e0b8 — relocations bind these to glUniform1fv/2fv/3fv/4fv
    extern "C" void glUniform1fv(GLint, GLsizei, const GLfloat *);
    extern "C" void glUniform2fv(GLint, GLsizei, const GLfloat *);
    extern "C" void glUniform3fv(GLint, GLsizei, const GLfloat *);
    extern "C" void glUniform4fv(GLint, GLsizei, const GLfloat *);

    // uber-shader identity bytes used by KeyGenerate3
    static const u8 uberShader2_md5[16] = {
        0x38, 0x2a, 0x9d, 0x15, 0xf8, 0xfa, 0xbf, 0x09,
        0xcb, 0xcc, 0x9b, 0xec, 0x5e, 0xb7, 0x62, 0x40}; // original .data @0x636e20 (first 8 bytes consumed)

    // --- manager object -----------------------------------------------------------
    // The manager is a 0x4bc8c-byte block carved by NuShaderManagerInit:
    //   [0x000..0x003] vptr (not modelled)
    //   [0x4bc84]      last-allocated slot index
    //   [0x4bc88]      currently bound slot
    //   slots live at +4, 0x308 bytes each: { vptr?, i32 refcount @+4, u32 key @+8,
    //   ... GLuint program @+0x10 }
    static const u32 SLOT_STRIDE = 0x308;
    static const u32 SLOT_COUNT = 0x191;
    static u8 g_managerBlock[0x4bc90];

    inline u8 *SlotPtr(i32 id) {
        return g_managerBlock + 4 + (u32)id * SLOT_STRIDE;
    }
    inline i32 &SlotRefcount(u8 *slot) {
        return *(i32 *)(slot + 4);
    }
    inline u32 &SlotKey(u8 *slot) {
        return *(u32 *)(slot + 8);
    }
    inline GLuint &SlotProgram(u8 *slot) {
        return *(GLuint *)(slot + 0x10);
    }

    void *g_shaderManager = g_managerBlock; // original B @0x11d8040

    // --- uniform table --------------------------------------------------------------
    // g_shaderUniforms: original D @0x6349c0, 0x244c bytes = 155 records of 0x5c.
    struct ShaderUniformRecord {
        u8 raw[0x5c];
    };
    ShaderUniformRecord g_shaderUniforms[155]; // semantic count from table size

} // namespace nu2api

// --- public API (originals cited) ---------------------------------------------

// 0x308da0
extern "C" i32 NuShaderManagerGetShaderById(i32 id) {
    if ((u32)id > 0x190) {
        return 0;
    }
    // pointer -> i32 slot handle (identical width on the i686 target)
    union {
        u8 *p_;
        i32 i_;
    } u;
    u.p_ = nu2api::SlotPtr(id);
    return u.i_;
}

// 0x308d00 — drop one reference.
extern "C" void NuShaderManagerReleaseShader(i32 slot) {
    (*(i32 *)(slot + 4))--;
}

// 0x308de0
static GLuint g_currentGLProgram;

extern "C" void NuShaderManagerBindShader(i32 slot_addr) {
    nu2api::g_shaderManager; // keep symbol referenced like the original global
    *(i32 *)((u8 *)nu2api::g_managerBlock + 0x4bc88) = slot_addr;
    if (slot_addr != 0) {
        GLuint program = *(GLuint *)(slot_addr + 0x10);
        if (program != g_currentGLProgram) {
            glUseProgram(program);
            g_currentGLProgram = program;
        }
    }
}

// 0x30dfb0
extern "C" void NuShaderManagerSetfv(i32 semantic, const f32 *values) {
    using namespace nu2api;
    i32 count = *(i32 *)(g_shaderUniforms[semantic].raw + 8);
    if (count < 5) {
        memcpy(g_shaderUniforms[semantic].raw + 0x1c, values, count << 4);
    }
}

namespace nu2api {

    // --- ShaderMtlDescFilter helpers ------------------------------------------------

    // 0x309460
    static i32 FilterHasDiffuseMap(const ShaderMtlDescFilter *f, i32 which) {
        if (f->variant == 0 || f->desc->unknown_1b4 != 0) {
            if (which == 1)
                return f->param4 < f->desc->diffuse_map_tex_id[1];
            if (which == 0)
                return f->param4 < f->desc->diffuse_map_tex_id[0];
            if (which == 2)
                return f->param4 < f->desc->diffuse_map_tex_id[2];
            if (which == 3)
                return f->param4 < f->desc->diffuse_map_tex_id[3];
        }
        return 0;
    }

    // 0x3093c0
    static i32 FilterHasLayer(const ShaderMtlDescFilter *f, i32 layer) {
        if (f->variant == 0) {
            if (layer == 1) {
                if ((f->desc->flagsbits_1b8 & 0x40) == 0)
                    return false;
                return f->desc->blendOp2 != 0xff;
            }
            if (layer == 0)
                return true;
            if (layer == 2) {
                if ((i8)f->desc->flagsbits_1b8 >= 0)
                    return false;
                return f->desc->blendOp3 != 0xff;
            }
            if (layer == 3) {
                if ((f->desc->byte4 & 1) == 0) // desc+0x1B9 bit0
                    return false;
                return f->desc->blendOp4 != 0xff;
            }
        } else if (layer == 0) {
            return f->desc->unknown_1b4 & 1;
        }
        return false;
    }

    // 0x3094e0
    static u32 FilterGetVertexFlags(const ShaderMtlDescFilter *f) {
        const NUSHADERMTLDESC *d = f->desc;
        const u8 *vb = (const u8 *)&d->vtx_desc; // desc+0x13C..
        u8 b_e = vb[2];                          // 0x13E
        bool pair = (b_e & 3) != 0;
        bool bit6 = (b_e & 0x40) != 0;
        u8 t = (vb[1] >> 1) & 3; // 0x13D texture-unit count field
        i32 variant = f->variant;
        bool ext = (variant == 0) || (d->unknown_1b4 != 0);

        // 3094ed..309518
        u32 flags = (b_e >> 2) & 1;
        if (pair && !bit6)
            flags |= 2;

        if (ext) {
            // 3095a0..3095dd — unit-count flags, each step gated by 0x1B4
            if (t != 0)
                flags |= 4;
            bool cont1 = (variant != 0) ? (d->unknown_1b4 != 0) : true;
            if (cont1) {
                if (t >= 2)
                    flags |= 8;
                bool cont2 = (variant != 0) ? (d->unknown_1b4 != 0) : true;
                if (cont2 && t == 3)
                    flags |= 0x10;
            }
        }

        // 30952d..309573 — shared middle
        if (pair && bit6)
            flags |= 0x40;
        if (b_e & 8)
            flags |= 0x80;
        if (vb[3] & 1)
            flags |= 0x100;
        if (vb[3] & 2)
            flags |= 0x400;

        // 309574..309596 — tail gates on 0x1BB sign and 0x13D bit0
        if (variant == 0 || d->unknown_1b4 != 0) {
            if ((i8)d->flagsbits_1bb < 0)
                flags |= 0x800;
            if (vb[1] & 1)
                flags |= 0x1000;
        }
        return flags;
    }

} // namespace nu2api

namespace nu2api {

    // 0x309240 — ShaderMtlDescFilter::internalInit(desc, mtl, flags, param4)
    static void FilterInternalInit(ShaderMtlDescFilter *f, const NUSHADERMTLDESC *desc, const void *mtl, i32 flags_in,
                                   i32 param4) {
        const u8 *desc_bytes = (const u8 *)desc;
        const u8 *mtl_bytes = (const u8 *)mtl;
        bool b1;
        i32 iv;

        f->mtl = mtl;
        f->param4 = param4;
        f->desc = desc;
        f->flags_in = flags_in;
        f->variant = flags_in & 3;

        if (f->variant == 0) {
            if ((flags_in & 0x10) != 0 &&
                ((((const u8 *)&desc->vtx_desc)[2] & 4) != 0 || (mtl_bytes[0x41] & 0x40) != 0)) {
                goto zeroed;
            }
            if ((flags_in & 0x10) != 0) {
                f->field4_0x10 = 1;
                b1 = true;
            } else {
                f->field4_0x10 = 0;
                b1 = false;
            }
            iv = 0;
            if (!b1 && !(((const u8 *)&desc->vtx_desc)[2] & 4)) {
                if (!(mtl_bytes[0x41] & 0x40)) {
                    iv = 0;
                    f->field6_0x18 = 0;
                } else {
                    iv = 1;
                    f->field6_0x18 = 1;
                }
            } else {
                iv = ((const u8 *)&desc->vtx_desc)[2] & 4 ? 1 : (mtl_bytes[0x41] & 0x40 ? 1 : 0);
                f->field6_0x18 = iv;
            }
        } else {
        zeroed:
            f->field4_0x10 = 0;
            b1 = false;
            iv = (((const u8 *)&desc->vtx_desc)[2] & 4) || (mtl_bytes[0x41] & 0x40) ? 1 : 0;
            f->field6_0x18 = iv;
        }

        bool b2;
        if (f->variant == 0) {
            if ((flags_in & 0x20) != 0 && !(((const u8 *)&desc->vtx_desc)[2] & 4) && !(mtl_bytes[0x41] & 0x40)) {
                f->field5_0x14 = 1;
                b2 = true;
            } else {
                f->field5_0x14 = 0;
                b2 = false;
            }
            i32 t = 1;
            if (!b1) {
                if (!(desc_bytes[0x1ba] & 1)) {
                    t = 1;
                } else {
                    t = 2;
                }
            } else {
                t = 2;
            }
            f->field7_0x1c = t;
            iv = t;
        } else {
            f->field5_0x14 = 0;
            f->field7_0x1c = 0;
            if (!b1) {
                iv = 0;
                b2 = false;
                f->field7_0x1c = 0;
            } else {
                iv = 1;
                b2 = false;
                f->field7_0x1c = 1;
            }
            if (b1 || f->variant != 0) {
                f->field7_0x1c = iv;
            }
        }
        if (iv != 0) {
            // original: field7 += (iv != 0)
        }
        if (b2) {
            f->field7_0x1c++;
        }
    }

} // namespace nu2api

namespace nu2api {

    // 0x309620 — NuShaderObjectKeyGenerate3(key_out, filter, pixel)
    // 0x309620 — NuShaderObjectKeyGenerate3(key_out, filter, pixel).
    // Byte-block layout recovered from the Ghidra locals (base = &local_80,
    // block offset = 0x80 - localNN):
    //   +0x00..03 vertex flags (getVertexFlags, LE bytes)
    //   +0x04..07 filtered desc-flag word bytes, +8 nibble of byte 0x1bc,
    //   +0x09..0b copies of flag-word bytes 0..2
    //   +0x0c..13 uber-shader md5 (vertex path) or zeros (pixel path)
    //   +0x14 unused, +0x15..16 aux tex ids 0xa9/0xaa, +0x17..1a diffuse-map
    //   flags/ids 0xab..0xad, +0x1b id 0xae, +0x1c..1e blend ops 2..4,
    //   +0x1f/+0x20 constant 1s, +0x21..23 filter fields 4/5/6 as bools,
    //   +0x24/+0x25 shadow-variant selectors, +0x26/+0x27 anim-set presence,
    //   +0x28 texcoord-derived bool, +0x29 unknown_a8
    static void KeyGenerate3(u32 *key_out, const ShaderMtlDescFilter *f, i32 pixel) {
        const NUSHADERMTLDESC *desc = f->desc;
        const u8 *fb = (const u8 *)desc;
        u8 block[0x68];
        memset(block, 0, sizeof(block));

        // 30965a — constant flag byte inside the hashed block
        block[0x14] = 1;

        u32 flags_word;
        u8 nibble_byte;
        if (pixel == 0) {
            block[0x0c] = uberShader2_md5[0];
            block[0x0d] = uberShader2_md5[1];
            block[0x0e] = uberShader2_md5[2];
            block[0x0f] = uberShader2_md5[3];
            block[0x10] = uberShader2_md5[4];
            block[0x11] = uberShader2_md5[5];
            block[0x12] = uberShader2_md5[6];
            block[0x13] = uberShader2_md5[7];
            if (f->variant != 0) {
                nibble_byte = 0;
                flags_word = 0x1000;
            } else {
                nibble_byte = fb[0x1bc];
                flags_word = *(const u32 *)(fb + 0x1b8) & 0xfffffe3fu;
            }
        } else {
            // md5 area stays zeroed on the pixel path
            if (f->variant == 0) {
                nibble_byte = fb[0x1bc];
                flags_word = *(const u32 *)(fb + 0x1b8) & 0xfffffe3fu;
            } else {
                nibble_byte = 0;
                flags_word = 0x1000;
            }
        }

        u8 b0 = fb[0x1b8];

        block[0x04] = (u8)(flags_word);
        block[0x05] = (u8)(flags_word >> 8);
        block[0x06] = (u8)(flags_word >> 16);
        block[0x07] = (u8)(flags_word >> 24);
        block[0x08] = nibble_byte & 0xf;
        block[0x09] = block[0x04];
        block[0x0a] = block[0x05];
        block[0x0b] = block[0x06];

        u32 vflags = FilterGetVertexFlags(f);
        block[0x00] = (u8)vflags;
        block[0x01] = (u8)(vflags >> 8);
        block[0x02] = (u8)(vflags >> 16);
        block[0x03] = (u8)(vflags >> 24);

        block[0x21] = f->field4_0x10 != 0;
        block[0x22] = f->field5_0x14 != 0;
        block[0x23] = f->field6_0x18 != 0;

        if (f->variant == 0) {
            block[0x15] = ((b0 & 1) != 0 || (b0 & 2) != 0) ? fb[0xa9] : 0;
            block[0x16] = f->param4 < desc->specularMapTID ? fb[0xaa] : 0;
        }

        block[0x17] = (u8)FilterHasDiffuseMap(f, 0);
        block[0x18] = FilterHasDiffuseMap(f, 1) ? fb[0xab] : 0;
        block[0x19] = FilterHasDiffuseMap(f, 2) ? fb[0xac] : 0;
        block[0x1a] = FilterHasDiffuseMap(f, 3) ? fb[0xad] : 0;

        block[0x1b] = 0;
        if (f->variant == 0) {
            u8 b1 = fb[0x1b9];
            if ((b1 & 0x20) != 0 || (b1 & 0x40) != 0) {
                block[0x1b] = fb[0xae];
            }
        }

        block[0x1c] = FilterHasLayer(f, 1) ? desc->blendOp2 : 0;
        block[0x1d] = FilterHasLayer(f, 2) ? desc->blendOp3 : 0;
        block[0x1e] = FilterHasLayer(f, 3) ? desc->blendOp4 : 0;

        block[0x20] = 1;
        block[0x1f] = 1;

        if (f->variant == 0) {
            i32 f16 = *(const i32 *)(fb + 0x34);
            i32 f38 = *(const i32 *)(fb + 0x38);
            u8 v24;
            if ((b0 & 1) == 0 || f->param4 >= f38) {
                v24 = 0;
                if ((b0 & 2) != 0) {
                    v24 = f->param4 < f38;
                }
            } else {
                v24 = 2;
            }
            block[0x24] = v24;

            u8 b2 = fb[0x1ba];
            if ((b2 & 0x10) != 0) {
                block[0x25] = (u8)((f->param4 < f16) + 1);
            } else {
                block[0x25] = 0;
            }
        } else {
            block[0x24] = 0;
            block[0x25] = 0;
        }

        block[0x26] = 0 < *(const i32 *)(fb + 0x1e4);
        block[0x27] = 0 < *(const i32 *)(fb + 0x1e8);

        // texcoord type heuristic over numtl_s+0x40..42
        // (attrib word bytes; asm 3098b1-3098e9)
        const u8 *mtl_bytes = (const u8 *)f->mtl;
        u8 units_low = (mtl_bytes[0x42] >> 4) & 7;
        u8 vtx_type = mtl_bytes[0x40] & 0xf;
        bool tc_bool;
        if (units_low == 3 || units_low == 5) {
            tc_bool = true;
        } else {
            tc_bool = vtx_type == 10 || (units_low & 3) == 2;
        }
        u8 attrib_c0 = mtl_bytes[0x41] & 0xc0;
        if (attrib_c0 == 0 || attrib_c0 == 0x80 || vtx_type == 0 || vtx_type == 10) {
            block[0x28] = tc_bool;
        }

        block[0x29] = desc->unknown_a8;

        u32 hi = CRC16HashInverse(block, 0x68);
        u32 lo = CRC16Hash(block, 0x68);
        // TEMP probe: first-call shader-key trace (remove once the key hits
        // the redirect/preload tables)
        char hx[0x69 * 2 + 1];
        for (int _i = 0; _i < 0x68; _i++)
            snprintf(hx + _i * 2, 3, "%02x", block[_i]);
        LOG_WARN("[kg3blk] %s", hx);
        {
            const u8 *vb = (const u8 *)&desc->vtx_desc;
            LOG_WARN("[kg3vd] %02x %02x %02x %02x", vb[0], vb[1], vb[2], vb[3]);
        }
        key_out[0] = (hi << 0x10) | (lo & 0xffff);
    }

} // namespace nu2api

namespace nu2api {

    // 0x31dce0 — ShaderManagerOpenGL::createShader(ShaderObjectKey const&, NuShaderObject*, int)
    static i32 ManagerCreateShader(const ShaderObjectKey &key, void *obj, i32 param) {
        u32 vkey = key.key[0];
        u32 pkey = key.key[0];

        u32 vsrc = vkey, psrc = pkey;
        LookupHash(vkey, &vsrc, g_vertexShaderRedirects, 0x318);
        LookupHash(pkey, &psrc, g_pixelShaderRedirects, 0x25a);

        GLuint *vslot = NULL, *pslot = NULL;
        LookupPreloaded(vkey, &vslot, g_loadedUniqueVertexShaders, 0x170);
        LookupPreloaded(pkey, &pslot, g_loadedUniquePixelShaders, 0x22e);
        if (vslot == NULL || pslot == NULL) {
            return 0;
        }

        char *vcode_src = NULL, *pcode_src = NULL;
        i32 vsize = 0, psize = 0;
        if (*vslot == 0) {
            NuThreadCriticalSectionBegin(file_criticalsection);
            if (!LoadShaderSource(&vcode_src, &vsize, vsrc, false)) {
                NuThreadCriticalSectionEnd(file_criticalsection);
                return 0;
            }
            if (!NuShaderObjectGenerateGLSLShader(vslot, GL_VERTEX_SHADER, vcode_src, vsize)) {
                NuThreadCriticalSectionEnd(file_criticalsection);
                return 0;
            }
            NuThreadCriticalSectionEnd(file_criticalsection);
        }
        if (*pslot == 0) {
            NuThreadCriticalSectionBegin(file_criticalsection);
            if (!LoadShaderSource(&pcode_src, &psize, psrc, true) ||
                !NuShaderObjectGenerateGLSLShader(pslot, GL_FRAGMENT_SHADER, pcode_src, psize)) {
                NuThreadCriticalSectionEnd(file_criticalsection);
                return 0;
            }
            NuThreadCriticalSectionEnd(file_criticalsection);
        }

        NuShaderObjectInit((nushaderobject_s *)obj, (const nushaderobjectkey_s *)&key, param, (u32)*vslot, (u32)*pslot,
                           eSHADERVERSION());
        return 1;
    }

    // 0x309090 — retrieveShader(desc, mtl, variant, flags, pixel)
    static void *RetrieveShader(void *mgr, NUSHADERMTLDESC *desc, void *mtl, i32 variant, i32 flags_in, bool pixel) {
        ShaderMtlDescFilter filter;
        u8 *base = (u8 *)mgr;

        FilterInternalInit(&filter, desc, mtl, variant, flags_in);

        u32 key[4] = {0};
        KeyGenerate3(key, &filter, pixel);

        ShaderObjectKey program_key;
        {
            u32 redirected;
            if (LookupHash(key[0], &redirected, g_shaderProgramRedirects, 0x1a1)) {
                program_key.key[0] = redirected;
            } else {
                program_key.key[0] = key[0];
            }
        }

        // cache hit scan: slots at +4, key at +8
        for (u32 id = 0; id < 0x190; id++) {
            u8 *slot = base + 4 + id * SLOT_STRIDE;
            if (*(u32 *)(slot + 8) == program_key.key[0]) {
                (*(i32 *)(slot + 4))++;
                return slot;
            }
        }

        // miss → take the next free slot round-robin from lastSlotIndex
        i32 last = *(i32 *)(base + 0x4bc84);
        for (i32 step = 1; step <= 0x190; step++) {
            i32 id = (last + step) % 0x191;
            u8 *slot = base + 4 + (u32)id * SLOT_STRIDE;
            if (*(i32 *)(slot + 4) <= 0) {
                *(i32 *)(base + 0x4bc84) = id;
                if (ManagerCreateShader(program_key, slot, 0)) {
                    (*(i32 *)(slot + 4))++;
                    return slot;
                }
                return NULL;
            }
        }
        return NULL;
    }

} // namespace nu2api

// --- wrappers matching the signatures numtl.cpp already declares ---------------

extern "C" void *NuShaderManagerRetrieveShader(NUSHADERMTLDESC *desc, void *mtl) {
    return nu2api::RetrieveShader(nu2api::g_shaderManager, desc, mtl, 0, 0, false);
}

extern "C" void *NuShaderManagerRetrieveShaderVariant(NUSHADERMTLDESC *desc, void *mtl, i32 variant) {
    // original 0x308ff0: the template's trailing bool is 0 here too
    return nu2api::RetrieveShader(nu2api::g_shaderManager, desc, mtl, variant, 0x10, false);
}

// SetupMaterial (original 0x30cba0) is not transcribed yet — it walks the
// linked uniform/parameter list of a bound program and pushes semantics via
// the glConstantSetter table. Until it lands the pipeline runs with the
// engine defaults; this is decompilation work in progress, NOT host-only code.
extern "C" void NuShaderObjectGLSLSetupMaterial(i32 program, struct numtl_s *mtl) {
    (void)program;
    (void)mtl;
}

// original .data @0x65e0b8 — { glUniform1fv, glUniform2fv, glUniform3fv,
// glUniform4fv }; consumed through an unmangled declaration.
extern "C" void (*g_glConstantSetterTable[4])(u32 loc, i32 count, const void *vals) = {
    (void (*)(u32, i32, const void *))nu2api::glUniform1fv,
    (void (*)(u32, i32, const void *))nu2api::glUniform2fv,
    (void (*)(u32, i32, const void *))nu2api::glUniform3fv,
    (void (*)(u32, i32, const void *))nu2api::glUniform4fv,
};
