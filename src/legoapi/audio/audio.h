#pragma once

#include "decomp.h"

struct LEVELDATA_s;
struct OPTIONSSAVE_s;

struct MUSICSTATE_s {
    u16 mode;
    i16 queued_track;
    u16 active_stream;
    i16 cutscene_track;
    u16 field_08;
    i16 previous_track;
    u32 field_0c;
    u8 preseek_pending;
    u8 field_11;
    u8 stream_delays[2];
    i32 preseek_start_point;
    f32 volume_scale;
    u8 alternate_active;
    u8 field_1d;
    i16 alternate_track;
    u32 field_20;
};
DECOMP_ASSERT(sizeof(MUSICSTATE_s) == 0x24, "MUSICSTATE_s size");

extern "C" MUSICSTATE_s Music;

extern i32 MusicOther;
extern i32 PlayersUnderAttack;
extern i32 (*CheckMusicOtherFn)(void);
extern i32 (*GameAudio_ActionMusicFn)(void);

i32 ActionMusicFn(void) __asm__("_ZL13ActionMusicFnv");
i32 CheckMusicOther(void) __asm__("_ZL15CheckMusicOtherv");

void ProcessMusicChanges(LEVELDATA_s *level, OPTIONSSAVE_s *options);
