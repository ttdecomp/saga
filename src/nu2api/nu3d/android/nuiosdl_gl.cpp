// GLES2 display-list backend — material, cull, and vertex-format state.
//
// This is the iOS/Android counterpart to the PS2/PICA display-list
// consumer.  The game thread builds display lists (nudlist.cpp) that
// are later drained on the render thread; each list item is a callback
// into this TU:
//
//   NuIOSDLMtlCallback                           original 0x29c480
//   NuIOSDLGeom2DCallback                        original 0x293ad6
//   NuMtlSetRenderStatesPS                       original 0x29c1c0
//   NuIOS_SetCullMode                            original 0x29c110
//   NuIOS_SetVertexFormat                        original 0x29c070
//   NuIOS_BindVertexAttributesImmediate           original 0x2939fe
//   NuIOS_BindVertexAttributesImmediateOverride   original 0x293a65
//   NuIOS_BindVertexAttributesInternal            original 0x293841
//
// Original bss laid the per-TU shader programmes and refraction state
// at 0x99b440.. (g_faceonProgram / g_faceonDecalProgram /
// g_debrisProgram / g_DebrisGlassDistortTID / refractionRT …).

#include "nuiosdl_gl.h"

#include <GLES2/gl2.h>
#include <string.h>

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuapi.h"

// ---------------------------------------------------------------------------
// Engine globals owned by this TU (original bss 0x99b440.. / 0x119b..).
// ---------------------------------------------------------------------------

u32 g_boundShader = 0;
NUSHADERPROGRAM *g_currentShaderProgram = nullptr;
numtl_s *g_boundMaterial = nullptr;
numtl_s *g_renderContext_materialInUse = nullptr;
numtl_s *g_LastMtl = nullptr;
usize g_boundVertexFormat = 0;
u32 g_activeAttributes = 0;
i32 g_renderContext_zFunc = 0;
u32 g_alphaRef = 0;
u32 g_alphaFunc = 0;
i32 g_alphaTestEnabled = 0;
u32 g_lastAlphaRef = 0;
u32 g_lastAlphaBlend = 0;
i32 g_renderingReflection = 0; // original bss @0x99b360 — flips cull when reflecting.

// GLES2 has no VAOs; the original file-static at 0x2a3168 only cached the
// last-bound handle to avoid redundant binds.
static u32 g_lastBoundVAO = 0;
static void NuIOSBindVAO(u32 vao) {
    if (vao != g_lastBoundVAO) {
        g_lastBoundVAO = vao;
    }
}

// Shader programmes cached per TU (original file-statics at 0x99b440..).
static NUSHADERPROGRAM *g_faceonProgram = nullptr;      // _ZL15g_faceonProgram
static NUSHADERPROGRAM *g_faceonDecalProgram = nullptr; // _ZL20g_faceonDecalProgram
static NUSHADERPROGRAM *g_debrisProgram = nullptr;      // _ZL15g_debrisProgram
static NUSHADERPROGRAM *g_debrisGlassProgram = nullptr;

#include "nuios_shader_sources.inc"

i32 g_DebrisGlassDistortTID = 0; // _ZL23g_DebrisGlassDistortTID @0x99b4c8

extern u32 g_DebriVB[8];
extern void *g_DebriSysMemVB[2][64];
extern u32 g_readBufferIndex;

// Refraction texture used by glass debris — lazily allocated.
static i32 NuIOSDLMtlCallback_refractionRT = 0;                 // @0x99b480
static NUNATIVETEX NuIOSDLMtlCallback_nativeRefractionTex = {}; // @0x99b4a0
static i32 NuIOSDLMtlCallback_lastFrameCount = 0;

// ---------------------------------------------------------------------------
// Cross-TU imports.
// ---------------------------------------------------------------------------

extern "C" void NuShaderManagerBindShader(NUSHADEROBJECT *shader);
extern "C" void NuShaderObjectGLSLSetupMaterial(NUSHADEROBJECT *shader_obj, numtl_s *mtl);
extern "C" NUSHADEROBJECT *NuShaderManagerGetShaderById(i32 id);
extern "C" NUSHADEROBJECT *NuShaderManagerGetCurrentShader(void);

extern i32 g_currentTexUnit; // nutex_ios_ex.cpp
extern NUAPI nuapi;

static inline isize PtrToArgInt(const void *p) {
    union {
        const void *ptr;
        isize val;
    } u = {p};
    return u.val;
}

static inline i32 NuApiFrameCount() {
    // Original reads *(i32*)(&nuapi + 0x60) directly.
    return *(i32 *)((u8 *)&nuapi + 0x60);
}

static inline usize ptrToUsize(const void *p) {
    return reinterpret_cast<usize>(p);
}
static inline const void *usizeToPtr(usize value) {
    return reinterpret_cast<const void *>(value);
}

__attribute__((weak)) GLenum NuIOS_PlatformVertexAttributeType(GLenum type) {
    return type;
}

__attribute__((weak)) isize NuIOS_PlatformPrepareImmediateVertexData(isize data_address, usize) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return data_address;
}

