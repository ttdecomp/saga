// GLES2 display-list callbacks and immediate-mode GL state.
//
// This TU mirrors the original ios/android display-list consumer side:
//   NuIOSDLMtlCallback        original 0x29c480
//   NuIOSDLGeom2DCallback     original 0x293ad6
//   NuMtlSetRenderStatesPS    original 0x29c1c0
//   NuIOS_SetCullMode         original 0x29c110
//   NuIOS_SetVertexFormat     original 0x29c070
//   NuIOS_BindVertexAttributesImmediate          original 0x2939fe
//   NuIOS_BindVertexAttributesImmediateOverrideDataLayout original 0x293a65
//   NuIOS_BindVertexAttributesInternal           original 0x293841
//
// The per-TU bss globals below live at 0x99b440.. in the original
// (_ZL15g_debrisProgram / _ZL20g_faceonDecalProgram / _ZL15g_faceonProgram /
// _ZZ18NuIOSDLMtlCallbackPvE12refractionRT / ...E19nativeRefractionTex /
// _ZL23g_DebrisGlassDistortTID).

#include "nuiosdl_gl.h"

#include <GLES2/gl2.h>
#include <string.h>

#include "decomp.h"
#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuapi.h"

// --- globals shared with other engine TUs -------------------------------

u32 g_boundShader;
i32 g_currentShaderProgram;
numtl_s *g_boundMaterial;
numtl_s *g_renderContext_materialInUse;
numtl_s *g_LastMtl;
u32 g_boundVertexFormat; // NuVertexFormatPS* (attribute word array)
u32 g_activeAttributes;
i32 g_renderContext_zFunc;
u32 g_alphaRef;
u32 g_alphaFunc;
i32 g_alphaTestEnabled;
u32 g_lastAlphaRef;
u32 g_lastAlphaBlend;
i32 g_renderingReflection; // original bss @0x99b360

// Per-TU VAO handle cache (original file-static @0x2a3168; GLES2 has no
// vertex array objects, only the last-bound handle is tracked).
static u32 g_lastBoundVAO;
static void NuIOSBindVAO(u32 vao) {
    if (vao != g_lastBoundVAO) {
        g_lastBoundVAO = vao;
    }
}

static i32 g_faceonProgram;      // _ZL15g_faceonProgram @0x99b440
static i32 g_faceonDecalProgram; // _ZL20g_faceonDecalProgram @0x99b450
static i32 g_debrisProgram;      // _ZL15g_debrisProgram @0x99b460
static i32 g_debrisGlassProgram; // _ZL20g_debrisGlassProgram (bss near above)

i32 g_DebrisGlassDistortTID; // _ZL23g_DebrisGlassDistortTID @0x99b4c8

static i32 NuIOSDLMtlCallback_refractionRT;                // @0x99b480
static NUNATIVETEX NuIOSDLMtlCallback_nativeRefractionTex; // @0x99b4a0
static i32 NuIOSDLMtlCallback_lastFrameCount;

extern "C" void NuShaderManagerBindShader(i32 shader);
extern "C" void NuShaderObjectGLSLSetupMaterial(i32 shader_obj, numtl_s *mtl);
extern "C" i32 NuShaderManagerGetShaderById(i32 id);

extern i32 g_currentTexUnit; // defined in nutex_ios_ex.cpp
extern NUAPI nuapi;          // nucore frame state

// The original passes the vertex-data pointer through an i32-sized argument
// slot (identical on i686); route it through an integer intermediate so the
// 64-bit clang-tidy pre-pass doesn't flag a pointer-to-smaller cast.
static inline isize PtrToArgInt(const void *p) {
    union {
        const void *p_;
        isize i_;
    } u;
    u.p_ = p;
    return u.i_;
}

// Original reads the frame counter at nuapi+0x60 directly.
static i32 NuApiFrameCount() {
    return *(i32 *)((u8 *)&nuapi + 0x60);
}

NUNATIVETEX *NuTexGetNative(i32 tex_id);
void NuTexSetTextureWithStagePS(NUNATIVETEX *tex, u32 stage);
i32 NuTexGenTexture(NUNATIVETEX *tex);
void NuIOS_CopyBackbufferToTexture(NUNATIVETEX *tex, bool depth);
extern NuVertexFormatPS *g_nuPrimVertexFormat; // defined in numtl_android.cpp
extern NuVertexFormatPS *g_nuFaceOnVertexFormat;
extern NuVertexFormatPS *g_nuDebrisVertexFormat;

