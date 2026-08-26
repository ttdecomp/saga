#include "nu2api/nuandroid/ios_graphics.h"

#include <GLES2/gl2.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"

i32 g_backingWidth;
i32 g_backingHeight;

void NuIOSInitOpenGLES(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 308);
    NuIOS_AllocateSystemFramebuffers();
    glFrontFace(GL_CW);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 312);
}

void NuIOS_AllocateSystemFramebuffers(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 106);
    NuCheckGLErrorsFL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 108);

    memset(g_lastBound2DTexIds, 0, sizeof(g_lastBound2DTexIds));
    memset(g_lastBoundCubeTexIds, 0, sizeof(g_lastBoundCubeTexIds));

#ifdef HOST_BUILD
    // HOST: the render thread is bound to a 1x1 pbuffer, so default 0 is 1x1
    // and the legal quad is clipped. Create a window-sized FBO+texture that
    // is shared across the share-group contexts. This is a PS technical
    // difference, not game logic. The texture is sampled by SwapBuffers for
    // presentation and by HostReadbackPixels for window.ppm.
    extern GLuint g_earlyColorTexture;
    LOG_WARN("[hostFBO] Allocate g_backing %dx%d g_earlyColorTexture %u FBO %u", g_backingWidth, g_backingHeight,
             g_earlyColorTexture, g_earlyColorFramebuffer);
    if (g_earlyColorFramebuffer == 0) {
        glGenFramebuffers(1, &g_earlyColorFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, g_earlyColorFramebuffer);
        GLuint colorTex = 0;
        glGenTextures(1, &colorTex);
        g_earlyColorTexture = colorTex;
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_backingWidth, g_backingHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
        GLuint depth = 0;
        glGenRenderbuffers(1, &depth);
        glBindRenderbuffer(GL_RENDERBUFFER, depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, g_backingWidth, g_backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
        GLenum st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (st != GL_FRAMEBUFFER_COMPLETE) {
            LOG_WARN("AllocateSystemFramebuffers: FBO incomplete 0x%x", st);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        LOG_WARN("[hostFBO] created tex %u FBO %u", g_earlyColorTexture, g_earlyColorFramebuffer);
    } else {
        LOG_WARN("[hostFBO] already have tex %u FBO %u", g_earlyColorTexture, g_earlyColorFramebuffer);
    }
    if (NuIOS_ShouldUseMSAA()) {
        glGenFramebuffers(1, &g_earlyColorMSAAFramebuffer);
    }
    g_defaultFramebuffer = 0;
    // Keep g_currentFramebuffer as the FBO for the render thread's DrawRenderScene.
    // The per-context FBO in nudlist will wrap g_earlyColorTexture directly,
    // but keeping this here documents the host PS intent.
    g_currentFramebuffer = g_earlyColorFramebuffer;
#else
    g_earlyColorFramebuffer = 0;
    if (NuIOS_ShouldUseMSAA()) {
        glGenFramebuffers(1, &g_earlyColorMSAAFramebuffer);
    }
    g_defaultFramebuffer = 0;
    g_currentFramebuffer = 0;
#endif
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 260);
}

i32 NuCheckGLErrorsFL(const char *file, i32 line) {
    return 0; // nice
}

i32 NuIOS_IsLowEndDevice(void) {
    return g_isLowEndDevice;
}

SAGA_NOMATCH char *NuIOS_GetDocumentsPath(void) {
    return "res/";
}

char *NuIOS_GetAppBundlePath(void) {
    static char storedBundlePath[4096];

    if (storedBundlePath[0] == '\0') {
        strcpy(storedBundlePath, "dummyPath");
    }

    return storedBundlePath;
}

u32 NuIOS_YieldThread(void) {
    UNIMPLEMENTED();
    return {};
}

i32 NuIOS_ShouldUseMSAA(void) {
    return 0;
}

static pthread_mutex_t g_wakeRenderMutex;
static pthread_cond_t g_wakeRenderCondition;
static pthread_mutex_t g_renderThreadDoneThreadMutex;
static pthread_cond_t g_renderThreadDoneThreadCondition;
static pthread_mutex_t g_awaitingRenderWakeMutex;
static pthread_cond_t g_awaitingRenderWakeCondition;
static i32 g_awaitingRenderWake;
static i32 g_wakeRenderThread;
static i32 g_renderThreadDoneThread;

