#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/android/nugscn_android.h"
#include "nu2api/nu3d/android/nurenderthread.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutexanm.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nusound/nusound.h"

extern "C" void NuRenderContextInit(void);
extern "C" void NuInitDebrisRenderer(VARIPTR *buffer);
extern "C" void NuIOSMtlInit(void);

void InitializeGLMutex(void) {
}

i32 NuInitHardwarePS(VARIPTR *buf, VARIPTR *buf_end, i32 heap_size) {
    NuIOSThreadInit();

    NuIOS_IsLowEndDevice();

    g_vaoLifetimeMutex = NuThreadCreateCriticalSection();
    g_texAnimCriticalSection = NuThreadCreateCriticalSection();
    InitializeGLMutex();

    NuPad_Interface_InputManagerInitialise();

    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0xf9);
    NuIOSMtlInit();
    NuInitDebrisRenderer(buf);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0xfe);

    NuRenderThreadCreate();

    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0x103);
    NuShaderManagerInit(buf, *buf_end);
    NuRenderContextInit();
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0x108);

    nurndr_pixel_width = g_backingWidth;
    nurndr_pixel_height = g_backingHeight;

    NuSound3InitV(buf, *buf_end, 0, 0);

    return 0;
}

i32 NuInitHardwareParseArgsPS(i32 setup_tok, char **args) {
    return 0;
}

void NudxFw_D3DBeginCriticalSection() {
    // This is undoubtedly a __FILE__ and __LINE__ usage in the original.
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0xd3);
}

void NudxFw_D3DEndCriticalSection() {
    // This is undoubtedly a __FILE__ and __LINE__ usage in the original.
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nucore/android/nuapi_android.c", 0xd7);
}