// Debris-shader fixed constants (original reads them straight out of the
// render-context bss: g_renderContext_viewProj @0x119bd00,
// g_renderContext_view @0x119bdc0).
extern "C" f32 g_renderContext_viewProj[16];
extern "C" f32 g_renderContext_view[16];

// --- GL state helpers ---------------------------------------------------

// original 0x2ac110
void NuIOS_SetCullMode(i32 mode) {
    static i32 prevCullMode = 0;                           // @0x628c50
    static i32 prevReflectionMode = 0;                     // @0x628c60
    static const u32 glCullModes[2] = {GL_BACK, GL_FRONT}; // @0x57bcec

    if (mode != prevCullMode || prevReflectionMode != g_renderingReflection) {
        prevReflectionMode = g_renderingReflection;
        if (mode == 2) {
            glDisable(GL_CULL_FACE);
            prevCullMode = 2;
        } else {
            if (prevCullMode == 2) {
                glEnable(GL_CULL_FACE);
            }
            u32 idx = (u32)(mode + g_renderingReflection) & 1;
            glCullFace(glCullModes[idx]);
            prevCullMode = (i32)idx;
        }
    }
}

// original 0x2ac1c0
void NuMtlSetRenderStatesPS(numtl_s *mtl) {
    u8 *m = (u8 *)mtl;
    u8 blend;
    u8 alpha_ref_byte = (m[0x42] >> 7) & 1 ? m[0x43] : m[0x43];

    if ((m[0x1f2] & 0x10) == 0) {
        u8 alpha_sel = (m[0x42] >> 4) & 7;
        if (alpha_sel > 1) {
            if (alpha_sel == 5) {
                alpha_ref_byte = m[0x43];
                g_alphaRef = alpha_ref_byte;
                g_alphaFunc = 5;
                g_alphaTestEnabled = 1;
            } else {
                g_alphaFunc = 6;
                g_alphaTestEnabled = 1;
                g_alphaRef = 0;
                alpha_ref_byte = m[0x43];
            }
            goto have_alpha;
        }
        if (g_alphaTestEnabled != 0) {
            g_alphaTestEnabled = 0;
            alpha_ref_byte = m[0x43];
            goto have_alpha;
        }
        alpha_ref_byte = m[0x43];
    } else {
        g_alphaFunc = 6;
        g_alphaTestEnabled = 1;
        g_alphaRef = 2;
        alpha_ref_byte = m[0x43];
    }

have_alpha:
    blend = m[0x40] & 0xf;
    switch (blend) {
        case 0:
            glDisable(GL_BLEND);
            break;
        case 1:
            glEnable(GL_BLEND);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD); // 0x8006
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 1, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case 2:
            glEnable(GL_BLEND);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
            break;
        case 3:
            glEnable(GL_BLEND);
            glBlendEquationSeparate(0x800b /* GL_MAX (desktop GL enum) */, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, 1, 1);
            break;
        case 10:
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
    NuIOS_SetCullMode((m[0x41] >> 4) & 3);
}

// original 0x2a3860
extern "C" void NuRenderContextSetZFunc(i32 zfunc) {
    if (zfunc != g_renderContext_zFunc) {
        if (zfunc == 1) {
            glEnable(GL_DEPTH_TEST); // 0xb71
            glDepthMask(0);
            glDepthFunc(GL_LEQUAL); // 0x203
        } else if (zfunc < 2) {
            if (zfunc == 0) {
                glEnable(GL_DEPTH_TEST);
                glDepthMask(1);
                glDepthFunc(GL_LEQUAL);
            }
        } else if (zfunc == 2) {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(1);
        } else if (zfunc == 3) {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(0);
        }
    }
    g_renderContext_zFunc = zfunc;
}

// --- vertex attribute binding -------------------------------------------

// original 0x293841 (file-static)
static void NuIOS_BindVertexAttributesInternal(isize data, usize base_index, const u32 *fmt, u32 mask) {
    i32 loc = 0;
    u32 want = mask & fmt[0];
    u32 to_disable = g_activeAttributes & ~want;
    u32 to_enable = ~g_activeAttributes & want;
    g_activeAttributes = want;

    do {
        if ((want & 1) == 0) {
            if ((to_disable & 1) != 0) {
                glDisableVertexAttribArray(loc);
            }
        } else {
            if ((to_enable & 1) != 0) {
                glEnableVertexAttribArray(loc);
            }
            // Attribute record stride is 6 dwords starting after the leading
            // mask dword: [+1] GL type, [+2] component count, [+3] normalized
            // flag, [+5] byte offset, [+6] stride.
            glVertexAttribPointer(loc, fmt[loc * 6 + 2], fmt[loc * 6 + 1], (i8)fmt[loc * 6 + 3], fmt[loc * 6 + 6],
                                  (const void *)(data + fmt[loc * 6 + 5] + base_index * fmt[loc * 6 + 6]));
        }
        loc++;
        want >>= 1;
        to_enable >>= 1;
        to_disable >>= 1;
    } while ((to_enable != 0 || want != 0) || to_disable != 0);
}

