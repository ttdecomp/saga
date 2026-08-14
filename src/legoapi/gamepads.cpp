#include "legoapi_types.h"

void GamePads_Init() {
}

void GamePad_Rotate(GameObject_s *) {
}

void GamePad_Waggle(GAMEPAD_s *) {
}

void GamePad_Allocate() {
}

void GamePads_NetHost() {
}

void GamePads_NetReset(i32) {
}

void GamePad_InputAngle(GameObject_s *, GAMEPAD_s *) {
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

    void Controller_IsConnected(void) {
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

    void TestForController(void) {
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

void ReadPad(i32) {
}

void ReadPads() {
}

void NewRumble(nupad_s *, float, i32) {
}