i64 g_renderStartTime; // original @0x66c9a8

void NuIOS_InitRenderThread() {
    pthread_mutex_init(&g_wakeRenderMutex, NULL);
    pthread_cond_init(&g_wakeRenderCondition, NULL);
    pthread_mutex_init(&g_renderThreadDoneThreadMutex, NULL);
    pthread_cond_init(&g_renderThreadDoneThreadCondition, NULL);
    pthread_mutex_init(&g_awaitingRenderWakeMutex, NULL);
    pthread_cond_init(&g_awaitingRenderWakeCondition, NULL);
}

void NuIOS_WaitUntilAllowedToRender(void) {
    pthread_mutex_lock(&g_awaitingRenderWakeMutex);
    g_awaitingRenderWake = 1;
    pthread_cond_signal(&g_awaitingRenderWakeCondition);
    pthread_mutex_unlock(&g_awaitingRenderWakeMutex);
    pthread_mutex_lock(&g_wakeRenderMutex);
    while (g_wakeRenderThread == 0) {
        pthread_cond_wait(&g_wakeRenderCondition, &g_wakeRenderMutex);
    }
    g_wakeRenderThread = 0;
    pthread_mutex_lock(&g_awaitingRenderWakeMutex);
    g_awaitingRenderWake = 0;
    pthread_mutex_unlock(&g_awaitingRenderWakeMutex);
    pthread_mutex_unlock(&g_wakeRenderMutex);
}

void NuIOS_SetRenderIncomplete(void) {
    pthread_mutex_lock(&g_renderThreadDoneThreadMutex);
    g_renderThreadDoneThread = 0;
    pthread_mutex_unlock(&g_renderThreadDoneThreadMutex);
}

void NuIOS_SetRenderComplete(void) {
    pthread_mutex_lock(&g_renderThreadDoneThreadMutex);
    if (g_renderThreadDoneThread == 0) {
        g_renderThreadDoneThread = 1;
        pthread_cond_signal(&g_renderThreadDoneThreadCondition);
    }
    pthread_mutex_unlock(&g_renderThreadDoneThreadMutex);
}

// original 0xe34b0 — pace to the remainder of the 16 ms frame budget since
// NuRenderThreadStartRender, then join on the render-thread completion flag.
void NuIOS_WaitForRenderThreadCompletion(void) {
    timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 0;

    i64 elapsed = getCurrentTime() - g_renderStartTime;
    u32 remain = (u32)0x10 - (u32)elapsed;
    if ((i64)(i32)((u32)0x10 < (u32)elapsed ? -1 : 0) >= 0) {
        if ((u32)elapsed >= 0x10 || remain >= 0x11) {
            remain = 0x10;
        }
        if (!((i64)(i32)((u32)elapsed < 0x10 ? -1 : 0) < 0) && !((u32)elapsed < 0x10 && remain < 2)) {
            ts.tv_nsec = remain * 1000000;
            ts.tv_sec = 0;
            nanosleep(&ts, NULL);
        }
    }

    pthread_mutex_lock(&g_renderThreadDoneThreadMutex);
    {
        static i32 wtc = 0;
        if (wtc++ < 3)
            LOG_WARN("[rt] game waiting for render done (elapsed=%lld)", (long long)elapsed);
    }
    while (g_renderThreadDoneThread == 0) {
        pthread_cond_wait(&g_renderThreadDoneThreadCondition, &g_renderThreadDoneThreadMutex);
    }
    g_renderThreadDoneThread = 0;
    pthread_mutex_unlock(&g_renderThreadDoneThreadMutex);
}

// original 0xe3450
i64 getCurrentTime(void) {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (i64)ts.tv_sec * 1000 + (i64)ts.tv_nsec;
}

// original 0xe3590
void NuIOS_WakeRenderThread(void) {
    g_renderStartTime = getCurrentTime();

    pthread_mutex_lock(&g_wakeRenderMutex);
    if (g_wakeRenderThread == 0) {
        g_wakeRenderThread = 1;
        pthread_cond_signal(&g_wakeRenderCondition);
    }
    pthread_mutex_unlock(&g_wakeRenderMutex);
}
