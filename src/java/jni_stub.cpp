#include <stddef.h>

#include "java/java.h"

jint AttachCurrentThread(JavaVM *vm, JNIEnv **env, void *args) {
    *env = NULL;

    return JNI_OK;
}

jint DetachCurrentThread(JavaVM *vm) {
    return JNI_OK;
}

jint GetEnv(JavaVM *vm, void **env, jint version) {
    *env = NULL;

    return JNI_OK;
}

struct JNIInvokeInterface stub = {
    .reserved0 = NULL,
    .reserved1 = NULL,
    .reserved2 = NULL,

    .DestroyJavaVM = NULL,
    .AttachCurrentThread = &AttachCurrentThread,
    .DetachCurrentThread = &DetachCurrentThread,
    .GetEnv = &GetEnv,
    .AttachCurrentThreadAsDaemon = NULL,
};

JavaVM g_javaVM = {
    .functions = &stub,
};
