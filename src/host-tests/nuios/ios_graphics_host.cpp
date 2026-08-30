#include "nu2api/nuandroid/ios_graphics.h"

#include <GLES2/gl2.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "decomp.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/NuRenderDevice.h"

// Host override for NuIOS_AllocateSystemFramebuffers — strong definition
// that replaces the weak target version in src/nu2api/nuandroid/ios_graphics.cpp.
// Target creates a pass-through 1x1 setup; host creates a window-sized
// FBO+texture (g_earlyColorTexture/g_earlyColorFramebuffer) so the render
// thread draws into shared texture that SwapBuffers presents and
// HostReadbackPixels captures for window.ppm.  Linked only for BUILD_FOR_HOST.

void NuIOS_AllocateSystemFramebuffers(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 106);
    NuCheckGLErrorsFL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 108);

    memset(g_lastBound2DTexIds, 0, sizeof(g_lastBound2DTexIds));
    memset(g_lastBoundCubeTexIds, 0, sizeof(g_lastBoundCubeTexIds));

    extern GLuint g_earlyColorTexture;
    LOG_WARN("[hostFBO] Allocate g_backing %dx%d g_earlyColorTexture %u FBO %u", g_backingWidth, g_backingHeight,
             g_earlyColorTexture, g_earlyColorFramebuffer);

    if (g_earlyColorFramebuffer == 0) {
        GLuint fbo = 0;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        GLuint colorTex = 0;
        glGenTextures(1, &colorTex);
        g_earlyColorTexture = colorTex;

        glBindTexture(GL_TEXTURE_2D, colorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_backingWidth, g_backingHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

        GLuint depthRb = 0;
        glGenRenderbuffers(1, &depthRb);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRb);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, g_backingWidth, g_backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRb);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            LOG_WARN("AllocateSystemFramebuffers: FBO incomplete 0x%x", status);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        g_earlyColorFramebuffer = fbo;
        LOG_WARN("[hostFBO] created tex %u FBO %u", g_earlyColorTexture, g_earlyColorFramebuffer);
    } else {
        LOG_WARN("[hostFBO] already have tex %u FBO %u", g_earlyColorTexture, g_earlyColorFramebuffer);
    }

    if (NuIOS_ShouldUseMSAA()) {
        glGenFramebuffers(1, &g_earlyColorMSAAFramebuffer);
    }

    g_defaultFramebuffer = 0;
    g_currentFramebuffer = g_earlyColorFramebuffer;

    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 260);
}

u32 NuIOS_YieldThread(void) {
    return sched_yield();
}

static char g_hostDocumentsPath[256] = ".work/host-documents/";

void HostSetDocumentsPath(const char *path) {
    snprintf(g_hostDocumentsPath, sizeof(g_hostDocumentsPath), "%s", path);
}

char *NuIOS_GetDocumentsPath(void) {
    // Keep host test saves isolated from extracted resources and user saves.
    return g_hostDocumentsPath;
}
