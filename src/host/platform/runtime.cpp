#include "host/platform/runtime.hpp"

#include <SDL3/SDL.h>

#include "legoapi/characters/core/players.h"

__attribute__((weak)) const char *HostPlatformVideoDriver() {
    return "dummy";
}

__attribute__((weak)) bool HostPlatformAudioOutputEnabled() {
    return true;
}

__attribute__((weak)) ANativeWindow *HostPlatformNativeWindow(SDL_Window *) {
    return nullptr;
}

__attribute__((weak)) void HostPlatformPrepareArguments(i32 *, char ***) {
}

__attribute__((weak)) void HostPlatformHandleInputEvent(const SDL_Event &, i32, i32) {
}

__attribute__((weak)) u32 HostPlatformKeyboardButtons(const bool *keyboard) {
    u32 buttons = 0;
    buttons |= keyboard[SDL_SCANCODE_RETURN] ? GAMEPAD_START | GAMEPAD_JUMP : 0;
    buttons |= keyboard[SDL_SCANCODE_SPACE] ? GAMEPAD_JUMP : 0;
    return buttons;
}