NUNATIVETEX *NuTexGetNative(i32 tex_id);
void NuTexSetTextureWithStagePS(NUNATIVETEX *tex, u32 stage);
i32 NuTexGenTexture(NUNATIVETEX *tex);
void NuIOS_CopyBackbufferToTexture(NUNATIVETEX *tex, bool depth);

extern NuVertexFormatPS *g_nuPrimVertexFormat;   // 2D immediate stream format
extern NuVertexFormatPS *g_nuFaceOnVertexFormat; // billboard format
extern NuVertexFormatPS *g_nuDebrisVertexFormat; // debris format

// View matrices from the render context (original bss g_renderContext_viewProj
// @0x119bd00, g_renderContext_view @0x119bdc0).  Debris shaders need both.
extern "C" f32 g_renderContext_viewProj[16];
extern "C" f32 g_renderContext_view[16];
extern "C" f32 g_renderContext_world[16];
extern "C" f32 g_renderContext_kTint[4];
extern void (*g_glConstantSetterTable[4])(u32 loc, i32 count, const void *vals);
extern "C" void NuShaderManagerSetfv(i32 semantic, const f32 *values);
extern "C" void NuRenderContextSetViewProj(NUMTX *view, NUMTX *projection);

// ---------------------------------------------------------------------------
// Material-variant helpers — raw offsets from the original binary.
//
// The header's NUMTL/NUSHADERMTLDESC layout has drifted from the shipped
// binary, so the variant selectors are still addressed by absolute byte
// offset with the original address in the comment.  Named accessors keep
// call-sites readable while preserving the exact bytes the original tested.
// ---------------------------------------------------------------------------

static inline u8 MaterialVariantFlags(const numtl_s *mtl) {
    // Original: *(u8*)((u8*)mtl + 0x1F2) bits 0x10 = debris, 0x20 = face-on.
    return *(const u8 *)((const u8 *)mtl + 0x1F2);
}

static inline char FaceOnDecalSelector(const numtl_s *mtl) {
    // Original: *(char*)((u8*)mtl + 0x268) — maps to shader_desc.unknown_1b4
    // (use mtl->shader_desc.unknown_1b4 when the struct is fully typed).
    return *(const char *)((const u8 *)mtl + 0x268);
}

static inline char DebrisGlassSelector(const numtl_s *mtl) {
    // Original: *(char*)((u8*)mtl + 0x99) == -0x69 => glass debris path.
    return *(const char *)((const u8 *)mtl + 0x99);
}

static constexpr char kGlassDebrisMarker = (char)-0x69; // 0x97

// ---------------------------------------------------------------------------
// GL state helpers.
// ---------------------------------------------------------------------------

// original 0x29c110 — mirrors GL cull state, flipping front/back when the
// reflection pass is active.
void NuIOS_SetCullMode(i32 mode) {
    static i32 s_prevCullMode = 0;                         // @0x628c50
    static i32 s_prevReflection = 0;                       // @0x628c60
    static const u32 kGlCullFace[2] = {GL_BACK, GL_FRONT}; // @0x57bcec

    if (mode == s_prevCullMode && s_prevReflection == g_renderingReflection) {
        return;
    }
    s_prevReflection = g_renderingReflection;

    // Mode 2 = double-sided: disable culling entirely.
    if (mode == 2) {
        glDisable(GL_CULL_FACE);
        s_prevCullMode = 2;
        return;
    }

    if (s_prevCullMode == 2) {
        glEnable(GL_CULL_FACE);
    }

    // Reflection XORs the winding, so the back/front choice is toggled.
    u32 idx = (u32)(mode + g_renderingReflection) & 1;
    glCullFace(kGlCullFace[idx]);
    s_prevCullMode = (i32)idx;
}

// Blend / alpha-test translation — original 0x29c1c0.
//
// The original reads NUMTL fields by raw byte offsets:
//   m[0x40] & 0xf          -> attribs.alpha_mode   (blend selector)
//   (m[0x41] >> 4) & 3     -> attribs.cull_mode
//   (m[0x42] >> 4) & 7     -> attribs.alpha_test   (alpha-test selector)
//   m[0x43]                -> attribs.alpha_ref
// We go through the typed attribs where the mapping is stable and fall
// back to raw bytes for the variant-flag byte at 0x1F2.

enum : u32 {
    kBlendOpaque = 0,
    kBlendAlpha = 1,        // srcA * src + (1-srcA) * dst
    kBlendAdd = 2,          // srcA * src + dst
    kBlendMax = 3,          // GL_MAX per channel (glow)
    kBlendAlphaTest10 = 10, // opaque + alpha-test (0x43 ref, func GEQUAL)
};

