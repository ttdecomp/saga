// Time-bar profiling slots — lightweight microsecond stopwatches.
//
// Each "set" is a fixed-size array of slots created by NuTimeBarCreateSet
// (only when profiling is enabled).  The render thread uses set -1
// (aliased to set 0) to publish CPU/GPU timings for the on-screen HUD.
// All calls no-op when no set has been created.  See nutimebar_plain.cpp
// for the double-buffered accumulator layout.
#pragma once

#include "decomp.h"

void NuTimeBarSlotReset(int32_t set, int32_t slot);
void _NuTimeBarSlotBegin(int32_t set, int32_t slot, const char *name);
uint32_t _NuTimeBarSlotEnd(int32_t set, int32_t slot);
void NuTimeBarSlotSet(int32_t set, int32_t slot, int32_t value);
void NuTimeBarSlotSetName(int32_t set, int32_t slot, const char *name);
