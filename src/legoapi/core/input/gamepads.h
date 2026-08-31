#pragma once

#include "decomp.h"

struct GAMEPAD_s;
struct GameObject_s;

// Gamepad system (module legoapi/core/input, gamepads.cpp).

void GamePads_Init();
GAMEPAD_s *GamePad_Allocate();
u16 GamePad_InputAngle(GameObject_s *object, GAMEPAD_s *pad);
f32 GamePad_Rotate(GameObject_s *object);
i32 GamePad_Waggle(GAMEPAD_s *pad);
i32 ReadPad(i32 port);
void ReadPads();

extern "C" i32 Controller_IsConnected();
extern "C" i32 TestForController();

extern f32 PadOldSpeed2[2];
extern f32 PadOldSpeed[2];
extern u16 PadOldAngle2[2];
extern u16 PadOldAngle[2];
