#include <string.h>

#include "globals.h"
#include "legoapi/cutscenes/minicamcut.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

MINICAM_s MiniCam = {};

void Minicam_Update() {
}

void Minicam_AddDeltas(float) {
}

void Minicam_AddCommand(i32, float, i32, void *, nuvec_s) {
}

void Minicam_CalcCamPos() {
}

void Minicam_InitSystem() {
}

void Minicam_ClearDeltas() {
}

void Minicam_ResetForNewCut() {
    memset(&MiniCam, 0, sizeof(MiniCam));
}

void Minicam_ResetForNextCommand() {
    Minicam_ClearDeltas();

    const i32 current_command = MiniCam.current_command;
    const i32 command_count = MiniCam.command_count;
    const i32 next_command = current_command + 1;
    i32 remaining_commands;
    if (next_command < command_count) {
        MINICAMCOMMAND_s *source = &MiniCam.commands[next_command];
        MINICAMCOMMAND_s *destination = MiniCam.commands;
        MINICAMCOMMAND_s *const command_end = &MiniCam.commands[command_count];
        while (source != command_end) {
            *destination++ = *source++;
        }
        remaining_commands = command_count - next_command;
    } else {
        remaining_commands = 0;
    }

    MiniCam.command_count = static_cast<u8>(remaining_commands);
    memset(&MiniCam.commands[remaining_commands], 0,
           static_cast<usize>(32 - remaining_commands) * sizeof(MINICAMCOMMAND_s));
    MiniCam.current_command = 0;
    MiniCam.delta_time = 0.0f;
}

void MiniCam_ChangeMode(i32) {
}
