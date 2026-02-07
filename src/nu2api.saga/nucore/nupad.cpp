#include "nu2api.saga/nucore/nupad.h"

#include <string.h>

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nuapi.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/numath/nufloat.h"
#include "nu2api.saga/numemory/NuMemoryManager.h"
#include "nu2api.saga/numemory/numemory.h"

#define MAX_GAME_PAD_COUNT 2

#define ANALOG_CENTER 0x80

static NUGENERICPAD g_nupadScannedPads[MAX_GAME_PAD_COUNT];
static i32 g_atLeastOnePadBeenActivated;
static i32 g_directpadMapping;

NUPADMAPPING g_nupadMapping[MAX_GAME_PAD_COUNT];
i32 g_profilePlayerPad = -1;
i32 MaxGamePads = MAX_GAME_PAD_COUNT;
char UseCorrectDeadZoning;
i32 enable_touch_controls = 1;

void NuPadInit() {
    i32 i;

    memset(g_nupadScannedPads, 0, sizeof(g_nupadScannedPads));

    for (i = 0; i < MAX_GAME_PAD_COUNT; i++) {
        g_nupadScannedPads[i].mapped_to_pad = -1;

        NuPadInitPS(g_nupadScannedPads + i);
    }

    for (i = 0; i < MAX_GAME_PAD_COUNT; i++) {
        g_nupadMapping[i].is_active = 0;
        g_nupadMapping[i].pad = -1;
        g_nupadMapping[i].port = -1;
    }

    g_atLeastOnePadBeenActivated = 0;
}

NUPAD *NuPadOpen(i32 port, i32 slot) {
    NUPAD *pad;

    pad = (NUPAD *)NuMemoryGet()->GetThreadMem()->_BlockAlloc(sizeof(NUPAD) + 0x800, 0x4,
                                                              NuMemoryManager::MEM_ALLOC_ZERO, "", 0);

    if (pad != NULL) {
        memset(pad, 0, sizeof(NUPAD));

        pad->port = port;
        pad->slot = slot;
        pad->stage = 0;

        pad->analog_right_x = pad->analog_right_y = pad->analog_left_x = pad->analog_left_y = ANALOG_CENTER;
    }

    NuPadOpenPS(pad);

    return pad;
}

void NuPadClose(NUPAD *pad) {
    NuPadClosePS(pad);
    NuMemoryGet()->GetThreadMem()->BlockFree(pad, 0);
}