extern "C" void NuMtlSetRenderStatesPS(numtl_s *mtl) {
    const u8 *bytes = (const u8 *)mtl;

    // ---- alpha-test setup (mirrors original goto have_alpha flow) ----
    // Original tautology: alpha_ref_byte = (m[0x42]>>7 &1) ? m[0x43] : m[0x43]
    // i.e. always m[0x43].  Kept verbatim for fidelity, but expressed
    // through the typed field when available.
    u8 alpha_ref_byte = mtl->attribs.alpha_ref; // == bytes[0x43]

    const u8 variantFlags = MaterialVariantFlags(mtl);
    const bool isDebris = (variantFlags & 0x10) != 0;

    if (!isDebris) {
        u32 alphaSel = (u32)(mtl->attribs.alpha_test & 7); // (bytes[0x42]>>4)&7
        if (alphaSel > 1) {
            if (alphaSel == 5) {
                g_alphaRef = alpha_ref_byte;
                g_alphaFunc = 5; // GEQUAL
                g_alphaTestEnabled = 1;
            } else {
                g_alphaFunc = 6; // GREATER
                g_alphaTestEnabled = 1;
                g_alphaRef = 0;
            }
        } else if (g_alphaTestEnabled != 0) {
            g_alphaTestEnabled = 0;
        }
    } else {
        // Debris materials force a minimal alpha-test.
        g_alphaFunc = 6;
        g_alphaTestEnabled = 1;
        g_alphaRef = 2;
    }

    // ---- blend mode ----
    u32 blend = mtl->attribs.alpha_mode & 0xf; // bytes[0x40] & 0xf
    switch (blend) {
        case kBlendOpaque:
            glDisable(GL_BLEND);
            break;
        case kBlendAlpha:
            glEnable(GL_BLEND);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case kBlendAdd:
            glEnable(GL_BLEND);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
            break;
        case kBlendMax:
            glEnable(GL_BLEND);
            // 0x800b is GL_MAX on desktop GL; GLES2 exposes it via EXT.
            glBlendEquationSeparate((GLenum)0x800b, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
            break;
        case kBlendAlphaTest10:
            glDisable(GL_BLEND);
            g_alphaTestEnabled = 1;
            g_alphaRef = alpha_ref_byte;
            g_alphaFunc = 5;
            break;
        default:
            break;
    }

    g_lastAlphaRef = alpha_ref_byte;
    g_lastAlphaBlend = blend;

    NuIOS_SetCullMode((bytes[0x41] >> 4) & 3); // attribs.cull_mode
}

// original 0x2a3860
extern "C" void NuRenderContextSetZFunc(i32 zfunc) {
    if (zfunc == g_renderContext_zFunc) {
        return;
    }

    switch (zfunc) {
        case 0: // depth test + write, LEQUAL
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LEQUAL);
            break;
        case 1: // depth test, no write (decal / transparent)
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LEQUAL);
            break;
        case 2: // no depth test, write enabled
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            break;
        case 3: // no depth test, no write (UI / 2D)
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            break;
        default:
            break;
    }
    g_renderContext_zFunc = zfunc;
}

// ---------------------------------------------------------------------------
// Vertex attribute binding — original 0x293841 / 0x2939fe / 0x293a65.
// ---------------------------------------------------------------------------

// Original attribute word array layout (leading dword = active mask,
// then 6 dwords per location):
//   [+0] mask
//   [+1] GL type         (e.g. GL_FLOAT)
//   [+2] component count (1..4)
//   [+3] normalized flag (GL_TRUE/GL_FALSE)
//   [+4] unused / padding
//   [+5] byte offset from vertex base
//   [+6] stride (bytes between vertices)
struct VertexAttribRecord {
    u32 gl_type;
    u32 comp_count;
    u32 normalized;
    u32 pad;
    u32 byte_offset;
    u32 stride;
};

static void NuIOS_BindVertexAttributesInternal(isize dataAddr, usize baseVertex, const u32 *fmtWords, u32 mask) {
    u32 wanted = mask & fmtWords[0];
    u32 toDisable = g_activeAttributes & ~wanted;
    u32 toEnable = ~g_activeAttributes & wanted;
    g_activeAttributes = wanted;

    const u32 *recordsBase = fmtWords + 1; // skip mask

    i32 loc = 0;
    // Walk bit-by-bit so enable/disable is delta-driven like the original.
    do {
        bool wantThis = (wanted & 1) != 0;
        bool needDisable = (toDisable & 1) != 0;
        bool needEnable = (toEnable & 1) != 0;

        if (!wantThis) {
            if (needDisable) {
                glDisableVertexAttribArray(loc);
            }
        } else {
            if (needEnable) {
                glEnableVertexAttribArray(loc);
            }
            const VertexAttribRecord *rec = reinterpret_cast<const VertexAttribRecord *>(recordsBase + loc * 6);
            const GLenum type = NuIOS_PlatformVertexAttributeType(static_cast<GLenum>(rec->gl_type));
            glVertexAttribPointer(loc, (GLint)rec->comp_count, type, (GLboolean)rec->normalized, (GLsizei)rec->stride,
                                  (const void *)(dataAddr + rec->byte_offset + baseVertex * rec->stride));
        }

        ++loc;
        wanted >>= 1;
        toEnable >>= 1;
        toDisable >>= 1;
    } while (wanted != 0 || toEnable != 0 || toDisable != 0);
}

// original 0x2939fe — bind using the currently bound vertex format.
static void NuIOS_BindVertexAttributesImmediate(isize, isize dataAddr) {
    NuIOSBindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    const u32 *fmt = static_cast<const u32 *>(usizeToPtr(g_boundVertexFormat));
    NuIOS_BindVertexAttributesInternal(dataAddr, 0, fmt, fmt[0]);
}

