#pragma once

#include "decomp.h"

struct GAMEPAD_s;

// Gamepad system (module legoapi/core/input, gamepads.cpp).

void GamePads_Init();
GAMEPAD_s *GamePad_Allocate();
i32 ReadPad(i32 port);
void ReadPads();

extern "C" i32 Controller_IsConnected();
extern "C" i32 TestForController();
