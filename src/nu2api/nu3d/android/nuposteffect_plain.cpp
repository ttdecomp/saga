// Post-effect state and framebuffer clear — Android GLES2 path.
//
// This TU is the faithful subset of the original Android post-effect
// system. The full effect graph (deferred shading, bloom, DOF, motion blur
// etc.) is not required to boot to the legal screen, so the filter objects
// stay null and the dispatch becomes a no-op.  What *is*
// required is the per-frame bookkeeping that the render thread relies on
// (reset / dynamic-light feed / end) and the framebuffer clear that wraps
// glClear — both are faithfully transcribed so the control flow matches the
// original control flow and GL state.
//
// Transcribed originals:
//   Nu360_dxClear               0x317070  Nu-side glClear wrapper (DX flag bits)
//   NuFramebufferClear          0x2a2720  thin forwarder to Nu360_dxClear
//   NuFramebufferSwapBuffers    0x2a2700  no-op on Android/host (swap owned by EGL)
//   NuPostEffectReset           0x2ab8b0
//   NuPostEffectEnd             0x2ab8d0
//   NuPostEffectIsInitialised   0x2ab9c0
//   NuPostEffectAddDynamicLight 0x2abc40

#include <GLES2/gl2.h>

#include "decomp.h"
#include "nu2api/nu3d/android/nuposteffect_plain.h"
#include "nu2api/nu3d/nutex.h"

// ──────────────────────────────────────────────────────────────────────────────
// Post-effect globals
// ──────────────────────────────────────────────────────────────────────────────

// Bitmask of which post-effects were successfully initialised.  Tested in
// renderThread_processRenderScenes (nurenderthread.cpp) against the per-slot
// masks — a disabled effect has its scene slot cleared before the frame is
// dispatched:
//
//   0x04  -> scn.unknown_58   (Bloom / main filter path)
//   0x08  -> scn.unknown_ac
//   0x10  -> scn.unknown_e4   (DOF)
//   0x20  -> scn.unknown_48   (deferred shading)
//   0x40  -> scn.unknown_178
//   0x80  -> scn.unknown_188  (motion blur)
//
// The enum below names the bits as they appear in the original binary.
enum NuPostEffectFlag : i32 {
    kEffect_BloomOrMain = 0x04,
    kEffect_Slot_AC = 0x08,
    kEffect_DOF = 0x10,
    kEffect_Deferred = 0x20,
    kEffect_Slot_178 = 0x40,
    kEffect_MotionBlur = 0x80,
};

i32 g_effectFlags;     // bss — see masks above
i32 g_effectsRan;      // bss — set by the (untranscribed) effect dispatch
i32 g_lastFrameEffect; // bss — last dispatched effect id

// ── Filter objects ───────────────────────────────────────────────────────────
// On device these are heap-allocated C++ objects with a vtable.  On host they
// are never created, so every dispatch is a null-check.  The handles are kept
// as void* (rather than a typed struct) because the original stores them as
// 32-bit pointers and the host may be 64-bit — the offsets below are byte
// offsets into the 32-bit layout.

static void *s_deferredFilter; // +0x54 lightCount, +0x58 lightIds[]; vtable+0x20 = End
static void *s_mainFilter;
static void *s_motionFilter;
static void *s_motionAccumFilter;

// Byte offsets into the filter object as laid out by the original 32-bit
// compiler.  Kept as constants so the transcribed AddDynamicLight / End
// remain greppable against the original.
static constexpr usize kFilterOffset_LightCount = 0x54;
static constexpr usize kFilterOffset_Lights = 0x58;
static constexpr usize kFilterVTableOffset_End = 0x20;                       // byte offset
static constexpr usize kFilterVTableIndex_End = kFilterVTableOffset_End / 4; // vtable[8] in original 32-bit layout

// Proxy surface descriptors that the original deferred pass populates with
// the G-buffer bindings.  On host they are reset to an identity descriptor
// every frame by NuPostEffectEnd.  Layout is 12 i32 words; only words
// 0,1,8,9 are written here:
//
//   [0] = 0  (origin / reserved)
//   [1] = buffer kind: 0 colour, 1 normal, 2 velocity, 4 depth
//   [8] = 1  (enabled)
//   [9] = 0  (mip / reserved)
static i32 s_proxyColorBuffer[12];
static i32 s_proxyNormalBuffer[12];
static i32 s_proxyVelocityBuffer[12];
static i32 s_proxyDepthBuffer[12];

static constexpr i32 kProxyKind_Color = 0;
static constexpr i32 kProxyKind_Normal = 1;
static constexpr i32 kProxyKind_Velocity = 2;
static constexpr i32 kProxyKind_Depth = 4;

static inline void ResetProxyBuffer(i32 *proxy, i32 kind) {
    proxy[0] = 0;
    proxy[1] = kind;
    proxy[8] = 1;
    proxy[9] = 0;
}

static inline void FilterEnd(void *filter) {
    if (filter == nullptr) {
        return;
    }
    // Original: (*(void(**)(void))(*(i32*)filter + 0x20))();
    // i.e. vtable[8]().
    void **vtable = *reinterpret_cast<void ***>(filter);
    using EndFn = void (*)();
    auto *fn = reinterpret_cast<EndFn>(vtable[kFilterVTableIndex_End]);
    fn();
}