static void NuIOS_BindVertexAttributes(isize dataAddr, usize baseVertex);

void NuIOSDLDebrisCallback(void *data) {
    nunativedebrisdata_s *packet = static_cast<nunativedebrisdata_s *>(data);
    if (packet->vertex_count == 0) {
        return;
    }
    g_boundVertexFormat = ptrToUsize(g_nuDebrisVertexFormat);
    if (packet->use_system_memory_vb == 0) {
        NuIOSBindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, g_DebriVB[g_readBufferIndex * 4 + packet->vertex_buffer_index]);
        NuIOS_BindVertexAttributes(0, 0);
    } else {
        NuIOS_BindVertexAttributesImmediate(
            0, PtrToArgInt(g_DebriSysMemVB[g_readBufferIndex][packet->vertex_buffer_index]));
    }
    glDrawArrays(GL_TRIANGLES, packet->first_vertex, packet->vertex_count);
}

static void NuIOS_BindVertexAttributes(isize dataAddr, usize baseVertex) {
    const u32 *fmt = static_cast<const u32 *>(usizeToPtr(g_boundVertexFormat));
    NuIOS_BindVertexAttributesInternal(dataAddr, baseVertex, fmt, fmt[0]);
}

// original 0x293a65 — bind with an explicit format override (2D path).
static void NuIOS_BindVertexAttributesImmediateOverrideDataLayout(isize dataAddr, usize baseVertex, const u32 *fmt) {
    NuIOSBindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    NuIOS_BindVertexAttributesInternal(dataAddr, baseVertex, fmt, fmt[0]);
}

// ---------------------------------------------------------------------------
// Display-list callbacks.
// ---------------------------------------------------------------------------

// original 0x29c070
extern "C" void NuIOS_SetVertexFormat(usize fmt) {
    g_boundVertexFormat = fmt;
}

// Helpers for the debris constant block (original walks the programme's
// i16 param table: pairs of {semantic, loc|class}).
namespace {
    constexpr i16 kParamViewProj = 0;        // semantic 0
    constexpr i16 kParamView = 0x0c;         // semantic 12
    constexpr i16 kParamKonstColourA = 0x30; // semantic 48
    constexpr i16 kParamTerminator = (i16)-0x8000;

    // Zero-initialized shader constants at original BSS addresses 0x6463bc
    // and 0x646360. They are only read by NuIOSDLMtlCallback.
    NUVEC4 g_debrisShaderConstantA = {};
    NUVEC4 g_debrisShaderConstantB = {};

    const NUSHADERPROGRAMPARAMETER *FindParam(const NUSHADERPROGRAMPARAMETER *params, i32 count, u16 semantic) {
        if (count <= 0 || params == nullptr) {
            return nullptr;
        }
        for (i32 i = 0; i < count; ++i) {
            if (params[i].register_index == semantic) {
                return &params[i];
            }
        }
        // Original falls through to the last entry when not found — mimic that
        // by returning the last pair so the caller still indexes [1].
        return &params[count - 1];
    }

    void UploadDebrisConstants(NUSHADERPROGRAM *program) {
        if (program == nullptr) {
            return;
        }
        i32 count = program->parameter_count;
        const NUSHADERPROGRAMPARAMETER *params = program->parameters;
        if (count < 1 || params == nullptr) {
            return;
        }

        auto upload = [&](i16 semantic, i32 vec4Count, const void *src) {
            const NUSHADERPROGRAMPARAMETER *parameter = FindParam(params, count, semantic);
            // Encoding of params[][1]: low 12 bits = uniform location,
            // high nibble (bits 12..15) encodes the setter-table class.
            u32 loc = parameter->location_and_setter & 0x0fff;
            u32 klass = parameter->location_and_setter >> 12;
            (*g_glConstantSetterTable[klass])(loc, vec4Count, src);
        };

        upload(kParamViewProj, 4, g_renderContext_viewProj);
        upload(kParamView, 4, g_renderContext_view);
        upload(kParamKonstColourA, 1, &g_debrisShaderConstantA);
        upload(kParamTerminator, 1, &g_debrisShaderConstantB);
    }

} // namespace

// original 0x29c000 — the four special-material programs are built from
// shader strings compiled into libTTapp.so rather than from scene resources.
extern "C" void NuIOSMtlInit(void) {
    g_faceonProgram = NuShaderProgramCreateIOS(reinterpret_cast<const char *>(FaceOn_vx),
                                               reinterpret_cast<const char *>(FaceOn_Hi_px));
    g_faceonDecalProgram = NuShaderProgramCreateIOS(reinterpret_cast<const char *>(FaceOn_vx),
                                                    reinterpret_cast<const char *>(FaceOn_Hi_px));
    g_debrisProgram =
        NuShaderProgramCreateIOS(reinterpret_cast<const char *>(debris_vx), reinterpret_cast<const char *>(debris_px));
    g_debrisGlassProgram = NuShaderProgramCreateIOS(reinterpret_cast<const char *>(debris_vx),
                                                    reinterpret_cast<const char *>(debris_glass_px));
}

