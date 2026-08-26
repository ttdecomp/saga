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
#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuapi.h"

// ---------------------------------------------------------------------------
// Engine globals owned by this TU (original bss 0x99b440.. / 0x119b..).
// ---------------------------------------------------------------------------

u32 g_boundShader = 0;
i32 g_currentShaderProgram = 0;
numtl_s *g_boundMaterial = nullptr;
numtl_s *g_renderContext_materialInUse = nullptr;
numtl_s *g_LastMtl = nullptr;
u32 g_boundVertexFormat = 0; // NuVertexFormatPS* as integer (attribute-word array).
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
static i32 g_faceonProgram = 0;       // _ZL15g_faceonProgram
static i32 g_faceonDecalProgram = 0;  // _ZL20g_faceonDecalProgram
static i32 g_debrisProgram = 0;       // _ZL15g_debrisProgram
static i32 g_debrisGlassProgram = 0;

i32 g_DebrisGlassDistortTID = 0; // _ZL23g_DebrisGlassDistortTID @0x99b4c8

// Refraction texture used by glass debris — lazily allocated.
static i32 NuIOSDLMtlCallback_refractionRT = 0;                 // @0x99b480
static NUNATIVETEX NuIOSDLMtlCallback_nativeRefractionTex = {}; // @0x99b4a0
static i32 NuIOSDLMtlCallback_lastFrameCount = 0;

// ---------------------------------------------------------------------------
// Cross-TU imports.
// ---------------------------------------------------------------------------

extern "C" void NuShaderManagerBindShader(i32 shader);
extern "C" void NuShaderObjectGLSLSetupMaterial(i32 shader_obj, numtl_s *mtl);
extern "C" i32 NuShaderManagerGetShaderById(i32 id);

extern i32 g_currentTexUnit; // nutex_ios_ex.cpp
extern NUAPI nuapi;

static inline isize PtrToArgInt(const void *p) {
    union { const void *ptr; isize val; } u = {p};
    return u.val;
}

static inline i32 NuApiFrameCount() {
    // Original reads *(i32*)(&nuapi + 0x60) directly.
    return *(i32 *)((u8 *)&nuapi + 0x60);
}

static inline u32 ptrToU32(const void *p) {
    union { const void *ptr; u32 id; } u{p};
    return u.id;
}
static inline const void *u32ToPtr(u32 id) {
    union { u32 id; const void *ptr; } u{id};
    return u.ptr;
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
extern void (*g_glConstantSetterTable[4])(u32 loc, i32 count, const void *vals);

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
    static i32 s_prevCullMode = 0;       // @0x628c50
    static i32 s_prevReflection = 0;     // @0x628c60
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
    kBlendOpaque       = 0,
    kBlendAlpha        = 1,  // srcA * src + (1-srcA) * dst
    kBlendAdd          = 2,  // srcA * src + dst
    kBlendMax          = 3,  // GL_MAX per channel (glow)
    kBlendAlphaTest10  = 10, // opaque + alpha-test (0x43 ref, func GEQUAL)
};

void NuMtlSetRenderStatesPS(numtl_s *mtl) {
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
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                                GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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

static void NuIOS_BindVertexAttributesInternal(isize dataAddr, usize baseVertex,
                                               const u32 *fmtWords, u32 mask) {
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
            const VertexAttribRecord *rec =
                reinterpret_cast<const VertexAttribRecord *>(recordsBase + loc * 6);
            glVertexAttribPointer(
                loc, (GLint)rec->comp_count, (GLenum)rec->gl_type,
                (GLboolean)rec->normalized, (GLsizei)rec->stride,
                (const void *)(dataAddr + rec->byte_offset + baseVertex * rec->stride));
        }

        ++loc;
        wanted >>= 1;
        toEnable >>= 1;
        toDisable >>= 1;
    } while (wanted != 0 || toEnable != 0 || toDisable != 0);
}

// original 0x2939fe — bind using the currently bound vertex format.
void NuIOS_BindVertexAttributesImmediate(isize dataAddr, usize baseVertex) {
    NuIOSBindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    const u32 *fmt = (const u32 *)u32ToPtr(g_boundVertexFormat); // NOLINT
    NuIOS_BindVertexAttributesInternal(dataAddr, baseVertex, fmt, fmt[0]);
}

// original 0x293a65 — bind with an explicit format override (2D path).
void NuIOS_BindVertexAttributesImmediateOverrideDataLayout(isize dataAddr, usize baseVertex,
                                                           const u32 *fmt) {
    NuIOSBindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    NuIOS_BindVertexAttributesInternal(dataAddr, baseVertex, fmt, fmt[0]);
}

// ---------------------------------------------------------------------------
// Display-list callbacks.
// ---------------------------------------------------------------------------

// original 0x29c070
extern "C" void NuIOS_SetVertexFormat(u32 fmt) {
    g_boundVertexFormat = fmt;
}

