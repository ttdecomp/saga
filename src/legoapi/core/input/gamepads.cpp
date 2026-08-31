#include "legoapi/core/input/gamepads.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/NuInputDevice.h"
#include "nu2api/nucore/nupad.h"

#include <string.h>

extern "C" nupad_s **Game_NuPad;
extern GAMECAMERA_s *GameCam;
extern WORLDINFO_s *WORLD;
extern i32 (*GamePads_IgnoreInputFn)(void);
extern NUPAD *pActivePad;

// Original bss @0x127a500: 64 pads x 0x60 bytes.
GAMEPAD_s GamePad[64];
// Original bss @0x127a4e0.
i32 readpads_always = 0;

__attribute__((optimize("O2"))) void GamePads_Init() {
    Game_NuPad[0] = NuPadOpen(0, 0);
    Game_NuPad[1] = NuPadOpen(1, 0);

    for (i32 i = 0; i < 64; ++i) {
        memset(&GamePad[i], 0, sizeof(GamePad[i]));
    }
    GamePad[0].pad = Game_NuPad[0];
    GamePad[1].pad = Game_NuPad[1];
}

void GamePad_Rotate(GameObject_s *) {
}

void GamePad_Waggle(GAMEPAD_s *) {
}

GAMEPAD_s *GamePad_Allocate() {
    for (i32 i = 0; i < 64; i++) {
        if ((GamePad[i].allocated_5a & 1) == 0) {
            GamePad[i].allocated_5a |= 1;
            return &GamePad[i];
        }
    }
    return NULL;
}

void GamePads_NetHost() {
}

void GamePads_NetReset(i32) {
}

__attribute__((optimize("O2,omit-frame-pointer,no-reorder-blocks"))) u16 GamePad_InputAngle(GameObject_s *object,
                                                                                            GAMEPAD_s *pad) {
    if (static_cast<i8>(object->apiobj.field_0x1f8) >= 0 || object->field_0x661 == 0xff) {
        goto camera_relative;
    }
    {
        SOCK &socket = WORLD->sock_sys->sock[static_cast<i8>(object->field_0x661)];
        if ((socket.flags & 0x40) != 0) {
            goto socket_relative;
        }
    }

camera_relative:
    return static_cast<u16>(pad->input_angle + GameCam->input_yaw);

socket_relative: {
    SOCK &socket = WORLD->sock_sys->sock[static_cast<i8>(object->field_0x661)];
    return static_cast<u16>(pad->input_angle + object->yrot + socket.input_yaw);
}
}

void GamePads_NetClient() {
}

void GamePads_SkipMovie() {
}

void GamePad_InitButtons() {
}

extern "C" {

    void Controller_Exit(void) {
    }

    void Controller_Init(void) {
    }

    i32 Controller_IsConnected(void) {
        return NuInputDevicePS::IsConnectedPS(1);
    }

    void Controller_Read(void) {
    }

    void Controller_Update(void) {
    }

    void InitPadPlayRecord(void) {
    }

    void PadRecPtr(void) {
    }

    void SavePadRecord(void) {
    }

    void SetPadRecPtr(void) {
    }

    i32 TestForController(void) {
        return Controller_IsConnected() != 0 || enable_touch_controls == 0;
    }

} // extern "C"

void PadOutPause(i32, WORLDINFO_s *) {
}

void ResetRumble(RUMBLEPACKET *) {
}

void UpdateRumble(RUMBLEPACKET *) {
}

void NewBuzzFrames(nupad_s *, i32, i32) {
}

void TakeHitRumble(GameObject_s *, float) {
}

void SpaceRumbleProcess() {
}

void NewRumbleAllPlayers(float, float, i32, i32) {
}

void NewStatusRumbleBuzz(i32, float, float, i32) {
}

void ObjLookingWithLeftStick(GameObject_s *) {
}

void PerformPauseButtonStuff() {
}

void VirtualControlDPad_OnDown_Callback(MechTouchUIElement &, TouchHolder &) {
}

void VirtualControlButton_OnDown_Callback(MechTouchUIElement &, TouchHolder &) {
}