i32 NuPadRead(NUPAD *pad) {
    i32 port;

    if (g_nupadMapping[pad->port].port != -1 || (g_directpadMapping && g_nupadScannedPads[pad->port].is_valid)) {
        if (g_directpadMapping != 0) {
            port = pad->port;
        } else {
            port = g_nupadMapping[pad->port].port;
        }

        pad->analog_left_x = g_nupadScannedPads[port].analog_left_x;
        pad->analog_left_y = g_nupadScannedPads[port].analog_left_y;
        pad->analog_right_x = g_nupadScannedPads[port].analog_right_x;
        pad->analog_right_y = g_nupadScannedPads[port].analog_right_y;

        pad->analog_l1 = g_nupadScannedPads[port].analog_l1;
        pad->analog_l2 = g_nupadScannedPads[port].analog_l2;
        pad->analog_r1 = g_nupadScannedPads[port].analog_r1;
        pad->analog_r2 = g_nupadScannedPads[port].analog_r2;

        pad->digital_buttons = g_nupadScannedPads[port].digital_buttons;
        pad->digital_buttons_pressed = g_nupadScannedPads[port].digital_buttons_pressed;
        pad->digital_buttons_released = g_nupadScannedPads[port].digital_buttons_released;
        pad->digital_buttons_prev = g_nupadScannedPads[port].digital_buttons_prev;

        pad->has_analog_buttons = g_nupadScannedPads[port].analog_button_flags;

        pad->deadzone = g_nupadScannedPads[port].deadzone;

        pad->digital_left_x = g_nupadScannedPads[port].digital_left_x;
        pad->digital_left_y = g_nupadScannedPads[port].digital_left_y;
        pad->digital_right_x = g_nupadScannedPads[port].digital_right_x;
        pad->digital_right_y = g_nupadScannedPads[port].digital_right_y;

        pad->left_x_normal = g_nupadScannedPads[port].left_x_normal;
        pad->left_y_normal = g_nupadScannedPads[port].left_y_normal;
        pad->left_x_unknown = g_nupadScannedPads[port].left_x_unknown;
        pad->left_y_unknown = g_nupadScannedPads[port].left_y_unknown;
        pad->left_x_unknown2 = g_nupadScannedPads[port].left_x_unknown2;
        pad->left_y_unknown2 = g_nupadScannedPads[port].left_y_unknown2;

        pad->right_x_unknown2 = g_nupadScannedPads[port].right_x_unknown2;
        pad->right_y_unknown2 = g_nupadScannedPads[port].right_y_unknown2;
        pad->right_x_normal = g_nupadScannedPads[port].right_x_normal;
        pad->right_y_normal = g_nupadScannedPads[port].right_y_normal;
        pad->right_x_unknown = g_nupadScannedPads[port].right_x_unknown;
        pad->right_y_unknown = g_nupadScannedPads[port].right_y_unknown;

        pad->analog_left_pad_left = g_nupadScannedPads[port].analog_left_pad_left;
        pad->analog_left_pad_right = g_nupadScannedPads[port].analog_left_pad_right;
        pad->analog_left_pad_up = g_nupadScannedPads[port].analog_left_pad_up;
        pad->analog_left_pad_down = g_nupadScannedPads[port].analog_left_pad_down;

        pad->unknown_b0 = g_nupadScannedPads[port].unknown_82;
        pad->unknown_b1 = g_nupadScannedPads[port].unknown_83;

        pad->is_valid = g_nupadScannedPads[port].is_valid;

        return pad->is_valid;
    }

    pad->is_valid = false;

    return 0;
}

static void DeadZoneValueXYProper(i32 *x_value, i32 *y_value, i32 deadzone) {
    i32 inverse;
    i32 cur_value;

    inverse = ANALOG_CENTER - deadzone;

    cur_value = *x_value;
    if (cur_value >= deadzone) {
        cur_value++;

        *x_value = ((cur_value - deadzone) * 0x7f) / inverse;
    } else if (cur_value < -deadzone) {
        *x_value = ((cur_value + deadzone) * 0x7f) / inverse;
    } else {
        *x_value = 0;
    }

    cur_value = *y_value;
    if (cur_value >= deadzone) {
        cur_value++;

        *y_value = ((cur_value - deadzone) * 0x7f) / inverse;
    } else if (cur_value < -deadzone) {
        *y_value = ((cur_value + deadzone) * 0x7f) / inverse;
    } else {
        *y_value = 0;
    }
}

static void DeadZoneValueXY(i32 *x_value, i32 *y_value, i32 deadzone) {
    f32 x_normal;
    f32 y_normal;
    f32 deadzone_normal;
    f32 magnitude;

    if (UseCorrectDeadZoning) {
        DeadZoneValueXYProper(x_value, y_value, deadzone);

        return;
    }

    x_normal = *x_value / 127.0f;
    y_normal = *y_value / 127.0f;
    deadzone_normal = deadzone / 127.0f;

    if (x_normal > 1.0f) {
        x_normal = 1.0f;
    }

    if (x_normal < -1.0f) {
        x_normal = -1.0f;
    }

    if (y_normal > 1.0f) {
        y_normal = 1.0f;
    }

    if (y_normal < -1.0f) {
        y_normal = -1.0f;
    }

    magnitude = NuFsqrt(x_normal * x_normal + y_normal * y_normal);

    if (magnitude < deadzone_normal) {
        *x_value = 0;
        *y_value = 0;
    }
}

static f32 ramp_vals[][2] = {{0.0f, 0.0f}, {0.5f, 0.1f}, {0.75f, 0.25f}, {0.85f, 0.5f}, {1.0f, 1.0f}};

