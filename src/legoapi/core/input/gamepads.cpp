#include "legoapi/core/input/gamepads.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/NuInputDevice.h"
#include "nu2api/nucore/nupad.h"

#include <string.h>

extern "C" nupad_s **Game_NuPad;
extern GAMECAMERA_s *GameCam;
extern WORLDINFO_s *WORLD;

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

__attribute__((weak)) i32 ReadPad(i32) {
    return 0;
}

__attribute__((weak)) void ReadPads() {
    ReadPad(0);
    ReadPad(1);
    readpads_always = 0;
}

void NewRumble(nupad_s *, float, i32) {
}