static inline i32 &FilterLightCount(void *filter) {
    return *reinterpret_cast<i32 *>(reinterpret_cast<u8 *>(filter) + kFilterOffset_LightCount);
}

static inline i32 *FilterLights(void *filter) {
    return reinterpret_cast<i32 *>(reinterpret_cast<u8 *>(filter) + kFilterOffset_Lights);
}

// ── Post-effect API ─────────────────────────────────────────────────────────

// original 0x2ab9c0
extern "C" bool NuPostEffectIsInitialised(u32 mask) {
    return (g_effectFlags & static_cast<i32>(mask)) != 0;
}

// original 0x2ab8b0 — per-frame reset called at the top of
// renderThread_processRenderScenes before the safe scene list is walked.
extern "C" void NuPostEffectReset(void) {
    g_effectsRan = 0;
    g_lastFrameEffect = 0;
}

// original 0x2abc40 — feeds a dynamic light handle into the deferred filter's
// light list.  The handle is an opaque light pointer; the original checks
// *(i32*)(light+0x7bc) != 0 before calling (see nurenderthread.cpp).
extern "C" void NuPostEffectAddDynamicLight(i32 light) {
    if (s_deferredFilter == nullptr) {
        return;
    }
    i32 count = FilterLightCount(s_deferredFilter);
    FilterLights(s_deferredFilter)[count] = light;
    FilterLightCount(s_deferredFilter) = count + 1;
}

// original 0x2ab8d0 — end-of-frame dispatch.  On device this invokes the
// filter vtable's End (vtable+0x20) for each allocated filter, then resets
// the G-buffer proxy descriptors.  On host the filters are null so only the
// proxy reset is observable.
extern "C" void NuPostEffectEnd(void) {
    FilterEnd(s_deferredFilter);
    FilterEnd(s_mainFilter);
    FilterEnd(s_motionFilter);
    FilterEnd(s_motionAccumFilter);

    ResetProxyBuffer(s_proxyColorBuffer, kProxyKind_Color);
    ResetProxyBuffer(s_proxyNormalBuffer, kProxyKind_Normal);
    ResetProxyBuffer(s_proxyVelocityBuffer, kProxyKind_Velocity);
    ResetProxyBuffer(s_proxyDepthBuffer, kProxyKind_Depth);
}

// ──────────────────────────────────────────────────────────────────────────────
// Framebuffer clear / swap
// ──────────────────────────────────────────────────────────────────────────────

// Cached clear colour to avoid redundant glClearColor calls — the original
// keeps this as a function-static (_ZZ13Nu360_dxClearE10lastColour).
static u32 s_lastClearColour = 0;

extern i32 g_renderContext_zFunc; // defined in nuiosdl_gl.cpp

// Nu-side clear flag bits (DX-style) as passed by the engine.  These are NOT
// GL bits — they are translated to GL_COLOR/DEPTH/STENCIL_BUFFER_BIT below.
static constexpr u32 kNuClear_Color = 0x100;
static constexpr u32 kNuClear_Depth = 0x200;
static constexpr u32 kNuClear_Stencil = 0x800;

// original 0x317070
// colour is packed ABGR8888 little-endian (R = bits 0..7, G = 8..15,
// B = 16..23, A = 24..31) — unpacked here to normalised floats for
// glClearColor.  The depth path also fixes up the host GL depth state so
// the clear is not inadvertently masked by a previous SetZFunc.
extern "C" void Nu360_dxClear(u32 clear_flags, u32 colour) {
    GLbitfield glMask = 0;

    if ((clear_flags & kNuClear_Color) != 0) {
        glMask |= GL_COLOR_BUFFER_BIT; // 0x4000
        if (colour != s_lastClearColour) {
            const float r = static_cast<float>(colour & 0xff) / 255.0f;
            const float g = static_cast<float>((colour >> 8) & 0xff) / 255.0f;
            const float b = static_cast<float>((colour >> 0x10) & 0xff) / 255.0f;
            const float a = static_cast<float>(colour >> 0x18) / 255.0f;
            glClearColor(r, g, b, a);
            s_lastClearColour = colour;
        }
    }
    if ((clear_flags & kNuClear_Depth) != 0) {
        glMask |= GL_DEPTH_BUFFER_BIT; // 0x0100
        // Ensure depth writes are enabled and depth test is off so the clear
        // actually reaches the depth buffer, matching the original's
        // g_renderContext_zFunc bookkeeping (2 == depth test disabled).
        if (g_renderContext_zFunc != 2) {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
        }
        g_renderContext_zFunc = 2;
    }
    if ((clear_flags & kNuClear_Stencil) != 0) {
        glMask |= GL_STENCIL_BUFFER_BIT; // 0x0400
    }
    glClear(glMask);
}

// original 0x2a2720 — Android forwarder; the engine calls this per scene
// when scn.clear_flags != 0 (see nurenderthread.cpp).
extern "C" void NuFramebufferClear(u32 clear_flags, u32 colour) {
    Nu360_dxClear(clear_flags, colour);
}

// original 0x2a2700 — no-op on Android/host.  The actual EGL swap is owned
// by NuRenderDeviceSwapBuffers() on the render thread (nurenderthread.cpp:
// renderThread_main).  Kept as an empty definition so the original call site
// links without ifdef.
extern "C" void NuFramebufferSwapBuffers(void) {
}
