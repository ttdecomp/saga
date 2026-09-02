// Lightweight microsecond profiling slots used by the render thread.
//
// The original game created one or more "time-bar sets" via
// NuTimeBarInit / NuTimeBarCreateSet when profiling was enabled.  Each
// set owns N slots; every slot has:
//   - a start timestamp  (NUTIME, written by _NuTimeBarSlotBegin)
//   - a name pointer     (written by Begin / SlotSetName)
//   - double-buffered microsecond accumulators so the HUD can read one
//     buffer while the profiled code writes the other.
//   - a per-slot toggle flag that selects which accumulator is active.
//
// All entry points below no-op unless a set has been created — the host
// build never creates one (see target_android.cpp — the four
// NuTimeBarCreateSet calls are commented out), so NuTimeBar_Initialised
// stays 0 and the render thread's profiling calls are free.  The bodies
// are transcribed from the original x86 lib at the addresses noted per
// function and keep the original double-buffer / toggle semantics so
// enabling profiling in the future behaves identically to device.

#include "nu2api/nu3d/android/nutimebar_plain.h"

#include "decomp.h"
#include "nu2api/nucore/nutime.h"

#include <cstddef>
#include <cstdint>

// ---------------------------------------------------------------------------
// Set record layout
//
struct TimeBarSet {
    i32 field_00;
    i32 slot_count;
    i32 *accumulators[2];
    NUTIME *start_times;
    i32 field_14[3];
    u32 *toggle_flags;
    const char **slot_names;
};

static_assert(offsetof(TimeBarSet, slot_count) == 0x04, "slot_count offset");
static_assert(offsetof(TimeBarSet, accumulators) == 0x08, "accum offset");
static_assert(offsetof(TimeBarSet, start_times) == 0x10, "start_times offset");
static_assert(offsetof(TimeBarSet, toggle_flags) == 0x20, "toggle offset");
static_assert(offsetof(TimeBarSet, slot_names) == 0x24, "names offset");

// ---------------------------------------------------------------------------
// Global state (bss in the original)

static TimeBarSet *NuTimeBar_SetList[16] asm("_ZL17NuTimeBar_SetList");
static i32 NuTimeBar_Initialised asm("_ZL21NuTimeBar_Initialised");

// Table of live sets.  Index 0 is the first real set; callers pass
// set==-1 to mean "set 0, broadcast to every slot" (only used by Reset).
// All other callers use set+1 to skip that broadcast alias, matching the
// original dispatch.
// ---------------------------------------------------------------------------
// Helpers

static inline TimeBarSet *get_set(int32_t set) {
    return NuTimeBar_SetList[set];
}

static inline int32_t *accumulator_for_slot(TimeBarSet *rec, int32_t slot) {
    // The active accumulator is the one *not* currently selected by the
    // toggle — the original toggles first, then writes the opposite buffer
    // so a concurrent reader sees a stable value.
    return &rec->accumulators[1 - (rec->toggle_flags[slot] & 1)][slot];
}

// ---------------------------------------------------------------------------
// Public API — original addresses noted per function
// ---------------------------------------------------------------------------

// original 0x2d7630
// Reset the accumulator for a single slot, or — when called as
// Reset(-1, 0) — reset every slot in set 0.  The per-slot toggle is
// flipped so the next End/SlotSet lands in the other buffer, then the
// newly-inactive accumulator is zeroed.
extern "C" void NuTimeBarSlotReset(int32_t set, int32_t slot) {
    if (NuTimeBar_Initialised == 0) {
        return;
    }

    // Broadcast reset: NuRenderThread tail calls Reset(-1, 0..1) for the
    // GPU/CPU summary slots.  The original tests slot==0 && set==-1 and
    // then iterates the whole set.
    if (slot == 0 && set == -1) {
        TimeBarSet *rec = get_set(0);
        if (rec->slot_count > 0) {
            u32 *toggle_flags = rec->toggle_flags;
            int32_t i = 0;
            do {
                const u32 toggle = toggle_flags[i] ^= 1;
                rec->accumulators[toggle == 0][i] = 0;
            } while (++i < rec->slot_count);
        }
        return;
    }

    TimeBarSet *rec = get_set(set);
    const u32 toggle = rec->toggle_flags[slot] ^= 1;
    rec->accumulators[toggle == 0][slot] = 0;
}

// original 0x2d76b0
// Mark the beginning of a timed region.  Records the current tick and
// the label pointer for the slot; the matching End computes the delta.
extern "C" void _NuTimeBarSlotBegin(int32_t set, int32_t slot, const char *name) {
    if (NuTimeBar_Initialised == 0) {
        return;
    }
    TimeBarSet *rec = get_set(set);
    NuTimeGet(&rec->start_times[slot]);
    rec->slot_names[slot] = name;
}

// original 0x2d7710
// Close the timed region opened by _NuTimeBarSlotBegin, accumulate the
// elapsed microseconds into the double-buffered slot, and return the
// updated accumulator value.  Returns 0 when profiling is disabled.
extern "C" uint32_t _NuTimeBarSlotEnd(int32_t set, int32_t slot) {
    if (NuTimeBar_Initialised == 0) {
        return 0;
    }

    TimeBarSet *rec = get_set(set);

    NUTIME now;
    NuTimeGet(&now);

    NUTIME delta;
    NuTimeSub(&delta, &now, &rec->start_times[slot]);

    const float elapsed_us = NuTimeMicroSeconds(&delta);

    rec->toggle_flags[slot] ^= 1;
    int32_t *acc = accumulator_for_slot(rec, slot);
    *acc += static_cast<int32_t>(elapsed_us);
    return static_cast<uint32_t>(*acc);
}

// original 0x2d77c0
// Directly overwrite the accumulator for a slot (used by the render
// thread to publish GPU timings that come from GL queries rather than
// CPU wall-clock).  Respects the same double-buffer selection as End.
extern "C" void NuTimeBarSlotSet(int32_t set, int32_t slot, int32_t value) {
    if (NuTimeBar_Initialised == 0) {
        return;
    }
    TimeBarSet *rec = get_set(set);
    *accumulator_for_slot(rec, slot) = value;
}

// original 0x2d7820
// Update the display name for a slot without touching its timing.
extern "C" void NuTimeBarSlotSetName(int32_t set, int32_t slot, const char *name) {
    if (NuTimeBar_Initialised == 0) {
        return;
    }
    TimeBarSet *rec = get_set(set);
    rec->slot_names[slot] = name;
}
