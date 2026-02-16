#pragma once

#include <string.h>

#include "nu2api/nucore/common.h"

struct NuInputMouseData {
    // Types uncertain.
    u8 unknown_0;
    u8 unknown_1;
    u8 unknown_2;
    u8 unknown_3;
    u8 unknown_4;
    u8 unknown_5;

    u16 unknown_6;

    u32 unknown_8;
    u32 unknown_c;
};

struct NuInputTouch {
    u8 unknown_00;
    u8 unknown_01;
    u8 unknown_02;

    f32 unknown_04;
    f32 unknown_08;
    f32 unknown_0c;
    f32 unknown_10;

    u32 unknown_14;
};

struct NuInputTouchData {
    // Type uncertain.
    u32 touch_count;

    NuInputTouch touch_events[10];
};

enum NUPADTYPE {
    NUPADTYPE_GAMEPAD = 0,
    NUPADTYPE_MOUSE = 3,
    NUPADTYPE_TOUCH = 4,
    NUPADTYPE_UNKNOWN_6 = 6,
    NUPADTYPE_UNKNOWN_7 = 7,
};

enum NUPADATTACHMENTTYPE {
    NUPADATTACHMENTTYPE_NONE = 0,
};

enum NUPADANALOGVALUE {
    NUPADANALOGVALUE_UNKNOWN_0 = 0x0,
    NUPADANALOGVALUE_UNKNOWN_1 = 0x1,
    NUPADANALOGVALUE_UNKNOWN_2 = 0x2,
    NUPADANALOGVALUE_UNKNOWN_3 = 0x3,

    NUPADANALOGVALUE_L1 = 0x4,
    NUPADANALOGVALUE_R1 = 0x5,
    NUPADANALOGVALUE_L2 = 0x6,
    NUPADANALOGVALUE_R2 = 0x7,

    NUPADANALOGVALUE_LEFT_X = 0x8,
    NUPADANALOGVALUE_LEFT_Y = 0x9,
    NUPADANALOGVALUE_RIGHT_X = 0xa,
    NUPADANALOGVALUE_RIGHT_Y = 0xb,
};

class NuInputDeviceTranslator {
  public:
    virtual void Execute(u32 port, NUPADTYPE in_type, NUPADATTACHMENTTYPE in_attch_type, u32 in_caps, u32 in_buttons,
                         const float *in_analog, const float *in_motion, const NuInputTouchData *in_touch_data,
                         const NuInputMouseData *in_mouse_data, NUPADTYPE &out_pad_type,
                         NUPADATTACHMENTTYPE &out_attch_type, u32 &out_caps, u32 &out_buttons, float *out_analog,
                         float *out_motion, NuInputTouchData *out_touch_data, NuInputMouseData *out_mouse_data);
};

class NuInputDevice {
  public:
    NuInputDevice(u32 port);

    // This constructor is theorized based on the inlining of a constructor
    // without initialization of touch data in the `NuInputManager` ctor. Its
    // signature and exact nature are unknown.
    NuInputDevice(u32 port, u32 unknown) {
        this->port = port;
        this->translators[0] = NULL;
        this->is_rumble_killed = false;

        this->prev_valid_type = NUPADTYPE_UNKNOWN_7;
        this->prev_valid_idx_by_type = 7;

        this->translators[1] = NULL;

        this->translator_count = 0;

        SetDisconnected();
        Clear();
    }

    void Update(f32 delta_time, bool emulate_touch);

    bool IsConnected(void) const;

    f32 GetAnalogValue(NUPADANALOGVALUE input) const;
    u32 GetButtons(void) const;
    NUPADTYPE GetType(void) const;

    void SetIndexByType(u32 idx_by_type);

    void SetDisconnected(void);
    void Clear(void);

  private:
    void DeadZone(NUPADANALOGVALUE input, f32 dead_zone);

    void ConvertToEmulatedTouchFromMouse(void);

    u32 port;

    NUPADTYPE type;
    u32 idx_by_type;

    NUPADTYPE prev_valid_type;
    u32 prev_valid_idx_by_type;

    NUPADATTACHMENTTYPE attch_type;

    u32 caps;

    bool is_connected;
    bool is_intercepted;

    u32 button_states;

    f32 analog_values[12];
    f32 motion_values[20];

    NuInputTouchData unknown_touch_data;
    NuInputTouchData touch_data;

    NuInputMouseData mouse_data;

    f32 motor_1;
    f32 motor_2;

    bool is_rumble_killed;

    f32 rumble_time_elapsed;
    f32 rumble_cooldown;

    bool has_headphones_connected;

    f32 volume;

    NuInputDeviceTranslator *translators[2];

    // Type uncertain.
    u32 translator_count;
};

namespace NuInputDevicePS {
    u32 ClassInitPS(void);
    void ClassShutdownPS(void);

    void UpdateAllPS(f32 delta_time);

    bool IsConnectedPS(u32 port);
    bool IsInterceptedPS(u32 port);
    bool HasHeadphonesConnectedPS(u32 port);

    NUPADTYPE GetTypePS(u32 port);
    NUPADATTACHMENTTYPE GetAttachmentTypePS(u32 port);
    u32 GetCapsPS(u32 port);
    f32 GetVolumePS(u32 port);

    void SetMotorsPS(u32 port, f32 motor_1, f32 motor_2);

    void ReadButtonsPS(u32 port, u32 *states);
    void ReadAnalogValuesPS(u32 port, f32 *values);
    void ReadMotionValuesPS(u32 port, f32 *values);
    void ReadTouchDataPS(u32 port, NuInputTouchData *data);
    void ReadMouseDataPS(u32 port, NuInputMouseData *data);
}; // namespace NuInputDevicePS
