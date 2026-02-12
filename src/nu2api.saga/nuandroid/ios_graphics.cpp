#include "nu2api.saga/nuandroid/ios_graphics.h"

#include <GLES2/gl2.h>
#include <pthread.h>
#include <string.h>

#include "decomp.h"
#include "nu2api.saga/nu3d/NuRenderDevice.h"

void NuIOSInitOpenGLES(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 308);
    NuIOS_AllocateSystemFramebuffers();
    glFrontFace(GL_CW);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 312);
}

GLuint g_lastBound2DTexIds[16];
GLuint g_lastBoundCubeTexIds[16];
GLuint g_earlyColorFramebuffer;
GLuint g_defaultFramebuffer;
GLuint g_currentFramebuffer;
GLuint g_earlyColorMSAAFramebuffer;

void NuIOS_AllocateSystemFramebuffers(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 106);
    NuCheckGLErrorsFL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 108);

    memset(g_lastBound2DTexIds, 0, sizeof(g_lastBound2DTexIds));
    memset(g_lastBoundCubeTexIds, 0, sizeof(g_lastBoundCubeTexIds));

    g_earlyColorFramebuffer = 0;
    if (NuIOS_ShouldUseMSAA()) {
        glGenFramebuffers(1, &g_earlyColorMSAAFramebuffer);
    }
    g_defaultFramebuffer = 0;
    g_currentFramebuffer = 0;
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 260);
}

i32 NuCheckGLErrorsFL(char *file, i32 line) {
    return 0; // nice
}

i32 g_isLowEndDevice = 0;

i32 NuIOS_IsLowEndDevice(void) {
    return g_isLowEndDevice;
}

SAGA_NOMATCH char *NuIOS_GetDocumentsPath(void) {
    return "res/";
}

char *NuIOS_GetAppBundlePath(void) {
    static char storedAppBundlePath[4096];

    if (storedAppBundlePath[0] == '\0') {
        strcpy(storedAppBundlePath, "dummyPath");
    }

    return storedAppBundlePath;
}

u32 NuIOS_YieldThread(void) {
    UNIMPLEMENTED();
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
static pthread_t *g_wakeRenderThread;
static pthread_t *g_renderThreadDoneThread;

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
    while (g_wakeRenderThread == NULL) {
        pthread_cond_wait(&g_wakeRenderCondition, &g_wakeRenderMutex);
    }
    g_wakeRenderThread = NULL;
    pthread_mutex_lock(&g_awaitingRenderWakeMutex);
    g_awaitingRenderWake = 0;
    pthread_mutex_unlock(&g_awaitingRenderWakeMutex);
    pthread_mutex_unlock(&g_wakeRenderMutex);
}

void NuIOS_SetRenderIncomplete(void) {
    pthread_mutex_lock(&g_renderThreadDoneThreadMutex);
    g_renderThreadDoneThread = NULL;
    pthread_mutex_unlock(&g_renderThreadDoneThreadMutex);
}
