#include "host/platform/runtime.hpp"

#include <SDL3/SDL.h>

#include "host/platform/input.hpp"
#include "legoapi/characters/core/players.h"

namespace {
    char *host_wasm_arguments[4];
}

const char *HostPlatformVideoDriver() {
    return "emscripten";
}

bool HostPlatformAudioOutputEnabled() {
    // SDL 3.4's Emscripten and dummy sinks both call a physical-device
    // function table from the browser thread while PROXY_TO_PTHREAD runs the
    // game in a worker. That cross-thread callback currently traps before the
    // first frame, so keep the OpenSL reconstruction active without opening a
    // browser output device.
    return false;
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
    // SDL's Web Audio backend currently calls into a physical-device
    // function table from the browser thread while the game itself runs in
    // the PROXY_TO_PTHREAD worker. Keep the reconstructed OpenSL mixer alive,
    // but use SDL's dummy sink until that cross-thread backend is safe; the
    // broken callback otherwise traps before the first frame is presented.
    host_wasm_arguments[2] = const_cast<char *>("--mute");
    host_wasm_arguments[3] = nullptr;
    *argc = 3;
    *argv = host_wasm_arguments;
}

void HostPlatformHandleInputEvent(const SDL_Event &event, i32 width, i32 height) {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        HostInputTouch(static_cast<i32>(event.button.x), static_cast<i32>(event.button.y), width, height);
    } else if (event.type == SDL_EVENT_FINGER_DOWN) {
        HostInputTouch(static_cast<i32>(event.tfinger.x * width), static_cast<i32>(event.tfinger.y * height), width,
                       height);
    } else if (event.type == SDL_EVENT_KEY_DOWN && (event.key.key == SDLK_RETURN || event.key.key == SDLK_SPACE)) {
        HostInputTap(0, GAMEPAD_START | GAMEPAD_JUMP);
    }
}

u32 HostPlatformKeyboardButtons(const bool *) {
    // Browser keydown events are edge-triggered above. Polling RETURN/SPACE as
    // held keys would turn one press into repeated menu input.
    return 0;
}
