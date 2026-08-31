#pragma once

#include "decomp.h"

struct GAMEPAD_s;
struct GameObject_s;

// Gamepad system (module legoapi/core/input, gamepads.cpp).

void GamePads_Init();
GAMEPAD_s *GamePad_Allocate();
u16 GamePad_InputAngle(GameObject_s *object, GAMEPAD_s *pad);
i32 ReadPad(i32 port);
void ReadPads();

extern "C" i32 Controller_IsConnected();
extern "C" i32 TestForController();
