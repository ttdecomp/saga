#pragma once

#include "decomp.h"

struct LEVELDATA_s;
struct OPTIONSSAVE_s;

extern i32 MusicOther;
extern i32 PlayersUnderAttack;
extern i32 (*CheckMusicOtherFn)(void);
extern i32 (*GameAudio_ActionMusicFn)(void);

i32 ActionMusicFn(void) __asm__("_ZL13ActionMusicFnv");
i32 CheckMusicOther(void) __asm__("_ZL15CheckMusicOtherv");

void ProcessMusicChanges(LEVELDATA_s *level, OPTIONSSAVE_s *options);
