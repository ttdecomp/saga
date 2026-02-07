#pragma once

#include "nu2api.saga/nucore/common.h"

typedef struct _nugenericpad_s {
    u8 is_valid;
    i32 mapped_to_pad;

    u8 analog_left_x;
    u8 analog_left_y;
    u8 analog_right_x;
    u8 analog_right_y;

    u8 analog_l1;
    u8 analog_l2;
    u8 analog_r1;
    u8 analog_r2;

    u32 digital_buttons;
    u32 digital_buttons_pressed;
    u32 digital_buttons_released;
    u32 digital_buttons_prev;

    i32 analog_button_flags;

    i32 digital_left_x;
    i32 digital_left_y;
    i32 digital_right_x;
    i32 digital_right_y;

    i32 deadzone;

    f32 left_x_normal;
    f32 left_y_normal;

    // Originally `l_rx` and `l_ry`.
    f32 left_x_unknown;
    f32 left_y_unknown;

    // Originally `l_dx` and `l_dy`.
    f32 left_x_unknown2;
    f32 left_y_unknown2;

    // Originally `r_dx` and `r_dy`.
    f32 right_x_unknown2;
    f32 right_y_unknown2;

    f32 right_x_normal;
    f32 right_y_normal;

    // Originally `r_rx` and `r_ry`.
    f32 right_x_unknown;
    f32 right_y_unknown;

    f32 left_up_ramp;
    f32 left_down_ramp;
    f32 left_left_ramp;
    f32 left_right_ramp;

    u8 analog_left_pad_left;
    u8 analog_left_pad_right;
    u8 analog_left_pad_up;
    u8 analog_left_pad_down;

    u8 unknown_7c;
    u8 unknown_7d;
    u8 unknown_7e;
    u8 unknown_7f;
    u8 unknown_80;
    u8 unknown_81;
    u8 unknown_82;
    u8 unknown_83;
} NUGENERICPAD;

typedef struct nupad_s {
    // Artificial field to cover unknown space.
    u8 unknown_00[0x10];

    i32 port;
    i32 stage;
    i32 id;

    i32 digital_left_x;
    i32 digital_left_y;
    i32 digital_right_x;
    i32 digital_right_y;

    i32 slot;

    f32 left_x_normal;
    f32 left_y_normal;

    // Originally `l_rx` and `l_ry`.
    f32 left_x_unknown;
    f32 left_y_unknown;

    // Originally `l_dx` and `l_dy`.
    f32 left_x_unknown2;
    f32 left_y_unknown2;

    f32 right_x_normal;
    f32 right_y_normal;

    // Originally `r_rx` and `r_ry`.
    f32 right_x_unknown;
    f32 right_y_unknown;

    // Originally `r_dx` and `r_dy`.
    f32 right_x_unknown2;
    f32 right_y_unknown2;

    // Artificial field to cover unknown space.
    u8 unknown_60[0x20];

    u32 digital_buttons;
    u32 digital_buttons_prev;
    u32 digital_buttons_pressed;
    u32 digital_buttons_released;

    i8 has_analog_sticks : 1;
    i8 has_analog_buttons : 1;
    i8 is_valid : 1;
    i8 must_have_analog_sticks : 1;

    // Types uncertain.
    u8 unknown_91;
    u8 unknown_92;
    u8 unknown_93;
    u8 unknown_94;

    char deadzone;

    // Artificial field to cover unknown space.
    u8 unknown_96[0xa];

    u8 analog_right_x;
    u8 analog_right_y;
    u8 analog_left_x;
    u8 analog_left_y;

    u8 analog_left_pad_right;
    u8 analog_left_pad_left;
    u8 analog_left_pad_up;
    u8 analog_left_pad_down;

    // Types uncertain.
    u8 unknown_a8;
    u8 unknown_a9;
    u8 unknown_aa;
    u8 unknown_ab;

    u8 analog_l1;
    u8 analog_r1;
    u8 analog_l2;
    u8 analog_r2;

    // Types uncertain.
    u8 unknown_b0;
    u8 unknown_b1;
} NUPAD;

typedef struct _nupadmapping_s {
    i32 pad;
    i32 port;
    i32 is_active;
} NUPADMAPPING;

typedef enum nurecmode_e { NUPAD_NORM = 0, NUPAD_RECORD = 1, NUPAD_PLAY = 2 } NURECMODE;

typedef struct nupadrec_s {
    NURECMODE mode;

    u8 *buf_start;
    u8 *buf_end;

    VARIPTR record;

    i32 buf_size;

    char *filepath;

    i32 should_end;

    i32 end_record_buttons;
    i32 end_play_buttons;

    i32 is_recording;
} NUPADREC;

#ifdef __cplusplus
extern "C" {
#endif
    extern NUPADMAPPING g_nupadMapping[];
    extern i32 g_profilePlayerPad;
    extern i32 MaxGamePads;
    extern char UseCorrectDeadZoning;
    extern i32 enable_touch_controls;

    void NuPadInit();
    void NuPadInitPS(NUGENERICPAD *pad);

    i32 NuPadRead(NUPAD *pad);

    void NuPadUpdatePads();

    i32 NuPadGetMaxGamePads();

    void NuPadRecordStart();

    i32 NuPad_Interface_GetMaxDevices();
    void NuPad_Interface_InputManagerUpdate(f32 unknown);
    u8 NuPad_Interface_NuPadRead(i32 port, u8 *analog_left_x, u8 *analog_left_y, u8 *analog_right_x, u8 *analog_right_y,
                                 u8 *analog_l1, u8 *analog_l2, u8 *analog_r1, u8 *analog_r2, u32 *digital_buttons,
                                 u8 *unknown, i32 *analog_button_flags);
#ifdef __cplusplus
}

NUPAD *NuPadOpen(i32 port, i32 slot);
void NuPadOpenPS(NUPAD *pad);

void NuPadClose(NUPAD *pad);
void NuPadClosePS(NUPAD *pad);

void NuPadProcessReadData(NUGENERICPAD *pad);

i32 NuPadGetDeadzoneByPortPS(i32 port);

void NuPadRecordPlay(NUGENERICPAD *pad);
void NuPadRecordEnd();

void NuPadRecordSave(char *filepath);

#endif
