#pragma once

#include <stddef.h>
#include <stdint.h>

#include "decomp.h"

enum platforms_supported_e {
    DEFAULT_PLATFORM = 0,
    XBOX_PLATFORM = 1,
    GAMECUBE_PLATFORM = 2,
    PSP_PLATFORM = 3,
    PC_PLATFORM = 4,
    PS3_PLATFORM = 5,
    X360_PLATFORM = 6,
    WII_PLATFORM = 7,
    IOS_PLATFORM = 8,
    ANDROID_ATITC_PLATFORM = 9,
    ANDROID_PVRTC_PLATFORM = 10,
    ANDROID_S3TC_PLATFORM = 11,
    ANDROID_ETC1_PLATFORM = 12,
    NUM_PLATFORMS_SUPPORTED = 13
};
typedef enum platforms_supported_e PLATFORMS_SUPPORTED;

C_API_START

extern char *g_fontExtension;
extern char *g_platformName;

C_API_END

#ifdef __cplusplus

class NuPlatform {
    char *name;
    char *textureExtension;
    char *fontExtension;
    PLATFORMS_SUPPORTED currentPlatform;

  public:
    static NuPlatform *ms_instance;

    static NuPlatform *Get(void) {
        return NuPlatform::ms_instance;
    }
    static void Create(void);

    char *GetCurrentTextureExtension() const {
        return this->textureExtension;
    }
    PLATFORMS_SUPPORTED GetCurrentPlatform() const {
        return this->currentPlatform;
    }
    void SetCurrentPlatform(PLATFORMS_SUPPORTED platform);
};

#endif
