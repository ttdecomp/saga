#include "legoapi/core/input/gamepads.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/startup/game.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/area.h"
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
extern "C" {
    extern i32 Paused;
    extern i32 NewMode;
    extern FadeSystem FadeSys;
    extern i32 CutSceneWaiting;
}

// Original bss @0x127a500: 64 pads x 0x60 bytes.
GAMEPAD_s GamePad[64];
// Original bss @0x127a4e0.
i32 readpads_always = 0;

// Two frames of direction history for each local player.  MovePlayer shifts
// these before accepting the new input; GamePad_Rotate then reports a stable
// angular velocity only when both consecutive turns agree.
f32 PadOldSpeed2[2] = {};
f32 PadOldSpeed[2] = {};
u16 PadOldAngle2[2] = {};
u16 PadOldAngle[2] = {};

static i32 RotationDistance(i32 difference) {
    return difference < 0 ? -difference : difference;
}

void GamePads_Init() {
    Game_NuPad[0] = NuPadOpen(0, 0);
    Game_NuPad[1] = NuPadOpen(1, 0);

    for (i32 i = 0; i < 64; ++i) {
        memset(&GamePad[i], 0, sizeof(GamePad[i]));
    }
    GamePad[0].pad = Game_NuPad[0];
    GamePad[1].pad = Game_NuPad[1];
}

f32 GamePad_Rotate(GameObject_s *object) {
    i32 player_index;
    if (Player[0] == object) {
        player_index = 0;
    } else if (Player[1] == object) {
        player_index = 1;
    } else {
        return 0.0f;
    }

    GAMEPAD_s *pad = object->pad_gamepad;
    if (pad == NULL || pad->input_magnitude == 0.0f || PadOldSpeed[player_index] == 0.0f ||
        PadOldSpeed2[player_index] == 0.0f) {
        return 0.0f;
    }

    const i32 current_delta = RotDiff(PadOldAngle[player_index], pad->input_angle);
    const i32 previous_delta = RotDiff(PadOldAngle2[player_index], PadOldAngle[player_index]);
    const bool turning_clockwise = current_delta > 0 && previous_delta > 0;
    const bool turning_counter_clockwise = current_delta < 0 && previous_delta < 0;
    if ((!turning_clockwise && !turning_counter_clockwise) || RotationDistance(current_delta) >= 0x2000 ||
        RotationDistance(previous_delta) >= 0x2000) {
        return 0.0f;
    }
    return (static_cast<f32>(current_delta) / 65536.0f) / FRAMETIME;
}

i32 GamePad_Waggle(GAMEPAD_s *pad) {
    if (pad->input_magnitude != 0.0f &&
        RotationDistance(RotDiff(pad->previous_input_angle, pad->input_angle)) > 0x2000) {
        return 1;
    }
    return (pad->input_magnitude != 0.0f) != (pad->previous_input_magnitude != 0.0f);
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

u16 GamePad_InputAngle(GameObject_s *object, GAMEPAD_s *pad) {
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

void PadOutPause(i32 port, WORLDINFO_s *world) {
    if (Paused != 0 || NoPad(port, 1) == 0 || NewMode != 0 || NewLData != NULL || FadeSys.fade != 0.0f) {
        return;
    }
    if (GamePads_IgnoreInputFn != NULL && GamePads_IgnoreInputFn() != 0) {
        return;
    }
    if (MiniCutCam != 0 || (world->area != NULL && (world->area->flags & AREAFLAG_ENDING_AREA) != 0) ||
        (world->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) != 0 || GetMenuID() != -1) {
        return;
    }
    if (CUTSTOPGAME != 0 && !CutScene_IsSkippable(static_cast<CUTINFO *>(CutStopInfo))) {
        return;
    }
    if (CutSceneWaiting == 0) {
        PauseGame(port);
    }
}

void ResetRumble(RUMBLEPACKET *packet) {
    packet->rumble_time = 0.0f;
    packet->rumble_amount = 0.0f;
    packet->active = 0;
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

i32 ObjLookingWithLeftStick(GameObject_s *object) {
    if (object->character_id_0x7a5 == 0x4d) {
        return object->field_0x7a3 == 0 ? 2 : 1;
    }
    return object->character_id_0x7a5 == 0x52 ? 2 : 1;
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

i32 NoPad(i32 port, i32 require_game_input) {
    const i32 menu_id = GetMenuID();
    if ((require_game_input != 0 && GamePad[port].input_state == 0) || GamePad[port].pad->is_valid != 0) {
        return 0;
    }
    if (LEGOMENU_NEWGAME != -1 && menu_id == LEGOMENU_NEWGAME) {
        return 0;
    }
    if (LEGOMENU_CREDITS != -1 && menu_id == LEGOMENU_CREDITS) {
        return 0;
    }
    return 1;
}

void NewBuzz(nupad_s *, float, i32) {
}

i32 ReadPad(i32 port) {
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
