#include "nu2api/nuandroid/ios_graphics.h"

#include <GLES2/gl2.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"

// ---------------------------------------------------------------------------
// ios_graphics — iOS/Android GLES2 platform glue
//
// Original iOS path managed its own EGL / framebuffer and a dedicated render
// thread.  On Android the same code is reused: the "iOS" name is historical.
// The host (desktop) build repurposes the path to render into a window-sized
// FBO+texture so the pbuffer-backed GL context can present via SwapBuffers
// and dump window.ppm readbacks.  Otherwise this file is purely platform
// plumbing — thread handoff, frame pacing, and a few path shims.
// ---------------------------------------------------------------------------

// Backing drawable size driven by NuRenderDevice::InitialiseOpenGLContext.
i32 g_backingWidth;
i32 g_backingHeight;

// ---------------------------------------------------------------------------
// Framebuffer setup
// ---------------------------------------------------------------------------

void NuIOSInitOpenGLES(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 308);
    NuIOS_AllocateSystemFramebuffers();
    glFrontFace(GL_CW);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 312);
}

void NuIOS_AllocateSystemFramebuffers(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 106);
    NuCheckGLErrorsFL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 108);

    // Texture-cache shadow state — driver may have been torn down.
    memset(g_lastBound2DTexIds, 0, sizeof(g_lastBound2DTexIds));
    memset(g_lastBoundCubeTexIds, 0, sizeof(g_lastBoundCubeTexIds));

#ifdef HOST_BUILD
    // Host: the render thread is bound to a 1×1 pbuffer, so the default
    // framebuffer 0 is useless for drawing.  Create a window-sized FBO +
    // color texture shared across the share-group.  The render thread draws
    // into it, SwapBuffers samples it for presentation, and
    // HostReadbackPixels reads it back for window.ppm.
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
    // Render thread's DrawRenderScene targets this FBO.  The per-context FBO
    // in nudlist wraps g_earlyColorTexture directly; keeping the assignment
    // here documents the host intent.
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

// ---------------------------------------------------------------------------
// Diagnostics / caps
// ---------------------------------------------------------------------------

i32 NuCheckGLErrorsFL(const char *file, i32 line) {
    (void)file;
    (void)line;
    return 0;
}

i32 NuIOS_IsLowEndDevice(void) {
    return g_isLowEndDevice;
}

i32 NuIOS_ShouldUseMSAA(void) {
    return 0;
}

// ---------------------------------------------------------------------------
// Filesystem shims — original iOS used NSBundle / Documents; on Android and
// host everything is relative to the APK / working directory.
// ---------------------------------------------------------------------------

SAGA_NOMATCH char *NuIOS_GetDocumentsPath(void) {
    return "res/";
}

char *NuIOS_GetAppBundlePath(void) {
    static char s_bundlePath[4096];

    if (s_bundlePath[0] == '\0') {
        strcpy(s_bundlePath, "dummyPath");
    }
    return s_bundlePath;
}

u32 NuIOS_YieldThread(void) {
    UNIMPLEMENTED();
    return 0;
}

// ---------------------------------------------------------------------------
// Render-thread handoff
//
// Game thread  ──Wakes──► render thread (via g_wakeRenderThread)
// Render thread ──Signals──► game thread "allowed to render" observability
// Render thread ──Signals──► game thread "frame done" (g_renderThreadDoneThread)
//
// Three independent mutex/cond pairs mirror the original iOS implementation.
// The split keeps the "awaiting wake" debug flag independent from the actual
// wake/completion signals.
// ---------------------------------------------------------------------------

static pthread_mutex_t g_wakeRenderMutex;
static pthread_cond_t g_wakeRenderCondition;
static pthread_mutex_t g_renderThreadDoneThreadMutex;
static pthread_cond_t g_renderThreadDoneThreadCondition;
static pthread_mutex_t g_awaitingRenderWakeMutex;
static pthread_cond_t g_awaitingRenderWakeCondition;
static i32 g_awaitingRenderWake;
static i32 g_wakeRenderThread;
static i32 g_renderThreadDoneThread;

// Timestamp (ms) when the current frame's render was kicked — original
// @0x66c9a8, set in NuIOS_WakeRenderThread and sampled in
// NuIOS_WaitForRenderThreadCompletion for 60 Hz pacing.
i64 g_renderStartTime;

// original 0xe3450 — monotonic millisecond clock.
i64 getCurrentTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (i64)ts.tv_sec * 1000 + (i64)ts.tv_nsec / 1000000;
}

void NuIOS_InitRenderThread() {
    pthread_mutex_init(&g_wakeRenderMutex, nullptr);
    pthread_cond_init(&g_wakeRenderCondition, nullptr);
    pthread_mutex_init(&g_renderThreadDoneThreadMutex, nullptr);
    pthread_cond_init(&g_renderThreadDoneThreadCondition, nullptr);
    pthread_mutex_init(&g_awaitingRenderWakeMutex, nullptr);
    pthread_cond_init(&g_awaitingRenderWakeCondition, nullptr);
}

// Render thread: block until the game thread wakes us for the next frame.
void NuIOS_WaitUntilAllowedToRender(void) {
    // Observability: tell the game thread we are now parked and awaiting
    // the wake signal (used by debug overlays / asserts on iOS).
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

// original 0xe34b0 — pace the remainder of the 16 ms frame budget since
// g_renderStartTime was stamped in NuIOS_WakeRenderThread, then join on the
// render-thread completion flag.  Sleeps < 2 ms are skipped to avoid
// nanosleep overhead dominating the frame.
void NuIOS_WaitForRenderThreadCompletion(void) {
    constexpr i64 kFrameBudgetMs = 16;
    constexpr i64 kMinSleepMs = 2;

    i64 elapsed = getCurrentTime() - g_renderStartTime;

    if (elapsed < kFrameBudgetMs) {
        i64 remainMs = kFrameBudgetMs - elapsed;
        // Clamp to a sane range and avoid tiny sleeps that cost more than
        // they save.  The decompiled original had extensive unsigned-wrap
        // guards here; the intent is simply "sleep the remainder, if
        // worthwhile".
        if (remainMs >= kMinSleepMs && remainMs < 32) {
            struct timespec ts = {};
            ts.tv_sec = 0;
            ts.tv_nsec = remainMs * 1000000L;
            nanosleep(&ts, nullptr);
        }
    }

    pthread_mutex_lock(&g_renderThreadDoneThreadMutex);
    {
        // Throttled log — first few frames only, to confirm the handoff
        // is alive without spamming logcat.
        static i32 s_waitLogCount = 0;
        if (s_waitLogCount++ < 3) {
            LOG_WARN("[rt] game waiting for render done (elapsed=%lld)", (long long)elapsed);
        }
    }
    while (g_renderThreadDoneThread == 0) {
        pthread_cond_wait(&g_renderThreadDoneThreadCondition, &g_renderThreadDoneThreadMutex);
    }
    g_renderThreadDoneThread = 0;
    pthread_mutex_unlock(&g_renderThreadDoneThreadMutex);
}

// original 0xe3590 — kick the render thread for the next frame.
void NuIOS_WakeRenderThread(void) {
    g_renderStartTime = getCurrentTime();

    pthread_mutex_lock(&g_wakeRenderMutex);
    if (g_wakeRenderThread == 0) {
        g_wakeRenderThread = 1;
        pthread_cond_signal(&g_wakeRenderCondition);
    }
    pthread_mutex_unlock(&g_wakeRenderMutex);
}