// original 0x2939fe
void NuIOS_BindVertexAttributesImmediate(isize data, usize index) {
    NuIOSBindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    NuIOS_BindVertexAttributesInternal(data, index, (const u32 *)(usize)g_boundVertexFormat,
                                       *(const u32 *)(usize)g_boundVertexFormat);
}

// original 0x293a65
void NuIOS_BindVertexAttributesImmediateOverrideDataLayout(isize data, usize index, const u32 *fmt) {
    NuIOSBindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    NuIOS_BindVertexAttributesInternal(data, index, fmt, *(const u32 *)fmt);
}

// --- display-list callbacks ---------------------------------------------

// original 0x2ac070
extern "C" void NuIOS_SetVertexFormat(u32 fmt) {
    g_boundVertexFormat = fmt;
}

// original 0x29c480
void NuIOSDLMtlCallback(void *arg) {
    numtl_s *mtl = (numtl_s *)arg;
    LOG_WARN("[mtl] tid=%d sid=%d", mtl ? *(i16 *)((u8 *)mtl + 0x3e) : -1, mtl ? *(i16 *)((u8 *)mtl + 500) : -1);
    u8 *m = (u8 *)mtl;
    i32 shader;

    g_boundMaterial = mtl;
    shader = NuShaderManagerGetShaderById(*(i16 *)(m + 500)); // mtl->shader_id @0x1f4
    g_LastMtl = mtl;
    g_renderContext_materialInUse = mtl;
    NuIOS_SetVertexFormat(*(u32 *)(m + 700)); // mtl->vtx_fmt_id @0x2bc

    if ((m[0x1f2] & 0x10) == 0) {
        if ((m[0x1f2] & 0x20) == 0) {
            // Standard material path.
            if (shader != 0) {
                g_boundShader = 0;
                glUseProgram(0);
                g_currentShaderProgram = 0;
                NuShaderManagerBindShader(shader);
                NuIOS_SetVertexFormat(*(u32 *)(m + 700));
            }
        } else {
            // Face-on pass (billboards).
            NuShaderManagerBindShader(0);
            g_boundVertexFormat = (u32)(usize)g_nuFaceOnVertexFormat;
            i32 prog;
            if (*(char *)(m + 0x268) == '\0') {
                prog = g_faceonProgram;
                if (prog != 0) {
                    g_boundShader = *(u32 *)(usize)(prog + 8);
                } else {
                    goto faceon_noshader;
                }
            } else {
                prog = g_faceonDecalProgram;
                if (prog == 0) {
                faceon_noshader:
                    g_boundShader = 0;
                } else {
                    g_boundShader = *(u32 *)(usize)(prog + 8);
                }
            }
            glUseProgram(g_boundShader);
            g_currentShaderProgram = prog;
            NUNATIVETEX *tex = NuTexGetNative(*(i16 *)(m + 0x74));
            if (tex != NULL) {
                NuTexSetTextureWithStagePS(tex, 0);
            }
        }
    } else {
        // Debris pass. Glass debris (mtl+0x99 == -0x69) first copies the
        // backbuffer into the refraction texture, then uses the glass program.
        i32 prog;
        if (*(char *)(m + 0x99) == (char)-0x69) {
            if (NuIOSDLMtlCallback_refractionRT == 0) {
                NuIOSDLMtlCallback_refractionRT = NuTexGenTexture(&NuIOSDLMtlCallback_nativeRefractionTex);
                memset(&NuIOSDLMtlCallback_nativeRefractionTex, 0, 8);
            }
            if (NuApiFrameCount() != NuIOSDLMtlCallback_lastFrameCount) {
                NuIOS_CopyBackbufferToTexture(&NuIOSDLMtlCallback_nativeRefractionTex, true);
                NuIOSDLMtlCallback_lastFrameCount = NuApiFrameCount();
            }
            prog = g_debrisGlassProgram;
        } else {
            prog = g_debrisProgram;
        }

        g_boundVertexFormat = (u32)(usize)g_nuDebrisVertexFormat;
        NuShaderManagerBindShader(0);
        if (prog == 0) {
            g_boundShader = 0;
        } else {
            g_boundShader = *(u32 *)(usize)(prog + 8);
        }
        glUseProgram(g_boundShader);
        g_currentShaderProgram = prog;

        // Set the fixed constant block the debris shaders rely on by walking
        // the program's parameter list (i16 pairs: semantic, location|class)
        // exactly like the original loop: viewProj (semantic 0), view (0xc),
        // konst colour A (0x30) and the terminator entry (-0x8000).
        {
            extern void (*g_glConstantSetterTable[4])(u32 loc, i32 count, const void *vals);
            i32 count = *(i32 *)(usize)(prog + 0xc);
            if (count >= 1) {
                i16 *params = *(i16 **)(usize)(prog + 0x10);
                i16 *p;
                i32 i;

                p = params;
                if (p[0] != 0) {
                    i = 0;
                    do {
                        p += 2;
                        i++;
                        if (i == count)
                            break;
                    } while (p[0] != 0);
                }
                (*g_glConstantSetterTable[((const u8 *)p)[3] >> 4])(p[1] & 0xfff, 4, g_renderContext_viewProj);

                p = params;
                if (count >= 1 && p[0] != 0xc) {
                    i = 0;
                    do {
                        p += 2;
                        i++;
                        if (i == count)
                            break;
                    } while (p[0] != 0xc);
                }
                (*g_glConstantSetterTable[((const u8 *)p)[3] >> 4])(p[1] & 0xfff, 4, g_renderContext_view);

                p = params;
                if (count >= 1 && p[0] != 0x30) {
                    i = 0;
                    do {
                        p += 2;
                        i++;
                        if (i == count)
                            break;
                    } while (p[0] != 0x30);
                }
                (*g_glConstantSetterTable[((const u8 *)p)[3] >> 4])(p[1] & 0xfff, 1, (const void *)0x6463bc);

                p = params;
                if (count >= 1 && p[0] != (i16)-0x8000) {
                    i = 0;
                    do {
                        p += 2;
                        i++;
                        if (i == count)
                            break;
                    } while (p[0] != (i16)-0x8000);
                }
                (*g_glConstantSetterTable[((const u8 *)p)[3] >> 4])(p[1] & 0xfff, 1, (const void *)0x646360);
            }
        }

        if (*(char *)(m + 0x99) == (char)-0x69) {
            glActiveTexture(GL_TEXTURE0);
            g_currentTexUnit = 0;
            glBindTexture(GL_TEXTURE_2D, NuIOSDLMtlCallback_nativeRefractionTex.platform.gl_tex);
            NUNATIVETEX *distort = NuTexGetNative(g_DebrisGlassDistortTID);
            NuTexSetTextureWithStagePS(distort, 1);
        } else {
            NUNATIVETEX *tex = NuTexGetNative(*(i16 *)(m + 0x74));
            NuTexSetTextureWithStagePS(tex, 0);
        }
    }

    NuRenderContextSetZFunc(m[0x41] >> 6);
    g_renderingReflection = 0;
    NuMtlSetRenderStatesPS(mtl);
}

