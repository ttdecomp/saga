#include "host/platform/runtime.hpp"

#include <SDL3/SDL.h>

const char *HostPlatformVideoDriver() {
    return "windows";
}

ANativeWindow *HostPlatformNativeWindow(SDL_Window *window) {
    const SDL_PropertiesID properties = SDL_GetWindowProperties(window);
    return reinterpret_cast<ANativeWindow *>(
        SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
}