static void NuIOS_GetAlphaTestParameters(f32 parameters[2]) {
    const f32 alpha_ref = static_cast<f32>(g_alphaRef) * (1.0f / 255.0f);

    if (g_alphaTestEnabled == 0) {
        parameters[0] = 0.0f;
        parameters[1] = -1.0f;
        return;
    }

    switch (g_alphaFunc) {
        case 2:
            parameters[0] = -1.0f;
            parameters[1] = -alpha_ref - (1.0f / 255.0f);
            if (parameters[1] <= 0.0f) {
                parameters[1] = 0.0f;
            }
            break;
        case 3:
            parameters[0] = -1.0f;
            parameters[1] = -alpha_ref;
            break;
        case 5:
            parameters[0] = 1.0f;
            parameters[1] = alpha_ref;
            break;
        case 6:
            parameters[0] = 1.0f;
            parameters[1] = alpha_ref + (1.0f / 255.0f);
            break;
        default:
            parameters[0] = 0.0f;
            parameters[1] = -1.0f;
            break;
    }
}

// original 0x29bf20 — install the matrices and constants consumed by FaceOn_vx.
void NuIOSDLFaceOnTransformCallback(void *arg) {
    auto *packet = static_cast<NuFaceOnTransformPacket *>(arg);
    NUMTX world = packet->world;
    const f32 opacity = world.m33;
    const f32 translation[4] = {world.m30, world.m31, world.m32, 1.0f};
    const f32 scale[4] = {packet->magnitude, 1.0f, 0.0f, 0.0f};
    f32 tint[4] = {
        g_renderContext_kTint[0],
        g_renderContext_kTint[1],
        g_renderContext_kTint[2],
        g_renderContext_kTint[3] * opacity,
    };
    f32 alpha_test[2];

    world.m30 = 0.0f;
    world.m31 = 0.0f;
    world.m32 = 0.0f;
    world.m33 = 1.0f;

    NuShaderProgramSetVertexParamfv(g_currentShaderProgram, 0x50, reinterpret_cast<const f32 *>(&world), 16);
    NuShaderProgramSetVertexParamfv(g_currentShaderProgram, 0x59, translation, 4);
    NuShaderProgramSetVertexParamfv(g_currentShaderProgram, 0x54, scale, 4);
    NuShaderProgramSetVertexParamfv(g_currentShaderProgram, 0x55, reinterpret_cast<const f32 *>(&packet->face_on), 16);
    NuShaderProgramSetVertexParamfv(g_currentShaderProgram, 0, g_renderContext_viewProj, 16);
    NuShaderProgramSetVertexParamfv(g_currentShaderProgram, 0xc, reinterpret_cast<const f32 *>(&packet->face_on), 16);
    NuIOS_GetAlphaTestParameters(alpha_test);
    NuShaderProgramSetFragmentParamfv(g_currentShaderProgram, 0x70, alpha_test, 2);
    NuShaderProgramSetVertexParamfv(g_currentShaderProgram, 0x28, tint, 4);

    if (opacity < 1.0f) {
        NuRenderContextSetZFunc(1);
    } else if (g_renderContext_materialInUse != nullptr) {
        NuRenderContextSetZFunc(g_renderContext_materialInUse->attribs.z_mode);
    }
}

// original 0x29bec0 — every face-on entry is expanded to two triangles in
// the scene vertex buffer.
void NuIOSDLFaceOnCallback(void *arg) {
    auto *packet = static_cast<NuFaceOnDrawPacket *>(arg);
    if (packet == nullptr) {
        return;
    }

    NuIOSBindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, packet->vertex_buffer);
    g_boundVertexFormat = ptrToUsize(g_nuFaceOnVertexFormat);
    NuIOS_BindVertexAttributes(0, 0);
    glDrawArrays(GL_TRIANGLES, packet->first_vertex, packet->face_count * 6);
}