// Helpers for the debris constant block (original walks the programme's
// i16 param table: pairs of {semantic, loc|class}).
namespace {

struct ShaderProgramView {
    u32 pad[2];
    GLuint gl_prog; // at +8
    i32 param_count; // at +0xC
    i16 *params;    // at +0x10 — array of i16 pairs
};

constexpr i16 kParamViewProj     = 0;       // semantic 0
constexpr i16 kParamView         = 0x0c;    // semantic 12
constexpr i16 kParamKonstColourA = 0x30;    // semantic 48
constexpr i16 kParamTerminator   = (i16)-0x8000;

// The original stores two global colours at fixed rodata addresses
// (0x6463bc and 0x646360).  They are the debris tint / light constants.
const void *kDebrisTintA = (const void *)0x6463bc;
const void *kDebrisTintB = (const void *)0x646360;

const i16 *FindParam(const i16 *params, i32 count, i16 semantic) {
    if (count <= 0 || params == nullptr) {
        return nullptr;
    }
    for (i32 i = 0; i < count; ++i) {
        if (params[i * 2] == semantic) {
            return &params[i * 2];
        }
    }
    // Original falls through to the last entry when not found — mimic that
    // by returning the last pair so the caller still indexes [1].
    return &params[(count - 1) * 2];
}

void UploadDebrisConstants(i32 progHandle) {
    if (progHandle == 0) {
        return;
    }
    const auto *prog = (const ShaderProgramView *)(uintptr_t)progHandle; // NOLINT
    i32 count = prog->param_count;
    const i16 *params = prog->params;
    if (count < 1 || params == nullptr) {
        return;
    }

    auto upload = [&](i16 semantic, i32 vec4Count, const void *src) {
        const i16 *p = FindParam(params, count, semantic);
        // Encoding of params[][1]: low 12 bits = uniform location,
        // high nibble (bits 12..15) encodes the setter-table class.
        u32 loc = (u32)(p[1] & 0x0fff);
        u32 klass = ((const u8 *)p)[3] >> 4;
        (*g_glConstantSetterTable[klass])(loc, vec4Count, src);
    };

    upload(kParamViewProj,     4, g_renderContext_viewProj);
    upload(kParamView,         4, g_renderContext_view);
    upload(kParamKonstColourA, 1, kDebrisTintA);
    upload(kParamTerminator,   1, kDebrisTintB);
}

} // namespace

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
    NuIOS_SetVertexFormat(ptrToU32(mtl->vertex_decl)); // NOLINT

    i32 shaderId = NuShaderManagerGetShaderById(mtl->shader_desc.shader_id);
    u8 variantFlags = MaterialVariantFlags(mtl);

    const bool isDebris = (variantFlags & 0x10) != 0;
    const bool isFaceOn = (variantFlags & 0x20) != 0;

    if (!isDebris) {
        if (!isFaceOn) {
            // ---- Standard material ----
            if (shaderId != 0) {
                g_boundShader = 0;
                glUseProgram(0);
                g_currentShaderProgram = 0;
                NuShaderManagerBindShader(shaderId);
                // BindShader may clobber the format; restore it.
                NuIOS_SetVertexFormat(ptrToU32(mtl->vertex_decl)); // NOLINT
            }
        } else {
            // ---- Face-on / billboard ----
            NuShaderManagerBindShader(0);
            g_boundVertexFormat = ptrToU32(g_nuFaceOnVertexFormat); // NOLINT

            char decalSel = FaceOnDecalSelector(mtl);
            i32 prog = (decalSel == '\0') ? g_faceonProgram : g_faceonDecalProgram;

            if (prog != 0) {
                auto *view = (ShaderProgramView *)(uintptr_t)prog; // NOLINT
                g_boundShader = view->gl_prog;
            } else {
                g_boundShader = 0;
            }
            glUseProgram(g_boundShader);
            g_currentShaderProgram = prog;

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
                NuIOSDLMtlCallback_refractionRT =
                    NuTexGenTexture(&NuIOSDLMtlCallback_nativeRefractionTex);
                memset(&NuIOSDLMtlCallback_nativeRefractionTex, 0, 8);
            }
            if (NuApiFrameCount() != NuIOSDLMtlCallback_lastFrameCount) {
                NuIOS_CopyBackbufferToTexture(&NuIOSDLMtlCallback_nativeRefractionTex, true);
                NuIOSDLMtlCallback_lastFrameCount = NuApiFrameCount();
            }
        }

        i32 prog = isGlass ? g_debrisGlassProgram : g_debrisProgram;

        g_boundVertexFormat = ptrToU32(g_nuDebrisVertexFormat); // NOLINT
        NuShaderManagerBindShader(0);

        if (prog == 0) {
            g_boundShader = 0;
        } else {
            auto *view = (ShaderProgramView *)(uintptr_t)prog; // NOLINT
            g_boundShader = view->gl_prog;
        }
        glUseProgram(g_boundShader);
        g_currentShaderProgram = prog;

        UploadDebrisConstants(prog);

        if (isGlass) {
            glActiveTexture(GL_TEXTURE0);
            g_currentTexUnit = 0;
            glBindTexture(GL_TEXTURE_2D,
                          NuIOSDLMtlCallback_nativeRefractionTex.platform.gl_tex);
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

    i32 shader = NuShaderManagerGetShaderById(
        g_LastMtl ? g_LastMtl->shader_desc.shader_id : -1);

    struct ShaderObjView {
        u32 pad[4];
        GLuint gl_prog;
    };
    auto *obj = (ShaderObjView *)(uintptr_t)shader; // NOLINT
    if (shader == 0 || obj->gl_prog == 0) {
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
        NuIOS_BindVertexAttributesImmediateOverrideDataLayout(
            PtrToArgInt(geom->vertices), 0, (const u32 *)g_nuPrimVertexFormat);
        glDrawArrays((GLenum)kPrimModes[pt], 0, (GLsizei)geom->vertex_count);
    }
}
