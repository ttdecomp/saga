#include "host/platform/runtime.hpp"

#include <SDL3/SDL.h>

#include "host/platform/input.hpp"
#include "legoapi/characters/core/players.h"

namespace {
    char *host_wasm_arguments[3];
}

const char *HostPlatformVideoDriver() {
    return "emscripten";
}

bool HostPlatformAudioOutputEnabled() {
    return true;
}

ANativeWindow *HostPlatformNativeWindow(SDL_Window *) {
    return nullptr;
}

void HostPlatformPrepareArguments(i32 *argc, char ***argv) {
    if (*argc >= 2) {
        return;
    }

    host_wasm_arguments[0] = (*argv)[0];
    host_wasm_arguments[1] = const_cast<char *>("window");
    host_wasm_arguments[2] = nullptr;
    *argc = 2;
    *argv = host_wasm_arguments;
}

void HostPlatformHandleInputEvent(const SDL_Event &event, i32 width, i32 height) {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        HostInputTouch(static_cast<i32>(event.button.x), static_cast<i32>(event.button.y), width, height);
    } else if (event.type == SDL_EVENT_FINGER_DOWN) {
        HostInputTouch(static_cast<i32>(event.tfinger.x * width), static_cast<i32>(event.tfinger.y * height), width,
                       height);
    } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_RETURN) {
        HostInputTap(0, GAMEPAD_START | GAMEPAD_JUMP);
    } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE) {
        HostInputTap(0, GAMEPAD_JUMP);
    }
}

u32 HostPlatformKeyboardButtons(const bool *) {
    // Browser keydown events are edge-triggered above. Polling RETURN/SPACE as
    // held keys would turn one press into repeated menu input or jumps.
    return 0;
}
