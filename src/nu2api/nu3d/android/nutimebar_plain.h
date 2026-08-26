// NuTimeBar profiling slot API — see nutimebar_plain.cpp.
#pragma once

#include "decomp.h"

void NuTimeBarSlotReset(i32 set, i32 slot);
void _NuTimeBarSlotBegin(i32 set, i32 slot, const char *name);
u32 _NuTimeBarSlotEnd(i32 set, i32 slot);
void NuTimeBarSlotSet(i32 set, i32 slot, i32 value);
void NuTimeBarSlotSetName(i32 set, i32 slot, const char *name);
