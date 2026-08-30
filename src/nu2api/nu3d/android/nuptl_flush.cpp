// Debris renderer vertex-buffer flush — transcribed original 0x296f35.
//
// The full debris renderer (nuptl_android.c in the original) is not yet in
// the tree; its bss globals are defined here so that this function — which
// runs every frame from NuRndrSwapScreen — behaves exactly like on device.
// When the rest of nuptl_android.c lands, move the globals there.

#include <GLES2/gl2.h>

#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec4.h"
#include "legoapi/legoapi_types.h"

i32 g_UseSysMemVB; // bss
void *g_pVBData;   // bss
u32 g_DebriVB[8];  // debris GL buffer handles
void *g_DebriSysMemVB[2][64];
void *g_debrisUploadBuffer;
u32 g_VBMaxVertexCount;
u32 g_writeBufferIndex;
u32 g_readBufferIndex;
u32 g_CurrentDebriVBIndex;
u32 g_VBSize = 0x10000;
u32 g_CurrentVBVertexCount;
u32 g_FrameVertexCount;
void *g_lastPartEffect;
NUMTX NuRndr_DebrisMtx;
NUMTX *NuRndr_DebrisRotMtxPtr;
NUVEC4 NuRndr_DebrisPlane;
nunativedebrisdata_s *g_ParticleGroup;

extern "C" void NuInitDebrisRenderer(VARIPTR *buffer) {
    const u32 ideal_dynamic_vb_size = 0x10000;
    g_VBMaxVertexCount = ideal_dynamic_vb_size / 0x18;
    if (NuIOS_IsLowEndDevice() != 0) {
        g_VBMaxVertexCount >>= 1;
    }
    g_VBSize = g_VBMaxVertexCount * 0x18;

    for (i32 frame = 0; frame < 2; ++frame) {
        for (i32 index = 0; index < 64; ++index) {
            g_DebriSysMemVB[frame][index] = buffer->void_ptr;
            buffer->addr += g_VBSize;
        }
        glGenBuffers(4, &g_DebriVB[frame * 4]);
        for (i32 index = 0; index < 4; ++index) {
            glBindBuffer(GL_ARRAY_BUFFER, g_DebriVB[frame * 4 + index]);
            glBufferData(GL_ARRAY_BUFFER, g_VBSize, NULL, GL_STREAM_DRAW);
        }
    }
    g_debrisUploadBuffer = buffer->void_ptr;
    buffer->addr += g_VBSize;
    g_pVBData = g_debrisUploadBuffer;
}

extern i32 g_forceSysMemVbs; // src/globals.h

i32 NuDebrisRendererNextBuffer() {
    if (g_UseSysMemVB == 0 && g_pVBData != NULL && g_CurrentVBVertexCount != 0) {
        BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nuptl_android.c", 0xc1);
        glBindBuffer(GL_ARRAY_BUFFER, g_DebriVB[g_writeBufferIndex * 4 + g_CurrentDebriVBIndex]);
        glBufferData(GL_ARRAY_BUFFER, g_VBSize, NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, g_CurrentVBVertexCount * sizeof(debris_vertex_s), g_pVBData);
        g_pVBData = NULL;
        EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nuptl_android.c", 0xc9);
    }

    if (g_UseSysMemVB == 0) {
        if (g_CurrentDebriVBIndex + 1 < 4) {
            g_CurrentDebriVBIndex = (g_CurrentDebriVBIndex + 1) & 3;
        } else {
            g_CurrentDebriVBIndex = 0;
            g_UseSysMemVB = 1;
        }
    } else {
        if (g_CurrentDebriVBIndex + 1 >= 64) {
            return 0;
        }
        g_CurrentDebriVBIndex = (g_CurrentDebriVBIndex + 1) & 63;
    }

    g_pVBData = g_UseSysMemVB == 0 ? g_debrisUploadBuffer : g_DebriSysMemVB[g_writeBufferIndex][g_CurrentDebriVBIndex];
    g_CurrentVBVertexCount = 0;
    return 1;
}

// original 0x296f35
void NuDebrisRendererFlushBuffers(void) {
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

// Original provides only the mangled spelling (_Z28NuDebrisRendererFlushBuffersv).
