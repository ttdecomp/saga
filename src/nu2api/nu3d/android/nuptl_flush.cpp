// Debris renderer vertex-buffer flush — transcribed original 0x296f35.
//
// The full debris renderer (nuptl_android.c in the original) is not yet in
// the tree; its bss globals are defined here so that this function — which
// runs every frame from NuRndrSwapScreen — behaves exactly like on device.
// When the rest of nuptl_android.c lands, move the globals there.

#include <GLES2/gl2.h>

#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"

i32 g_UseSysMemVB; // bss
void *g_pVBData;   // bss
u32 g_DebriVB[8];  // debris GL buffer handles
u32 g_writeBufferIndex;
u32 g_readBufferIndex;
u32 g_CurrentDebriVBIndex;
u32 g_VBSize = 0x10000;
u32 g_CurrentVBVertexCount;
u32 g_FrameVertexCount;
void *g_lastPartEffect;

extern i32 g_forceSysMemVbs; // src/globals.h

// original 0x296f35
extern "C" void NuDebrisRendererFlushBuffers(void) {
    if ((g_UseSysMemVB == 0) && (g_pVBData != NULL) && (g_CurrentVBVertexCount != 0)) {
        BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nuptl_android.c", 0xa4);
        glBindBuffer(GL_ARRAY_BUFFER, g_DebriVB[(g_writeBufferIndex * 4 + g_CurrentDebriVBIndex) % 8]);
        glBufferData(GL_ARRAY_BUFFER, g_VBSize, NULL, GL_DYNAMIC_DRAW); // 0x88e0
        glBufferSubData(GL_ARRAY_BUFFER, 0, g_CurrentVBVertexCount * 0x18, g_pVBData);
        EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nuptl_android.c", 0xab);
    }
    g_pVBData = NULL;
    g_CurrentDebriVBIndex = 0;
    g_UseSysMemVB = (g_forceSysMemVbs != 0);
    g_CurrentVBVertexCount = 0;
    g_lastPartEffect = NULL;
    g_FrameVertexCount = 0;
    g_readBufferIndex = g_writeBufferIndex;
    g_writeBufferIndex = (g_writeBufferIndex + 1) & 1;
}

// The original library exported this routine under both the unmangled name
// and the C++-mangled one (_Z28NuDebrisRendererFlushBuffersv); keep both
// export spellings alive with an assembler alias.
asm(".globl _Z28NuDebrisRendererFlushBuffersv\n"
    ".set _Z28NuDebrisRendererFlushBuffersv, NuDebrisRendererFlushBuffers\n");
