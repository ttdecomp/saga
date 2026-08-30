#include "host/platform/input.hpp"

#include <atomic>
#include <cstring>

#include "nu2api/nucore/NuInputDevice.h"

namespace {
    std::atomic<u32> host_pending_buttons[2];
    std::atomic<u32> host_held_buttons[2];
    u32 host_frame_buttons[2];
} // namespace

void HostInputReset() {
    for (i32 port = 0; port < 2; ++port) {
        host_pending_buttons[port].store(0, std::memory_order_relaxed);
        host_held_buttons[port].store(0, std::memory_order_relaxed);
        host_frame_buttons[port] = 0;
    }
}

void HostInputSetHeld(i32 port, u32 buttons) {
    if (port < 0 || port >= 2) {
        return;
    }

    host_held_buttons[port].store(buttons, std::memory_order_release);
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
        return 1;
    }

    void ClassShutdownPS() {
        HostInputReset();
    }

    void UpdateAllPS(f32) {
        for (i32 port = 0; port < 2; ++port) {
            const u32 tapped = host_pending_buttons[port].exchange(0, std::memory_order_acq_rel);
            const u32 held = host_held_buttons[port].load(std::memory_order_acquire);
            host_frame_buttons[port] = tapped | held;
        }
    }

    bool IsConnectedPS(u32 port) {
        return port == 0;
    }

    bool IsInterceptedPS(u32) {
        return false;
    }

    bool HasHeadphonesConnectedPS(u32) {
        return false;
    }

    NUPADTYPE GetTypePS(u32 port) {
        return port == 0 ? NUPADTYPE_GAMEPAD : NUPADTYPE_UNKNOWN_7;
    }

    NUPADATTACHMENTTYPE GetAttachmentTypePS(u32) {
        return NUPADATTACHMENTTYPE_NONE;
    }

    u32 GetCapsPS(u32) {
        return 0;
    }

    f32 GetVolumePS(u32) {
        return 0.0f;
    }

    void SetMotorsPS(u32, f32, f32) {
    }

    void ReadButtonsPS(u32 port, u32 *states) {
        *states = port < 2 ? host_frame_buttons[port] : 0;
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
