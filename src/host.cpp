#include <EGL/egl.h>
#include <SDL3/SDL.h>

#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"

const char VIDEO_DRIVER[] =
#ifdef _WIN32
    "windows"
#else
    "x11"
#endif
    ;

void host_init() {
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, VIDEO_DRIVER);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
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
    LOG_INFO("HOST BUILD");

    g_renderDevice.OnWindowCreated((ANativeWindow *)handle);
}
