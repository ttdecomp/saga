#include <EGL/egl.h>
#include <SDL3/SDL.h>

#include <stdlib.h>

#include "decomp.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legogame/startup.h"
#include "legogame/target.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nuplatform/nudevicespecs.hpp"
#include "nu2api/nuplatform/nuplatform.h"

const char VIDEO_DRIVER[] =
#ifdef _WIN32
    "windows"
#else
    "x11"
#endif
    ;

void host_init() {
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, VIDEO_DRIVER);
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        LOG_ERR("SDL_Init failed: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow("title", 1280, 720, 0);
    if (!window) {
        LOG_ERR("SDL_CreateWindow failed: %s", SDL_GetError());
        exit(1);
    }

    if (strcmp(SDL_GetCurrentVideoDriver(), VIDEO_DRIVER) != 0) {
        LOG_ERR("Unexpected video driver: %s", SDL_GetCurrentVideoDriver());
        exit(1);
    }

    SDL_PropertiesID prop_id = SDL_GetWindowProperties(window);

#ifdef _WIN32
    HWND handle = (HWND)SDL_GetPointerProperty(prop_id, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#else
    i32 handle = SDL_GetNumberProperty(prop_id, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
#endif

    g_renderDevice.OnWindowCreated((ANativeWindow *)handle);
}

int main(int argc, char **argv) {
    host_init();

    NuPlatform::Create();
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);

    if (!NuScreen::Exists()) {
        NuScreen::Create();
    }

#ifdef HOST_BUILD
    host_init();
#endif

    g_renderDevice.Initialize();

    g_isLowestEndDevice = 1;
    g_isLowEndDevice = 1;
    g_isMidRangeDevice = 0;
    i32 specs = NuDeviceSpecs::ms_instance->specs;
    if (specs == 1) {
        g_isLowestEndDevice = 0;
        g_isLowEndDevice = 0;
        g_isMidRangeDevice = 1;
        g_lowEndLevelBehaviour = 0;
    } else if (specs < 1 || 3 < specs) {
        g_lowEndLevelBehaviour = 1;
    } else {
        g_isLowestEndDevice = 0;
        g_isLowEndDevice = 0;
        g_lowEndLevelBehaviour = 0;
    }

    InitOnce(argc, argv);
    TriggerExtraDataLoad();

    StartPerm();
    LoadPerm();
    EndPerm();

    return 0;
}