void VirtualControlButtonMover_OnDown_Callback(MechTouchUIElement &, TouchHolder &) {
}

void VirtualControlDPad_LockButton_OnClick_Callback(MechTouchUIElement &, TouchHolder &) {
}

void NoPad(i32, i32) {
}

void NewBuzz(nupad_s *, float, i32) {
}

__attribute__((optimize("O2"))) i32 ReadPad(i32 port) {
    GAMEPAD_s *gamepad = &GamePad[port];
    NUPAD *pad = gamepad->pad;
    WORLDINFO_s *world = WorldInfo_CurrentlyActive();

    i32 result;
    i32 input_mode;
    if (GamePads_IgnoreInputFn != NULL && GamePads_IgnoreInputFn() != 0) {
        result = 1;
        input_mode = 1;
    } else if (readpads_always != 0 || world == NULL || world->current_level == NULL ||
               world->current_level == TITLES_LDATA || world->current_level == CREDITS_LDATA ||
               (world->current_level->flags & LEVEL_STATUS) != 0) {
        result = 0;
        input_mode = 2;
    } else if (port >= PLAYERCOUNT || Player[port] == NULL) {
        result = 1;
        input_mode = 1;
    } else if (static_cast<i8>(Player[port]->apiobj.field_0x1f8) < 0) {
        result = 0;
        input_mode = 2;
    } else {
        result = 0;
        input_mode = 3;
    }

    // These are the six fields cleared by the original before attempting a
    // low-level read. Offsets 0x0c..0x20 contain the two sticks' synthesized
    // directional held/pressed/previous masks (some legacy member names are
    // misleading, so use their offset aliases here).
    gamepad->unknown_0c = 0;
    gamepad->unknown_10 = 0;
    gamepad->unknown_18 = 0;
    gamepad->unknown_1c = 0;
    gamepad->unknown_04 = 0;
    gamepad->buttons_down_08 = 0;

    if (pad == NULL || NuPadRead(pad) == 0 || input_mode == 1) {
        return result;
    }

    if (input_mode == 3) {
        gamepad->buttons_down_08 |= pad->digital_buttons_pressed & GAMEPAD_START;
        return 3;
    }

    const f32 left_x = static_cast<f32>(pad->analog_left_x) - 127.5f;
    const f32 left_y = static_cast<f32>(pad->analog_left_y) - 127.5f;
    u32 left_directions = 0;
    if (left_x < -85.0f) {
        left_directions = GAMEPAD_DLEFT;
    } else if (left_x > 85.0f) {
        left_directions = GAMEPAD_DRIGHT;
    }
    if (left_y < -85.0f) {
        left_directions |= GAMEPAD_DUP;
    } else if (left_y > 85.0f) {
        left_directions |= GAMEPAD_DDOWN;
    }
    gamepad->unknown_0c = left_directions;
    gamepad->unknown_10 = left_directions & ~gamepad->unknown_14;
    gamepad->unknown_14 = left_directions;

    const f32 right_x = static_cast<f32>(pad->analog_right_x) - 127.5f;
    const f32 right_y = static_cast<f32>(pad->analog_right_y) - 127.5f;
    u32 right_directions = 0;
    if (right_x < -85.0f) {
        right_directions = GAMEPAD_DLEFT;
    } else if (right_x > 85.0f) {
        right_directions = GAMEPAD_DRIGHT;
    }
    if (right_y < -85.0f) {
        right_directions |= GAMEPAD_DUP;
    } else if (right_y > 85.0f) {
        right_directions |= GAMEPAD_DDOWN;
    }
    gamepad->unknown_18 = right_directions;
    gamepad->unknown_1c = right_directions & ~gamepad->unknown_20;
    gamepad->unknown_20 = right_directions;
    gamepad->input_state = 1;

    gamepad->unknown_04 = pad->digital_buttons;
    gamepad->buttons_down_08 = pad->digital_buttons_pressed;
    pActivePad = gamepad->pad;
    return 2;
}

void ReadPads() {
    ReadPad(0);
    ReadPad(1);
    readpads_always = 0;
}

void NewRumble(nupad_s *, float, i32) {
}
