#include <EGL/egl.h>
#include <SDL3/SDL.h>

#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"

void host_init() {
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LOG_ERR("SDL_Init failed: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow("title", 1280, 720, 0);
    if (!window) {
        LOG_ERR("SDL_CreateWindow failed: %s", SDL_GetError());
        exit(1);
    }

    if (strcmp(SDL_GetCurrentVideoDriver(), "x11") != 0) {
        LOG_ERR("Unexpected video driver: %s", SDL_GetCurrentVideoDriver());
        exit(1);
    }

    SDL_PropertiesID prop_id = SDL_GetWindowProperties(window);
    i32 xwindow = SDL_GetNumberProperty(prop_id, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);

    g_renderDevice.OnWindowCreated((ANativeWindow *)xwindow);
}