// original 0x293ad6
void NuIOSDLGeom2DCallback(void *arg) {
    u8 *item = (u8 *)arg;
    LOG_WARN("[g2d] cnt=%d geom tid=%d", *(i16 *)(item + 10), g_LastMtl ? *(i16 *)((u8 *)g_LastMtl + 0x3e) : -1);
    u8 *data = item + 0x10;

    if (*(i16 *)(item + 10) == 0) {
        LOG_WARN("[g2d] zero cnt bail");
        return;
    }

    i32 shader = NuShaderManagerGetShaderById(*(i16 *)((u8 *)g_LastMtl + 500));
    if (shader == 0 || *(i32 *)(usize)(shader + 0x10) == 0) {
        // TEMP probe: why the draw item bails before rendering
        LOG_WARN("[draw] bail shader=%d prog@10=%d", shader, shader ? (int)*(i32 *)(usize)(shader + 0x10) : -1);
        return;
    }

    NuIOSBindVAO(0);
    NuShaderObjectGLSLSetupMaterial(shader, g_LastMtl);

    static const u32 prim_modes[5] = {
        GL_TRIANGLES,      // 0 -> triangles
        GL_TRIANGLE_STRIP, // 1
        GL_LINES,          // 2
        GL_LINE_LOOP,      // 3
        GL_TRIANGLES,      // 4
    };
    u32 pt = *(u32 *)item;
    if (pt < 5) {
        NuIOS_BindVertexAttributesImmediateOverrideDataLayout(PtrToArgInt(data), 0, (const u32 *)g_nuPrimVertexFormat);
        glDrawArrays(prim_modes[pt], 0, *(u16 *)(item + 10));
    }
}
