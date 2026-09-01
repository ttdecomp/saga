#include "host/platform/input.hpp"

#include <atomic>
#include <cstring>

#include "nu2api/nucore/NuInputDevice.h"

namespace {
    constexpr u32 host_touch_device = 0;
    constexpr u32 host_gamepad_device = 1;
    std::atomic<u32> host_pending_buttons[2];
    std::atomic<u32> host_held_buttons[2];
    std::atomic<u32> host_keyboard_buttons[2];
    u32 host_frame_buttons[2];
} // namespace

void HostInputReset() {
    for (i32 port = 0; port < 2; ++port) {
        host_pending_buttons[port].store(0, std::memory_order_relaxed);
        host_held_buttons[port].store(0, std::memory_order_relaxed);
        host_keyboard_buttons[port].store(0, std::memory_order_relaxed);
        host_frame_buttons[port] = 0;
    }
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
