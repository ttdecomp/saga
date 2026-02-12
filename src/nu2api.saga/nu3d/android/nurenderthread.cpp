#include "nu2api.saga/nu3d/android/nurenderthread.h"

#include <pthread.h>

#include "decomp.h"
#include "nu2api.saga/nu3d/NuRenderDevice.h"
#include "nu2api.saga/nuandroid/nuios.h"
#include "nu2api.saga/nucore/nuthread.h"

static volatile i32 renderThreadCS;
static i32 renderThreadIsLocked;
static pthread_t *g_renderThread;

void NuRenderThreadLock(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nurenderthread.cpp", 134);
    NuThreadCriticalSectionBegin(renderThreadCS);
    renderThreadIsLocked++;
}

void NuRenderThreadUnlock(void) {
    renderThreadIsLocked--;
    NuThreadCriticalSectionEnd(renderThreadCS);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nurenderthread.cpp", 153);
}

i32 NuRenderThreadIsLocked(void) {
    return renderThreadIsLocked;
}

void NuRenderThreadCreate(void) {
    NuIOS_InitRenderThread();
    pthread_create(g_renderThread, NULL, renderThread_main, NULL);
    renderThreadCS = NuThreadCreateCriticalSection();
}

void *renderThread_main(void *arg) {
    UNIMPLEMENTED();
}
