// Post-effect state and framebuffer clear/swap — transcribed originals.
//
//   Nu360_dxClear              original 0x317070
//   NuFramebufferClear         original 0x2a2720
//   NuFramebufferSwapBuffers   original 0x2a2700
//   NuPostEffectReset          original 0x2ab8b0
//   NuPostEffectEnd            original 0x2ab8d0
//   NuPostEffectIsInitialised  original 0x2ab9c0
//   NuPostEffectAddDynamicLight original 0x2abc40

#include <GLES2/gl2.h>

#include "decomp.h"
#include "nu2api/nu3d/android/nuposteffect_plain.h"
#include "nu2api/nu3d/nutex.h"

// --- post effect globals -------------------------------------------------

i32 g_effectFlags; // bss, tested against masks 4/8/0x10/0x20/0x40/0x80
i32 g_effectsRan;
i32 g_lastFrameEffect;

static i32 deferredFilter; // posteffect filter object pointers (vtable'd)
static i32 mainFilter;
static i32 motionFilter;
static i32 motionAccumFilter;
static i32 proxyColorBuffer[12];
static i32 proxyNormalBuffer[12];
static i32 proxyVelocityBuffer[12];
static i32 proxyDepthBuffer[12];

bool NuPostEffectIsInitialised(u32 mask) {
    return (g_effectFlags & mask) != 0;
}

void NuPostEffectReset(void) {
    g_effectsRan = 0;
    g_lastFrameEffect = 0;
}

void NuPostEffectAddDynamicLight(i32 light) {
    i32 count;

    if (deferredFilter != 0) {
        count = *(i32 *)(usize)(deferredFilter + 0x54);
        *(i32 *)(usize)(deferredFilter + 0x58 + count * 4) = light;
        *(i32 *)(usize)(deferredFilter + 0x54) = count + 1;
    }
}

void NuPostEffectEnd(void) {
    if (deferredFilter != 0) {
        (*(void (**)(void))(usize)(*(i32 *)(usize)deferredFilter + 0x20))();
    }
    if (mainFilter != 0) {
        (*(void (**)(void))(usize)(*(i32 *)(usize)mainFilter + 0x20))();
    }
    if (motionFilter != 0) {
        (*(void (**)(void))(usize)(*(i32 *)(usize)motionFilter + 0x20))();
    }
    if (motionAccumFilter != 0) {
        (*(void (**)(void))(usize)(*(i32 *)(usize)motionAccumFilter + 0x20))();
    }
    proxyColorBuffer[0] = 0;
    proxyColorBuffer[1] = 0;
    proxyColorBuffer[8] = 1;
    proxyColorBuffer[9] = 0;
    proxyNormalBuffer[0] = 0;
    proxyNormalBuffer[1] = 1;
    proxyNormalBuffer[8] = 1;
    proxyNormalBuffer[9] = 0;
    proxyVelocityBuffer[0] = 0;
    proxyVelocityBuffer[1] = 2;
    proxyVelocityBuffer[8] = 1;
    proxyVelocityBuffer[9] = 0;
    proxyDepthBuffer[0] = 0;
    proxyDepthBuffer[1] = 4;
    proxyDepthBuffer[8] = 1;
    proxyDepthBuffer[9] = 0;
}

// --- framebuffer clear / swap --------------------------------------------

static u32 Nu360_dxClear_lastColour; // _ZZ13Nu360_dxClearE10lastColour

extern i32 g_renderContext_zFunc; // defined in nuiosdl_gl.cpp

// original 0x317070
extern "C" void Nu360_dxClear(u32 clear_flags, u32 colour) {
    u32 mask = 0;

    if ((clear_flags & 0x100) != 0) {
        mask = 0x4000; // GL_COLOR_BUFFER_BIT
        if (colour != Nu360_dxClear_lastColour) {
            glClearColor((float)(colour & 0xff) / 255.0f, (float)((colour >> 8) & 0xff) / 255.0f,
                         (float)((colour >> 0x10) & 0xff) / 255.0f, (float)(colour >> 0x18) / 255.0f);
            Nu360_dxClear_lastColour = colour;
        }
    }
    if ((clear_flags & 0x200) != 0) {
        mask |= 0x100; // GL_DEPTH_BUFFER_BIT
        if (g_renderContext_zFunc != 2) {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(1);
        }
        g_renderContext_zFunc = 2;
    }
    if ((clear_flags & 0x800) != 0) {
        mask |= 0x400; // GL_STENCIL_BUFFER_BIT
    }
    glClear(mask);
}

// original 0x2a2720
void NuFramebufferClear(u32 clear_flags, u32 colour) {
    Nu360_dxClear(clear_flags, colour);
}

// original 0x2a2700
void NuFramebufferSwapBuffers(void) {
}