// original 0x29c480 — per-material display-list callback.  Selects the GL
// programme, vertex format, and textures for the three material families:
//
//   * standard   (variantFlags & 0x10 == 0, & 0x20 == 0) — shader-manager
//     programme keyed by mtl->shader_desc.shader_id.
//   * face-on    (0x20 != 0) — billboard programmes g_faceonProgram /
//     g_faceonDecalProgram, driven by shader_desc.unknown_1b4.
//   * debris     (0x10 != 0) — g_debrisProgram or g_debrisGlassProgram;
//     glass additionally copies the backbuffer into refractionRT and binds
//     the distortion map at stage 1.
//
void NuIOSDLMtlCallback(void *arg) {
    auto *mtl = (numtl_s *)arg;

    g_boundMaterial = mtl;
    g_LastMtl = mtl;
    g_renderContext_materialInUse = mtl;
    NuIOS_SetVertexFormat(ptrToUsize(mtl->vertex_decl));

    NUSHADEROBJECT *shaderId = NuShaderManagerGetShaderById(mtl->shader_desc.shader_id);
    u8 variantFlags = MaterialVariantFlags(mtl);

    const bool isDebris = (variantFlags & 0x10) != 0;
    const bool isFaceOn = (variantFlags & 0x20) != 0;

    if (!isDebris) {
        if (!isFaceOn) {
            // ---- Standard material ----
            if (shaderId != 0) {
                g_boundShader = 0;
                glUseProgram(0);
                g_currentShaderProgram = nullptr;
                NuShaderManagerBindShader(shaderId);
                // BindShader may clobber the format; restore it.
                NuIOS_SetVertexFormat(ptrToUsize(mtl->vertex_decl));
            }
        } else {
            // ---- Face-on / billboard ----
            NuShaderManagerBindShader(0);
            g_boundVertexFormat = ptrToUsize(g_nuFaceOnVertexFormat);

            char decalSel = FaceOnDecalSelector(mtl);
            NUSHADERPROGRAM *program = (decalSel == '\0') ? g_faceonProgram : g_faceonDecalProgram;

            g_boundShader = program != nullptr ? program->program : 0;
            glUseProgram(g_boundShader);
            g_currentShaderProgram = program;

            NUNATIVETEX *tex = NuTexGetNative(mtl->tex_id);
            if (tex != nullptr) {
                NuTexSetTextureWithStagePS(tex, 0);
            }
        }
    } else {
        // ---- Debris ----
        bool isGlass = (DebrisGlassSelector(mtl) == kGlassDebrisMarker);

        if (isGlass) {
            if (NuIOSDLMtlCallback_refractionRT == 0) {
                NuIOSDLMtlCallback_refractionRT = NuTexGenTexture(&NuIOSDLMtlCallback_nativeRefractionTex);
                memset(&NuIOSDLMtlCallback_nativeRefractionTex, 0, 8);
            }
            if (NuApiFrameCount() != NuIOSDLMtlCallback_lastFrameCount) {
                NuIOS_CopyBackbufferToTexture(&NuIOSDLMtlCallback_nativeRefractionTex, true);
                NuIOSDLMtlCallback_lastFrameCount = NuApiFrameCount();
            }
        }

        NUSHADERPROGRAM *program = isGlass ? g_debrisGlassProgram : g_debrisProgram;

        g_boundVertexFormat = ptrToUsize(g_nuDebrisVertexFormat);
        NuShaderManagerBindShader(0);

        g_boundShader = program != nullptr ? program->program : 0;
        glUseProgram(g_boundShader);
        g_currentShaderProgram = program;

        UploadDebrisConstants(program);

        if (isGlass) {
            glActiveTexture(GL_TEXTURE0);
            g_currentTexUnit = 0;
            glBindTexture(GL_TEXTURE_2D, NuIOSDLMtlCallback_nativeRefractionTex.platform.gl_tex);
            NUNATIVETEX *distort = NuTexGetNative(g_DebrisGlassDistortTID);
            NuTexSetTextureWithStagePS(distort, 1);
        } else {
            NUNATIVETEX *tex = NuTexGetNative(*(i16 *)((u8 *)mtl + 0x74)); // mtl->tex_id
            NuTexSetTextureWithStagePS(tex, 0);
        }
    }

    NuRenderContextSetZFunc(mtl->attribs.z_mode);
    g_renderingReflection = 0;
    NuMtlSetRenderStatesPS(mtl);
}

// original 0x293ad6 — 2D geometry callback.  Binds the 2D vertex format
// and issues the draw for the geom item built by nurndr_plain's Prim2D.
void NuIOSDLGeom2DCallback(void *arg) {
    struct Geom2DItem {
        u32 prim_type;
        u32 pad0;
        u16 pad1;
        u16 vertex_count; // at +0xa
        u32 pad2;
        u8 vertices[0]; // tightly packed PrimVertexRaw array
    };

    auto *geom = (Geom2DItem *)arg;
    if (geom->vertex_count == 0) {
        return;
    }

    NUSHADEROBJECT *shader = NuShaderManagerGetShaderById(g_LastMtl ? g_LastMtl->shader_desc.shader_id : -1);

    if (shader == NULL || shader->glsl.program == 0) {
        return;
    }

    NuIOSBindVAO(0);
    NuShaderObjectGLSLSetupMaterial(shader, g_LastMtl);

    static const u32 kPrimModes[5] = {
        GL_TRIANGLES,      // 0
        GL_TRIANGLE_STRIP, // 1
        GL_LINES,          // 2
        GL_LINE_LOOP,      // 3
        GL_TRIANGLES,      // 4 — quads expanded to triangles by NuPrim2DAddXYZ
    };

    u32 pt = geom->prim_type;
    if (pt < 5) {
        NuIOS_BindVertexAttributesImmediateOverrideDataLayout(PtrToArgInt(geom->vertices), 0,
                                                              (const u32 *)g_nuPrimVertexFormat);
        glDrawArrays((GLenum)kPrimModes[pt], 0, (GLsizei)geom->vertex_count);
    }
}

