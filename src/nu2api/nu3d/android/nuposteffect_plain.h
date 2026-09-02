// Post-effect + framebuffer state — see nuposteffect_plain.cpp.
#pragma once

#include "decomp.h"

extern i32 g_effectFlags;
extern i32 g_effectsRan;
extern i32 g_lastFrameEffect;

extern "C" bool NuPostEffectIsInitialised(u32 mask);
extern "C" void NuPostEffectReset(void);
extern "C" void NuPostEffectEnd(void);
extern "C" void NuPostEffectAddDynamicLight(void *light);

extern "C" void Nu360_dxClear(u32 clear_flags, u32 colour);
extern "C" void NuFramebufferClear(u32 clear_flags, u32 colour);
extern "C" void NuFramebufferSwapBuffers(void);
