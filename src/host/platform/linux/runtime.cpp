#include "host/platform/runtime.hpp"

#include <SDL3/SDL.h>

const char *HostPlatformVideoDriver() {
    return "x11";
}

ANativeWindow *HostPlatformNativeWindow(SDL_Window *window) {
    const SDL_PropertiesID properties = SDL_GetWindowProperties(window);
    const i32 handle = static_cast<i32>(SDL_GetNumberProperty(properties, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
    return reinterpret_cast<ANativeWindow *>(handle);
}
