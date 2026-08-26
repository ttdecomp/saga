// Post-effect + framebuffer state — see nuposteffect_plain.cpp.
#pragma once

#include "decomp.h"

extern i32 g_effectFlags;
extern i32 g_effectsRan;
extern i32 g_lastFrameEffect;

bool NuPostEffectIsInitialised(u32 mask);
void NuPostEffectReset(void);
void NuPostEffectEnd(void);
void NuPostEffectAddDynamicLight(i32 light);

extern "C" void Nu360_dxClear(u32 clear_flags, u32 colour);
void NuFramebufferClear(u32 clear_flags, u32 colour);
void NuFramebufferSwapBuffers(void);
