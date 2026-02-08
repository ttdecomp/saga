#include "nu2api.saga/nucore/nuapi.h"

#include "nu2api.saga/nu3d/NuRenderDevice.h"
#include "nu2api.saga/nusound/nusound.h"

i32 NuInitHardwarePS(VARIPTR *buffer_start, VARIPTR *buffer_end, i32 zero) {
    // NuIOSThreadInit();
    // NuIOS_IsLowEndDevice();
    // g_vaoLifetimeMutex = NuThreadCreateCriticalSection();
    // g_texAnimCriticalSection = NuThreadCreateCriticalSection();
    // InitializeGLMutex();
    // NuPad_Interface_InputManagerInitialise();
    // BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0xf9);
    // NuIOSMtlInit();
    // NuInitDebrisRenderer(buffer_start, buffer_end->voidptr);
    // EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0xfe);
    // NuRenderThreadCreate();
    // BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0x103);
    // NuShaderManagerInit(buffer_start, buffer_end->voidptr);
    // NuRenderContextInit();
    // EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0x108);
    // nurndr_pixel_width = g_backingWidth;
    // nurndr_pixel_height = g_backingHeight;
    NuSound3InitV(buffer_start, *buffer_end, 0, 0);

    return 0;
}

i32 NuInitHardwareParseArgsPS(i32 setup_tok, char **value) {
    return 0;
}

void NudxFw_D3DBeginCriticalSection() {
    // This is undoubtedly a __FILE__ and __LINE__ usage in the original.
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0xd3);
}

void NudxFw_D3DEndCriticalSection() {
    // This is undoubtedly a __FILE__ and __LINE__ usage in the original.
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0xd7);
}
