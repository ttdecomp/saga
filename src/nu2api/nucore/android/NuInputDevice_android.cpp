#include "nu2api/nucore/android/NuInputDevice_android.h"

#include <pthread.h>

#include "nu2api/nucore/NuInputDevice.h"
#include "nu2api/nucore/common.h"

namespace NuInputDevicePS {
    bool m_gamepadConnected;

    u32 m_deviceButtons;
    u32 m_padButtons;

    pthread_mutex_t m_touchEventQueueCriticalSection;

    u32 ClassInitPS(void) {
        return 0;
    }

    void ClassShutdownPS(void) {
    }

    void UpdateAllPS(f32 delta_time) {
    }

    void HandleGamepPadStatusConnect(bool is_connected) {
        m_gamepadConnected = is_connected;
    }

    bool IsConnectedPS(u32 port) {
        if (port == 0) {
            return true;
        }

        if (port == 1) {
            return m_gamepadConnected;
        }

        return false;
    }

    bool IsInterceptedPS(u32 port) {
        return false;
    }

    bool HasHeadphonesConnectedPS(u32 port) {
        return false;
    }

    NUPADTYPE GetTypePS(u32 port) {
        return port == 0 ? NUPADTYPE_TOUCH : NUPADTYPE_GAMEPAD;
    }

    NUPADATTACHMENTTYPE GetAttachmentTypePS(u32 port) {
        return NUPADATTACHMENTTYPE_NONE;
    }

    u32 GetCapsPS(u32 port) {
        return port == 0 ? 0x440 : 0x0;
    }

    f32 GetVolumePS(u32 port) {
        return 0.0f;
    }

    void SetMotorsPS(u32 port, f32 motor_1, f32 motor_2) {
    }

    void ReadButtonsPS(u32 port, u32 *states) {
        *states = 0;

        if (port == 0) {
            *states = m_deviceButtons;
        }

        if (port == 1) {
            *states = m_padButtons;
        }
    }

    void ReadAnalogValuesPS(u32 port, f32 *values) {
    }

    void ReadMotionValuesPS(u32 port, f32 *values) {
    }

    void ReadTouchDataPS(u32 port, NuInputTouchData *data) {
    }

    void ReadMouseDataPS(u32 port, NuInputMouseData *data) {
    }

    u32 GetGamePadButtonIndex(i32 key, i32 *port) {
        return {};
    }

    void HandleKeyDown_ANDROID_SPECIFIC(i32 key) {
        u32 button_idx;
        i32 port;

        pthread_mutex_lock(&m_touchEventQueueCriticalSection);

        button_idx = GetGamePadButtonIndex(key, &port);

        if (port == 0) {
            m_deviceButtons |= button_idx;
        } else {
            m_padButtons |= button_idx;
        }

        pthread_mutex_unlock(&m_touchEventQueueCriticalSection);
    }

    void HandleKeyUp_ANDROID_SPECIFIC(i32 key) {
        u32 button_idx;
        i32 port;

        pthread_mutex_lock(&m_touchEventQueueCriticalSection);

        button_idx = ~GetGamePadButtonIndex(key, &port);

        if (port == 0) {
            m_deviceButtons &= button_idx;
        } else {
            m_padButtons &= button_idx;
        }

        pthread_mutex_unlock(&m_touchEventQueueCriticalSection);
    }
}; // namespace NuInputDevicePS
