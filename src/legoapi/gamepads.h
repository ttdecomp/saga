#pragma once

#include "legoapi/gameobject.h"
#include "nu2api/nucore/nupad.h"

struct GAMEPAD_s {
    NUPAD *pad;
    u8 unknown_04[0x22];
    u16 direction;
    f32 speed;
};

#if UINTPTR_MAX != UINT64_MAX
static_assert(offsetof(GAMEPAD_s, direction) == 0x26, "GAMEPAD_s direction offset");
static_assert(offsetof(GAMEPAD_s, speed) == 0x28, "GAMEPAD_s speed offset");
#endif

extern f32 PadOldSpeed2[2];
extern f32 PadOldSpeed[2];
extern u16 PadOldAngle2[2];
extern u16 PadOldAngle[2];

f32 GamePad_Rotate(GameObject_s *object);
void DieRumble(GameObject_s *object);
void KillRumble(GameObject_s *object);
void HitRumble(GameObject_s *object);
void TakeHitRumble(GameObject_s *object, f32 strength);
void FloatRumble(GameObject_s *object);
void ConstantRumble(GameObject_s *object, f32 strength, f32 time_offset);
u16 GamePad_InputAngle(GameObject_s *object, GAMEPAD_s *game_pad);
void NewBuzz(NUPAD *pad, f32 strength, i32 frames);
void NewRumble(NUPAD *pad, f32 strength, i32 frames);
void NewRumbleAllPlayers(f32 strength, f32 buzz, i32 rumble_frames, i32 buzz_frames);
