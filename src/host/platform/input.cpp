#include "host/platform/input.hpp"

#include <atomic>
#include <cstring>

#include "nu2api/nucore/NuInputDevice.h"
#ifdef __EMSCRIPTEN__
#include "MechInputTouch/MechInputTouch_types.h"
#include "gameapi/gui/apimenu.h"
#include "legoapi/characters/core/players.h"
#include "nu2api/numath/nufloat.h"

extern i32 GetMenuID();
#endif

namespace {
    constexpr u32 host_touch_device = 0;
    constexpr u32 host_gamepad_device = 1;
    std::atomic<u32> host_pending_buttons[2];
    std::atomic<u32> host_held_buttons[2];
    std::atomic<u32> host_keyboard_buttons[2];
    u32 host_frame_buttons[2];
#ifdef __EMSCRIPTEN__
    std::atomic<i32> host_touch_x;
    std::atomic<i32> host_touch_y;
    std::atomic<i32> host_touch_width;
    std::atomic<i32> host_touch_height;
    std::atomic<bool> host_touch_pending;

    u32 host_consume_touch() {
        if (!host_touch_pending.exchange(false, std::memory_order_acquire)) {
            return 0;
        }

        const i32 width = host_touch_width.load(std::memory_order_relaxed);
        const i32 height = host_touch_height.load(std::memory_order_relaxed);
        if (width <= 0 || height <= 0 || GameMenuLevel < 0 || GameMenuLevel >= 10) {
            return 0;
        }

        const f32 x =
            static_cast<f32>(host_touch_x.load(std::memory_order_relaxed)) / static_cast<f32>(width) * 2.0f - 1.0f;
        const f32 y =
            1.0f - static_cast<f32>(host_touch_y.load(std::memory_order_relaxed)) / static_cast<f32>(height) * 2.0f;
        const i32 menu_id = GetMenuID();
        MENU &menu = GameMenu[GameMenuLevel];

        MechInputTouchMenuController::AnyTouchesThisFrame = 1;
        if (menu_id == 0) {
            return GAMEPAD_START | GAMEPAD_JUMP;
        }

        if (menu_id == 1) {
            const f32 centre_offset = static_cast<f32>(menu.last_row - menu.first_row) * MENUDY * -0.5f;
            const f32 first_y = -0.5f - centre_offset;
            const i32 row = static_cast<i32>((y - first_y) / MENUDY + 0.5f);
            const f32 row_y = first_y + static_cast<f32>(row) * MENUDY;
            if (x > -0.7f && x < 0.7f && row >= menu.first_row && row <= menu.last_row &&
                NuFabs(y - row_y) < NuFabs(MENUDY) * 0.6f) {
                menu.queued_item = row;
                menu.queued_column = menu.selected_column;
                menu.queued_row = row;
            }
            return 0;
        }

        if (menu_id == 1000 || menu_id == 1012) {
            if (NuFabs(y) < 0.22f && x > -0.75f && x < 0.75f) {
                i32 column = static_cast<i32>((x + 0.75f) / 0.5f);
                if (column > menu.last_column) {
                    column = menu.last_column;
                }
                menu.queued_item = menu.first_row;
                menu.queued_column = column;
                menu.queued_row = menu.first_row;
            } else if (NuFabs(y + 0.75f) < 0.14f && NuFabs(x) < 0.5f) {
                menu.queued_item = menu.last_row;
                menu.queued_column = menu.selected_column;
                menu.queued_row = menu.last_row;
            }
        }
        return 0;
    }
#endif
} // namespace

void HostInputReset() {
    for (i32 port = 0; port < 2; ++port) {
        host_pending_buttons[port].store(0, std::memory_order_relaxed);
        host_held_buttons[port].store(0, std::memory_order_relaxed);
        host_keyboard_buttons[port].store(0, std::memory_order_relaxed);
        host_frame_buttons[port] = 0;
    }
#ifdef __EMSCRIPTEN__
    host_touch_pending.store(false, std::memory_order_relaxed);
#endif
}