// original 0x2a430d — 3D geometry callback
void NuIOSDLGeomCallback(void *arg) {
    auto *geom = static_cast<NUDISPLAYLISTGEOM *>(arg);
    const isize immediate_vertices = reinterpret_cast<isize>(geom + 1);
    NUSHADEROBJECT *shader = NuShaderManagerGetCurrentShader();
    if (shader == NULL || shader->glsl.program == 0) {
        return;
    }

    NuShaderObjectGLSLSetupMaterial(shader, g_LastMtl);
    switch (geom->primitive_type) {
        case 0:
            NuIOS_BindVertexAttributesImmediateOverrideDataLayout(immediate_vertices, 0,
                                                                  (const u32 *)g_nuPrimVertexFormat);
            glDrawArrays(GL_TRIANGLES, 0, geom->vertex_count);
            break;
        case 1:
            NuIOS_BindVertexAttributesImmediateOverrideDataLayout(immediate_vertices, 0,
                                                                  (const u32 *)g_nuPrimVertexFormat);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, geom->vertex_count);
            break;
        case 2:
            NuIOS_BindVertexAttributesImmediateOverrideDataLayout(immediate_vertices, 0,
                                                                  (const u32 *)g_nuPrimVertexFormat);
            glDrawArrays(GL_LINES, 0, geom->vertex_count);
            break;
        case 3:
            NuIOS_BindVertexAttributesImmediateOverrideDataLayout(immediate_vertices, 0,
                                                                  (const u32 *)g_nuPrimVertexFormat);
            glDrawArrays(GL_LINE_STRIP, 0, geom->vertex_count);
            break;
        case 5:
            NuIOS_BindVertexAttributesImmediateOverrideDataLayout(immediate_vertices, 0,
                                                                  (const u32 *)g_nuPrimVertexFormat);
            glDrawArrays(GL_TRIANGLE_FAN, 0, geom->vertex_count);
            break;
        case 6: {
            i32 index_count = geom->index_count + 2;
            if (geom->immediate == 0) {
                NuIOS_SetVertexFormat(geom->vertex_format);
                NuIOSBindVAO(0);
                glBindBuffer(GL_ARRAY_BUFFER, geom->vertex_buffer);
                NuIOS_BindVertexAttributes(0, geom->base_vertex);
            } else if (geom->dynamic_vertex_data == nullptr) {
                NuIOSBindVAO(0);
                const isize data_address = geom->vertex_buffer + geom->vertex_stride * geom->base_vertex;
                const usize data_size = geom->vertex_stride * geom->vertex_count;
                NuIOS_BindVertexAttributes(NuIOS_PlatformPrepareImmediateVertexData(data_address, data_size), 0);
            } else {
                NuIOSBindVAO(0);
                glBindBuffer(GL_ARRAY_BUFFER, geom->vertex_format);
                glBufferData(GL_ARRAY_BUFFER, geom->vertex_stride * geom->vertex_count, nullptr, GL_DYNAMIC_DRAW);
                glBufferData(GL_ARRAY_BUFFER, geom->vertex_stride * geom->vertex_count, geom->dynamic_vertex_data,
                             GL_DYNAMIC_DRAW);
                NuIOS_BindVertexAttributes(0, 0);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom->index_buffer);
            glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_SHORT,
                           (const void *)(usize)(geom->first_index * 2));
            break;
        }
        case 0x32:
            NuIOS_BindVertexAttributesImmediateOverrideDataLayout(immediate_vertices, 0,
                                                                  (const u32 *)g_nuPrimVertexFormat);
            glDrawArrays(GL_POINTS, 0, geom->vertex_count);
            break;
    }
    NuIOSBindVAO(0);
}

// original 0x2a3c20 — installs a display-list world transform and applies
// the per-instance opacity to the current tint.
void NuIOSDLTransformCallback(void *arg) {
    auto *world = static_cast<NUMTX *>(arg);
    const f32 opacity = world->m33;
    const f32 shadow_factor = world->m23;
    f32 tint[4] = {
        g_renderContext_kTint[0],
        g_renderContext_kTint[1],
        g_renderContext_kTint[2],
        g_renderContext_kTint[3],
    };

    if (opacity < 1.0f) {
        tint[3] *= opacity;
        NuRenderContextSetZFunc(1);
    } else if (g_renderContext_materialInUse != nullptr) {
        NuRenderContextSetZFunc(g_renderContext_materialInUse->attribs.z_mode);
    }
    NuShaderManagerSetfv(0x44, tint);

    world->m33 = 1.0f;
    world->m23 = 0.0f;
    memcpy(g_renderContext_world, world, sizeof(NUMTX));
    NuShaderManagerSetfv(0x3c, reinterpret_cast<const f32 *>(world));
    world->m33 = opacity;
    world->m23 = shadow_factor;
}

// original 0x2a3e20 — dynamic special transforms are stored transposed in
// the render stream. Restore the ordinary world matrix before publishing it
// to the shader state.
void NuIOSDLTransformParamsCallback(void *arg) {
    auto *stream_matrix = static_cast<NUMTX *>(arg);
    const f32 opacity = stream_matrix->m33;
    const f32 shadow_factor = stream_matrix->m32;
    f32 tint[4] = {
        g_renderContext_kTint[0],
        g_renderContext_kTint[1],
        g_renderContext_kTint[2],
        g_renderContext_kTint[3],
    };

    if (opacity < 1.0f) {
        tint[3] *= opacity;
        NuRenderContextSetZFunc(1);
    } else if (g_renderContext_materialInUse != nullptr) {
        NuRenderContextSetZFunc(g_renderContext_materialInUse->attribs.z_mode);
    }
    NuShaderManagerSetfv(0x44, tint);

    stream_matrix->m33 = 1.0f;
    stream_matrix->m32 = 0.0f;
    NuMtxTranspose(reinterpret_cast<NUMTX *>(g_renderContext_world), stream_matrix);
    NuShaderManagerSetfv(0x3c, g_renderContext_world);
    stream_matrix->m33 = opacity;
    stream_matrix->m32 = shadow_factor;
}

