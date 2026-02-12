#include "decomp.h"

#include "nu2api.saga/nuandroid/nuios.h"

#include <pthread.h>
#include <string.h>

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

static pthread_mutex_t g_wakeRenderMutex;
static pthread_cond_t g_wakeRenderCondition;
static pthread_mutex_t g_renderThreadDoneThreadMutex;
static pthread_cond_t g_renderThreadDoneThreadCondition;
static pthread_mutex_t g_awaitingRenderWakeMutex;
static pthread_cond_t g_awaitingRenderWakeCondition;

void NuIOS_InitRenderThread() {
    pthread_mutex_init(&g_wakeRenderMutex, NULL);
    pthread_cond_init(&g_wakeRenderCondition, NULL);
    pthread_mutex_init(&g_renderThreadDoneThreadMutex, NULL);
    pthread_cond_init(&g_renderThreadDoneThreadCondition, NULL);
    pthread_mutex_init(&g_awaitingRenderWakeMutex, NULL);
    pthread_cond_init(&g_awaitingRenderWakeCondition, NULL);
}

i32 NuIOS_ShouldUseMSAA(void) {
    return 0;
}
