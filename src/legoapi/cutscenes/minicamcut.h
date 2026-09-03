#pragma once

#include "decomp.h"

struct MINICAMCOMMAND_s {
    i32 state_words[7];
};
DECOMP_ASSERT(sizeof(MINICAMCOMMAND_s) == 0x1c, "MINICAMCOMMAND_s size");

struct MINICAM_s {
    MINICAMCOMMAND_s commands[32];
    u8 command_count;
    u8 current_command;
    u8 flags[2];
    i32 command_state[27];
    f32 delta_time;
    i32 field_0x3f4;
    i32 field_0x3f8;
};
DECOMP_ASSERT(sizeof(MINICAM_s) == 0x3fc, "MINICAM_s size");
DECOMP_ASSERT(offsetof(MINICAM_s, command_count) == 0x380, "MINICAM command count offset");
DECOMP_ASSERT(offsetof(MINICAM_s, current_command) == 0x381, "MINICAM command index offset");
DECOMP_ASSERT(offsetof(MINICAM_s, delta_time) == 0x3f0, "MINICAM delta time offset");

extern MINICAM_s MiniCam;