void NuIOSDLKonstCallback(void *arg) {
    const f32 *tint = static_cast<const f32 *>(arg);
    memcpy(g_renderContext_kTint, tint, sizeof(g_renderContext_kTint));
    NuShaderManagerSetfv(0x44, tint);
}

// original 0x2a4dfe — installs the light packet produced by
// RndrStateBuildLightState into the shader semantic state.
void NuIOSDLLightsCallback(void *arg) {
    auto *lights = static_cast<NULIGHTSTATE *>(arg);
    NuShaderManagerSetfv(0x35, reinterpret_cast<const f32 *>(&lights->ambient_intensity));
    NuShaderManagerSetfv(0x39, reinterpret_cast<const f32 *>(&lights->light_direction[0]));
    NuShaderManagerSetfv(0x3a, reinterpret_cast<const f32 *>(&lights->light_direction[1]));
    NuShaderManagerSetfv(0x3b, reinterpret_cast<const f32 *>(&lights->light_direction[2]));
    NuShaderManagerSetfv(0x36, reinterpret_cast<const f32 *>(&lights->light_intensity[0]));
    NuShaderManagerSetfv(0x37, reinterpret_cast<const f32 *>(&lights->light_intensity[1]));
    NuShaderManagerSetfv(0x38, reinterpret_cast<const f32 *>(&lights->light_intensity[2]));
    NuShaderManagerSetfv(0x4b, reinterpret_cast<const f32 *>(&lights->specular_mtx));

    NUVEC4 average_direction = {
        lights->light_direction[0].x + lights->light_direction[1].x + lights->light_direction[2].x,
        lights->light_direction[0].y + lights->light_direction[1].y + lights->light_direction[2].y,
        lights->light_direction[0].z + lights->light_direction[1].z + lights->light_direction[2].z,
        1.0f,
    };
    NuVecNorm(reinterpret_cast<NUVEC *>(&average_direction), reinterpret_cast<NUVEC *>(&average_direction));
    NuShaderManagerSetfv(0x4e, &average_direction.x);

    const f32 max_r = lights->light_intensity[1].r <= lights->light_intensity[2].r ? lights->light_intensity[2].r
                                                                                   : lights->light_intensity[1].r;
    const f32 max_g = lights->light_intensity[1].g <= lights->light_intensity[2].g ? lights->light_intensity[2].g
                                                                                   : lights->light_intensity[1].g;
    const f32 max_b = lights->light_intensity[1].b <= lights->light_intensity[2].b ? lights->light_intensity[2].b
                                                                                   : lights->light_intensity[1].b;
    f32 average_colour[4] = {
        lights->light_intensity[0].r <= max_r ? max_r : lights->light_intensity[0].r,
        lights->light_intensity[0].g <= max_g ? max_g : lights->light_intensity[0].g,
        lights->light_intensity[0].b <= max_b ? max_b : lights->light_intensity[0].b,
        1.0f,
    };
    NuShaderManagerSetfv(0x4d, average_colour);
    f32 specular_intensity[4] = {
        lights->specular_intensity.x,
        lights->specular_intensity.y,
        lights->specular_intensity.z,
        1.0f,
    };
    NuShaderManagerSetfv(0x57, specular_intensity);
}

// original 0x2a4030 — camera packets carry view/projection matrices at +4
// and +0x44 respectively. The viewport tail is deliberately a no-op on this
// platform, matching NuRenderContextSetViewport in the original.
void NuIOSDLCameraCallback(void *arg) {
    struct NuIOSCameraPacket {
        i32 id;
        NUMTX view;
        NUMTX projection;
        f32 viewport[4];
    };
    static i32 last_id = -1;
    auto *packet = static_cast<NuIOSCameraPacket *>(arg);
    if (packet->id != last_id) {
        last_id = packet->id;
        NuRenderContextSetViewProj(&packet->view, &packet->projection);
    }
}

// original 0x2a45d0 — records the material's vertex format on static geometry
// while a scene is being fixed up.
void NuIOSDLPreWarmGeomCallback(void *arg) {
    if (g_LastMtl == nullptr || g_LastMtl->shader_desc.blend_op2 == 0xff) {
        return;
    }
    NUSHADEROBJECT *shader = NuShaderManagerGetShaderById(g_LastMtl->shader_desc.shader_id);
    if (shader == NULL || shader->glsl.program == 0) {
        return;
    }

    auto *geometry = static_cast<NUDISPLAYLISTGEOM *>(arg);
    NuIOSBindVAO(0);
    if (geometry->primitive_type == 6 && geometry->immediate == 0) {
        geometry->vertex_format = g_boundVertexFormat;
    }
}
