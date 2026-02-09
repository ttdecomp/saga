#include "nu2api.saga/nucore/android/NuThread_android.h"

#include <pthread.h>

#include "java/java.h"
#include "nu2api.saga/nucore/nuthread.h"

static __thread NuThreadBase *g_currentThread;
static __thread pthread_t g_currentPthread;

NuThreadBase *NuThreadGetCurrentThread() {
    return g_currentThread;
}

NuThread::NuThread(const NuThreadCreateParameters &params) : NuThreadBase(params) {
    sched_param scheduling;
    pthread_attr_t attrs;

    if (params.use_current) {
        g_currentPthread = pthread_self();

        return;
    }

    switch (params.priority) {
        case 0:
            scheduling.sched_priority = -2;
            break;
        case 1:
            scheduling.sched_priority = -1;
            break;
        case 2:
            scheduling.sched_priority = 0;
            break;
        case 3:
            scheduling.sched_priority = 1;
            break;
        case 4:
            scheduling.sched_priority = 2;
            break;
    }

    this->is_suspended = params.is_suspended;

    pthread_attr_init(&attrs);
    pthread_attr_setschedparam(&attrs, &scheduling);

    pthread_create(&g_currentPthread, &attrs, &ThreadMain, this);

    pthread_attr_destroy(&attrs);

    SetDebugName(params.name);
}

NuThread::~NuThread() {
}

void *NuThread::ThreadMain(void *thread) {
    JNIEnv *jni_env;
    NuThread *self;

    jni_env = NULL;

    g_javaVM.GetEnv((void **)&jni_env, JNI_VERSION_1_4);
    g_javaVM.AttachCurrentThread(&jni_env, NULL);

    self = ((NuThread *)thread);
    g_currentThread = self;

    while (self->is_suspended) {
        NuThreadSleep(1);
    }

    void (*threadFn)(void *) = self->GetThreadFn();
    void *param = self->GetParam();

    (*threadFn)(param);

    delete self;

    g_javaVM.DetachCurrentThread();

    return NULL;
}

NuThread *NuThreadInitPS() {
    pthread_t current;
    int policy;
    sched_param scheduling;
    NuThreadCreateParameters params;
    NuThread *thread;

    current = pthread_self();

    policy = 0;
    pthread_getschedparam(current, &policy, &scheduling);

    params.thread_fn = NULL;
    params.fn_arg = NULL;

    params.priority = 1;

    params.name = "";

    params.stack_size = 0;

    params.is_suspended = false;

    params.cafe_core = NUTHREADCAFECORE_UNKNOWN_1;
    params.xbox360_core = NUTHREADXBOX360CORE_UNKNOWN_1;

    params.use_current = true;

    thread = new NuThread(params);
    thread->SetDebugName("Main");

    g_currentThread = thread;

    return thread;
}

void NuThreadSleep(i32 seconds) {
    timespec time;

    if (seconds < 1000) {
        time.tv_sec = 0;
    } else {
        time.tv_sec = seconds / 1000;
        seconds %= 1000;
    }

    time.tv_nsec = seconds * 1000000;

    nanosleep(&time, NULL);
}
