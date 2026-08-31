#include <stddef.h>

#include "java/java.h"

static jint AttachCurrentThread(JavaVM *vm, JNIEnv **env, void *args) {
    *env = NULL;

    return JNI_OK;
}

static jint DetachCurrentThread(JavaVM *vm) {
    return JNI_OK;
}

static jint GetEnv(JavaVM *vm, void **env, jint version) {
    *env = NULL;

    return JNI_ERR;
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

jclass g_activityClass;

extern "C" {

    jint JNI_OnLoad(JavaVM *vm, void *reserved) {
        return JNI_OK;
    }

    void Java_com_tt_tech_CheckGamepadStatus_nativeSetGamePadConnected(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeCacheJNIVars(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnCreate(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnKeyDown(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnKeyUp(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnPause(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnResume(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnSensorUpdate(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnStart(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnStop(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnTouchDown(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnTouchMove(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeOnTouchUp(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetAndroidVersion(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetAssetManager(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetCaps(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetLanguage(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetManufacturer(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetModel(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetObbInfo(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetPaths(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetScreenDimesions(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeSetSurface(void) {
    }

    void Java_com_tt_tech_TTActivity_nativeUpdateGamepadAxisValues(void) {
    }

} // extern "C"
