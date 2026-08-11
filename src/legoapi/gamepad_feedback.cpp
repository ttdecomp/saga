#include "legoapi/gamepads.h"

i32 RotDiff(u16 first, u16 second) {
    return (i16)(second - first);
}

void NewBuzzFrames(NUPAD *, i32, i32) {}

void NewRumbleAllPlayers(f32, f32, i32, i32) {}
