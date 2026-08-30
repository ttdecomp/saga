#include "host-tests/input/host_input.h"

#include <atomic>

#include "decomp.h"
#include "gameframework/saveload.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nupad.h"

extern GAMEPAD_s GamePad[64];

namespace {
    std::atomic<u32> pending_buttons[2];
    std::atomic<u32> held_buttons[2];
    u32 previous_held_buttons[2];
} // namespace

void HostInputReset() {
    enable_touch_controls = 0;
    for (i32 port = 0; port < 2; ++port) {
        pending_buttons[port].store(0, std::memory_order_relaxed);
        held_buttons[port].store(0, std::memory_order_relaxed);
        previous_held_buttons[port] = 0;
        g_nupadMapping[port].pad = port == 0 ? 0 : -1;
        g_nupadMapping[port].port = port == 0 ? 0 : -1;
        g_nupadMapping[port].is_active = port == 0 ? 1 : 0;
    }
    saveload_slotid = -1;
}

void HostInputSetHeld(i32 port, u32 buttons) {
    if (port < 0 || port >= 2) {
        return;
    }

    LOG_INFO("host input: port %d held buttons=0x%x", port, buttons);
    held_buttons[port].store(buttons, std::memory_order_release);
}

void HostInputTap(i32 port, u32 buttons) {
    if (port < 0 || port >= 2) {
        return;
    }

    LOG_INFO("host input: port %d tapped buttons=0x%x", port, buttons);
    pending_buttons[port].fetch_or(buttons, std::memory_order_release);
}

i32 ReadPad(i32 port) {
    if (port < 0 || port >= 2) {
        return 0;
    }

    GAMEPAD_s &gamepad = GamePad[port];
    if (port == 0) {
        // The host bypasses the platform NuPad scanner. Keep the logical
        // keyboard-backed controller mapped exactly like a connected pad;
        // game-side player activation consumes both this mapping and the
        // per-frame input state written by the original ReadPad.
        g_nupadMapping[0].pad = 0;
        g_nupadMapping[0].port = 0;
        g_nupadMapping[0].is_active = 1;
        gamepad.input_state = 1;
        if (gamepad.pad != nullptr) {
            gamepad.pad->is_valid = true;
        }
    }
    const u32 tapped = pending_buttons[port].exchange(0, std::memory_order_acq_rel);
    const u32 held = held_buttons[port].load(std::memory_order_acquire);
    const u32 buttons = tapped | held;

    gamepad.unknown_04 = buttons;
    gamepad.buttons_down_08 = tapped | (held & ~previous_held_buttons[port]);
    gamepad.unknown_0c = previous_held_buttons[port] & ~held;
    gamepad.unknown_10 = 0;
    gamepad.unknown_14 = 0;
    gamepad.unknown_18 = 0;
    gamepad.unknown_1c = 0;
    previous_held_buttons[port] = held;

    return 2;
}

void DoInput(WORLDINFO_s *) {
    ReadPads();
}