static f32 CalcRamp(f32 ramp) {
    i32 i;

    ramp = MIN(ramp, 1.0f);

    for (i = 0; ramp > ramp_vals[i][0]; i++) {
    }

    return ramp_vals[i][1];
}

static f32 ramp_rate = 0.1f;

enum {
    PAD_BUTTON_JUMP_SELECT = 1 << 0x6,
    PAD_BUTTON_START = 1 << 0xb,
    PAD_BUTTON_UP = 1 << 0xc,
    PAD_BUTTON_RIGHT = 1 << 0xd,
    PAD_BUTTON_DOWN = 1 << 0xe,
    PAD_BUTTON_LEFT = 1 << 0xf,
};

void NuPadProcessReadData(NUGENERICPAD *pad) {
    f32 ramp_rate_normal;

    pad->digital_buttons_pressed = pad->digital_buttons & ~pad->digital_buttons_prev;
    pad->digital_buttons_released = ~pad->digital_buttons & pad->digital_buttons_prev;

    pad->digital_buttons_prev = pad->digital_buttons;

    pad->digital_left_x = pad->analog_left_x - ANALOG_CENTER;
    pad->digital_left_y = ANALOG_CENTER - pad->analog_left_y;

    pad->digital_right_x = pad->analog_right_x - ANALOG_CENTER;
    pad->digital_right_y = ANALOG_CENTER - pad->analog_right_y;

    if (pad->deadzone != 0) {
        DeadZoneValueXY(&pad->digital_left_x, &pad->digital_left_y, pad->deadzone);
        DeadZoneValueXY(&pad->digital_right_x, &pad->digital_right_y, pad->deadzone);

        // Keep analog values consistent after deadzone modification.
        pad->analog_left_x = pad->digital_left_x + ANALOG_CENTER;
        pad->analog_left_y = ANALOG_CENTER - pad->digital_left_y;

        pad->analog_right_x = pad->digital_right_x + ANALOG_CENTER;
        pad->analog_right_y = ANALOG_CENTER - pad->digital_right_y;
    }

    pad->left_x_normal = (f32)pad->digital_left_x;
    pad->left_y_normal = (f32)pad->digital_left_y;

    pad->left_x_normal /= 128.0f;
    pad->left_y_normal /= 128.0f;

    if (pad->left_x_normal > 0.0f) {
        pad->left_x_normal = MAX(0.0f, pad->left_x_normal - pad->left_x_unknown2);
    }

    if (pad->left_x_normal < 0.0f) {
        pad->left_x_normal = MIN(0.0f, pad->left_x_normal + pad->left_x_unknown2);
    }

    if (pad->left_y_normal > 0.0f) {
        pad->left_y_normal = MAX(0.0f, pad->left_y_normal - pad->left_y_unknown2);
    }

    if (pad->left_y_normal < 0.0f) {
        pad->left_y_normal = MIN(0.0f, pad->left_y_normal + pad->left_y_unknown2);
    }

    pad->right_x_normal = (f32)pad->digital_right_x;
    pad->right_y_normal = (f32)pad->digital_right_y;

    pad->right_x_normal /= 128.0f;
    pad->right_y_normal /= 128.0f;

    if (pad->right_x_normal > 0.0f) {
        pad->right_x_normal = MAX(0.0f, pad->right_x_normal - pad->right_x_unknown2);
    }

    if (pad->right_x_normal < 0.0f) {
        pad->right_x_normal = MIN(0.0f, pad->right_x_normal + pad->right_x_unknown2);
    }

    if (pad->right_y_normal > 0.0f) {
        pad->right_y_normal = MAX(0.0f, pad->right_y_normal - pad->right_y_unknown2);
    }

    if (pad->right_y_normal < 0.0f) {
        pad->right_y_normal = MIN(0.0f, pad->right_y_normal + pad->right_y_unknown2);
    }

    if ((pad->analog_button_flags & 1) != 0) {
        // The right-hand side values never appear to be written. This may be a
        // platform difference.
        pad->analog_left_pad_left = pad->unknown_7c;
        pad->analog_left_pad_right = pad->unknown_7d;
        pad->analog_left_pad_up = pad->unknown_7e;
        pad->analog_left_pad_down = pad->unknown_7f;
    } else {
        ramp_rate_normal = ramp_rate / 30.0f;

        if ((pad->digital_buttons & PAD_BUTTON_UP) != 0) {
            pad->left_up_ramp = MIN(1.0f, pad->left_up_ramp + ramp_rate_normal);
        } else {
            pad->left_up_ramp = 0.0f;
        }

        if ((pad->digital_buttons & PAD_BUTTON_DOWN) != 0) {
            pad->left_down_ramp = MIN(1.0f, pad->left_down_ramp + ramp_rate_normal);
        } else {
            pad->left_down_ramp = 0.0f;
        }

        if ((pad->digital_buttons & PAD_BUTTON_LEFT) != 0) {
            pad->left_left_ramp = MIN(1.0f, pad->left_left_ramp + ramp_rate_normal);
        } else {
            pad->left_left_ramp = 0.0f;
        }

        if ((pad->digital_buttons & PAD_BUTTON_RIGHT) != 0) {
            pad->left_right_ramp = MIN(1.0f, pad->left_right_ramp + ramp_rate_normal);
        } else {
            pad->left_right_ramp = 0.0f;
        }

        pad->analog_left_pad_left = CalcRamp(pad->left_left_ramp) * 255.0f;
        pad->analog_left_pad_right = CalcRamp(pad->left_right_ramp) * 255.0f;
        pad->analog_left_pad_up = CalcRamp(pad->left_up_ramp) * 255.0f;
        pad->analog_left_pad_down = CalcRamp(pad->left_down_ramp) * 255.0f;
    }

    if ((pad->analog_button_flags & 2) != 0) {
        pad->unknown_82 = pad->unknown_80;
        pad->unknown_83 = pad->unknown_81;
    } else {
        pad->unknown_82 = 0x7f;
        pad->unknown_83 = 0x7f;
    }
}