void HostInputSetHeld(i32 port, u32 buttons) {
    if (port < 0 || port >= 2) {
        return;
    }

    host_held_buttons[port].store(buttons, std::memory_order_release);
}

void HostInputSetKeyboardHeld(i32 port, u32 buttons) {
    if (port < 0 || port >= 2) {
        return;
    }

    host_keyboard_buttons[port].store(buttons, std::memory_order_release);
}

void HostInputTap(i32 port, u32 buttons) {
    if (port < 0 || port >= 2) {
        return;
    }

    host_pending_buttons[port].fetch_or(buttons, std::memory_order_release);
}

#ifdef __EMSCRIPTEN__
void HostInputTouch(i32 x, i32 y, i32 width, i32 height) {
    host_touch_x.store(x, std::memory_order_relaxed);
    host_touch_y.store(y, std::memory_order_relaxed);
    host_touch_width.store(width, std::memory_order_relaxed);
    host_touch_height.store(height, std::memory_order_relaxed);
    host_touch_pending.store(true, std::memory_order_release);
}
#endif

namespace NuInputDevicePS {

    u32 ClassInitPS() {
        HostInputReset();
        // Android exposes a built-in touch device at index 0 and the external
        // gamepad at index 1. Game code also queries index 1 directly when it
        // decides whether to render controller-oriented menu feedback.
        return 2;
    }

    void ClassShutdownPS() {
        HostInputReset();
    }

    void UpdateAllPS(f32) {
        for (i32 port = 0; port < 2; ++port) {
            const u32 tapped = host_pending_buttons[port].exchange(0, std::memory_order_acq_rel);
            const u32 held = host_held_buttons[port].load(std::memory_order_acquire);
            const u32 keyboard = host_keyboard_buttons[port].load(std::memory_order_acquire);
            host_frame_buttons[port] = tapped | held | keyboard;
#ifdef __EMSCRIPTEN__
            if (port == 0) {
                host_frame_buttons[port] |= host_consume_touch();
            }
#endif
        }
    }

    bool IsConnectedPS(u32 device) {
        // The host harness currently supplies keyboard-backed gamepad input,
        // but no touch events.  Advertising the placeholder touch device as
        // connected makes NuPad initially map player 0 to that silent device;
        // the first gamepad press is then spent remapping to device 1 instead
        // of reaching the menu.
        return device == host_gamepad_device;
    }

    bool IsInterceptedPS(u32) {
        return false;
    }

    bool HasHeadphonesConnectedPS(u32) {
        return false;
    }

    NUPADTYPE GetTypePS(u32 device) {
        return device == host_touch_device ? NUPADTYPE_TOUCH : NUPADTYPE_GAMEPAD;
    }

    NUPADATTACHMENTTYPE GetAttachmentTypePS(u32) {
        return NUPADATTACHMENTTYPE_NONE;
    }

    u32 GetCapsPS(u32 device) {
        return device == host_touch_device ? 0x440 : 0;
    }

    f32 GetVolumePS(u32) {
        return 0.0f;
    }

    void SetMotorsPS(u32, f32, f32) {
    }

    void ReadButtonsPS(u32 device, u32 *states) {
        // Harness port 0 is the first player-facing gamepad. The platform
        // device index remains 1, matching the Android device topology.
        *states = device == host_gamepad_device ? host_frame_buttons[0] : 0;
    }

    void ReadAnalogValuesPS(u32, f32 *values) {
        memset(values, 0, sizeof(f32) * 12);
    }

    void ReadMotionValuesPS(u32, f32 *values) {
        memset(values, 0, sizeof(f32) * 20);
    }

    void ReadTouchDataPS(u32, NuInputTouchData *data) {
        memset(data, 0, sizeof(*data));
    }

    void ReadMouseDataPS(u32, NuInputMouseData *data) {
        memset(data, 0, sizeof(*data));
    }

} // namespace NuInputDevicePS
