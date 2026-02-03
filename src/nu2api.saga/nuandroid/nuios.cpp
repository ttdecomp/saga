#include "decomp.h"

#include "nu2api.saga/nuandroid/nuios.h"

#include <string.h>

int32_t g_isLowEndDevice = 0;

int32_t NuIOS_IsLowEndDevice(void) {
    return g_isLowEndDevice;
}

SAGA_NOMATCH char *NuIOS_GetDocumentsPath(void) {
    return "res/";
}

char* NuIOS_GetAppBundlePath(void) {
    static char storedAppBundlePath[4096];

    if (storedAppBundlePath[0] == '\0') {
        strcpy(storedAppBundlePath, "dummyPath");
    }

    return storedAppBundlePath;
}

uint32_t NuIOS_YieldThread(void) {
    UNIMPLEMENTED();
}