void NuPadUpdatePads() {
    i32 max_interface_devices;
    i32 max_game_pads;
    i32 i;
    i32 j;
    int has_active_pad;
    i32 unused;
    i32 unused2;

    max_interface_devices = NuPad_Interface_GetMaxDevices();
    max_game_pads = NuPadGetMaxGamePads();

    NuPad_Interface_InputManagerUpdate(0.0f);

    for (i = 0; i < max_interface_devices; i++) {
        g_nupadScannedPads[i].is_valid = NuPad_Interface_NuPadRead(
            i, &g_nupadScannedPads[i].analog_left_x, &g_nupadScannedPads[i].analog_left_y,
            &g_nupadScannedPads[i].analog_right_x, &g_nupadScannedPads[i].analog_right_y,
            &g_nupadScannedPads[i].analog_l1, &g_nupadScannedPads[i].analog_l2, &g_nupadScannedPads[i].analog_r1,
            &g_nupadScannedPads[i].analog_r2, &g_nupadScannedPads[i].digital_buttons, &g_nupadScannedPads[i].unknown_7c,
            &g_nupadScannedPads[i].analog_button_flags);

        NuPadRecordPlay(g_nupadScannedPads + i);

        if (g_nupadScannedPads[i].is_valid) {
            g_nupadScannedPads[i].deadzone = NuPadGetDeadzoneByPortPS(i);
        }

        NuPadProcessReadData(g_nupadScannedPads + i);
    }

    if (nuapi.pad_record.should_end == 1) {
        NuPadRecordEnd();
    }

    for (i = 0; i < max_interface_devices; i++) {
        has_active_pad = false;

        for (j = 0; j < max_game_pads; j++) {
            if (g_nupadMapping[j].is_active && g_nupadMapping[j].port == i) {
                has_active_pad = true;
            }
        }

        if (has_active_pad) {
            continue;
        }

        g_nupadScannedPads[i].mapped_to_pad = -1;
    }

    if (!g_atLeastOnePadBeenActivated) {
        g_nupadMapping[0].port = -1;

        for (j = 0; j < max_interface_devices; j++) {
            if (g_nupadScannedPads[j].is_valid) {
                if (g_nupadMapping[0].port == -1) {
                    g_nupadMapping[0].port = j;
                    g_nupadScannedPads[j].mapped_to_pad = 0;
                }

                if (g_nupadScannedPads[j].digital_buttons != 0 ||
                    g_nupadScannedPads[j].analog_left_x != ANALOG_CENTER ||
                    g_nupadScannedPads[j].analog_left_y != ANALOG_CENTER ||
                    g_nupadScannedPads[j].analog_right_x != ANALOG_CENTER ||
                    g_nupadScannedPads[j].analog_right_y != ANALOG_CENTER) {
                    unused = 1;
                    g_nupadMapping[0].port = j;
                    g_nupadScannedPads[j].mapped_to_pad = 0;

                    if (enable_touch_controls) {
                        return;
                    }

                    if ((g_nupadScannedPads[j].digital_buttons_pressed & (PAD_BUTTON_START | PAD_BUTTON_JUMP_SELECT)) !=
                        0) {
                        return;
                    }
                }
            }
        }
    }

    if (g_atLeastOnePadBeenActivated) {
        if (g_nupadMapping[0].is_active) {
            unused2 = 0;
        } else {
            unused2 = 1;
        }

        for (i = 0; i < max_game_pads; i++) {
            if (g_nupadMapping[i].is_active == 1 && !g_nupadScannedPads[g_nupadMapping[i].port].is_valid) {
                for (j = 0; j < max_interface_devices; j++) {
                    if (g_nupadScannedPads[j].is_valid &&
                        (g_nupadScannedPads[j].digital_buttons_pressed & PAD_BUTTON_START) != 0 &&
                        (g_nupadScannedPads[j].mapped_to_pad == -1 ||
                         !g_nupadMapping[g_nupadScannedPads[j].mapped_to_pad].is_active)) {
                        g_nupadMapping[i].port = j;
                        g_nupadScannedPads[j].mapped_to_pad = i;
                        g_nupadScannedPads[j].digital_buttons_pressed = 0;

                        return;
                    }
                }
            }

            if (!g_nupadMapping[i].is_active) {
                g_nupadMapping[i].port = -1;

                if (g_nupadMapping[0].is_active + g_nupadMapping[1].is_active == 0) {
                    if (i == 0) {
                        for (j = 0; j < max_interface_devices; j++) {
                            if (g_nupadScannedPads[j].mapped_to_pad != -1 &&
                                g_nupadMapping[g_nupadScannedPads[j].mapped_to_pad].is_active == 1) {
                                continue;
                            }

                            g_nupadScannedPads[j].mapped_to_pad = -1;
                        }
                    }
                } else {
                    for (j = 0; j < max_interface_devices; j++) {
                        if (g_nupadScannedPads[j].mapped_to_pad != -1 &&
                            g_nupadMapping[g_nupadScannedPads[j].mapped_to_pad].is_active == 1) {
                            continue;
                        }

                        g_nupadScannedPads[j].mapped_to_pad = -1;
                    }
                }

                for (j = 0; j < max_interface_devices; j++) {
                    if (g_nupadScannedPads[j].mapped_to_pad == -1 && g_nupadScannedPads[j].is_valid) {
                        g_nupadMapping[i].port = j;
                        g_nupadScannedPads[j].mapped_to_pad = i;

                        if (g_nupadScannedPads[j].digital_buttons != 0 ||
                            g_nupadScannedPads[j].analog_left_x != ANALOG_CENTER ||
                            g_nupadScannedPads[j].analog_left_y != ANALOG_CENTER ||
                            g_nupadScannedPads[j].analog_right_x != ANALOG_CENTER ||

                            // ORIG_BUG: This tests `analog_right_x` twice.
                            g_nupadScannedPads[j].analog_right_x != ANALOG_CENTER ||
                            (g_nupadScannedPads[j].digital_buttons_pressed &
                             (PAD_BUTTON_START | PAD_BUTTON_JUMP_SELECT)) != 0) {
                            break;
                        }
                    }
                }
            }
        }
    }
}

i32 NuPadGetMaxGamePads() {
    return MaxGamePads;
}

void NuPadRecordStart(void) {
    nuapi.pad_record.is_recording = 1;
}

void NuPadRecordPlay(NUGENERICPAD *pad) {
    VARIPTR *record_ptr;

    if (pad->is_valid && nuapi.pad_record.mode == NUPAD_PLAY &&
        (pad->digital_buttons & nuapi.pad_record.end_play_buttons) != 0) {
        nuapi.pad_record.mode = NUPAD_NORM;
    }

    if (pad->is_valid && nuapi.pad_record.mode == NUPAD_RECORD &&
        (pad->digital_buttons & nuapi.pad_record.end_record_buttons) != 0) {
        nuapi.pad_record.should_end = 1;
    }

    record_ptr = &nuapi.pad_record.record;

    switch (nuapi.pad_record.mode) {
        case NUPAD_RECORD:
            *record_ptr->u32_ptr++ = pad->is_valid;

            if (pad->is_valid) {
                *record_ptr->u8_ptr++ = pad->analog_left_x;
                *record_ptr->u8_ptr++ = pad->analog_left_y;
                *record_ptr->u8_ptr++ = pad->analog_right_x;
                *record_ptr->u8_ptr++ = pad->analog_right_y;

                *record_ptr->u8_ptr++ = pad->analog_l1;
                *record_ptr->u8_ptr++ = pad->analog_l2;
                *record_ptr->u8_ptr++ = pad->analog_r1;
                *record_ptr->u8_ptr++ = pad->analog_r2;

                *record_ptr->u32_ptr++ = pad->digital_buttons;
            }
            break;
        case NUPAD_PLAY:
            pad->is_valid = *record_ptr->u32_ptr++;

            if (pad->is_valid) {
                pad->analog_left_x = *record_ptr->u8_ptr++;
                pad->analog_left_y = *record_ptr->u8_ptr++;
                pad->analog_right_x = *record_ptr->u8_ptr++;
                pad->analog_right_y = *record_ptr->u8_ptr++;

                pad->analog_l1 = *record_ptr->u8_ptr++;
                pad->analog_l2 = *record_ptr->u8_ptr++;
                pad->analog_r1 = *record_ptr->u8_ptr++;
                pad->analog_r2 = *record_ptr->u8_ptr++;

                pad->digital_buttons = *record_ptr->u32_ptr++;
            }
            break;
        default:
            break;
    }
}

void NuPadRecordEnd() {
    switch (nuapi.pad_record.mode) {
        case NUPAD_RECORD:
            NuPadRecordSave(nuapi.pad_record.filepath);
            break;
        case NUPAD_PLAY:
            nuapi.pad_record.mode = NUPAD_NORM;
            nuapi.pad_record.record.u8_ptr = nuapi.pad_record.buf_start;
            break;
    }

    nuapi.pad_record.should_end = 0;
}

void NuPadRecordSave(char *filepath) {
    NUFILE file;
    u8 *record_start;
    usize record_size;
    NUPADREC *record;

    record = &nuapi.pad_record;

    if (record->mode == NUPAD_RECORD && record->record.u8_ptr != record->buf_start) {
        if (filepath != NULL) {
            file = NuFileOpen(filepath, NUFILE_WRITE);

            if (file != 0) {
                record_size = record->record.addr - (usize)record->buf_start;

                record_start = record->buf_start;

                NuFileWriteInt(file, record_size);
                NuFileWrite(file, record_start, record_size);
                NuFileWriteUnsignedInt(file, 0xe0f);

                NuFileClose(file);
            }
        }

        record->should_end = 0;
        record->mode = NUPAD_NORM;
    }
}
