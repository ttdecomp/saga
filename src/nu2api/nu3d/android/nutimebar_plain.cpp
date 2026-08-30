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
// Built by NuTimeBarCreateSet.  Offsets are expressed here as they appear
// in the original binary: the transcribed code indexes an i32* record with
// word offsets (rec+4, rec+0x10, rec+0x20 …) while the accumulator
// selectors at +0x08/+0x0C are addressed as byte offsets.  The struct below
// mirrors that exact byte layout so a reinterpret_cast from the opaque
// void* in the set table reaches the same fields without manual
// usize arithmetic.
//
//   +0x08  i32*  accumulators[0]   \
//   +0x0C  i32*  accumulators[1]   / double-buffered, selected by toggle
//   +0x10  i32   slot_count
//   +0x40  NUTIME* start_times      // word offset 0x10 == byte 0x40
//   +0x80  u32*  toggle_flags       // word offset 0x20 == byte 0x80
//   +0x90  const char** slot_names  // word offset 0x24 == byte 0x90
//
struct TimeBarSet {
    uint8_t pad_00[0x08];
    uint32_t accumulators[2]; // +0x08 as u32 handles (32-bit pointers on device)
    int32_t slot_count;       // +0x10
    uint8_t pad_14[0x2C];     // +0x14 .. +0x3F
    uint32_t start_times;     // +0x40 as u32 handle (rec+0x10 words)
    uint8_t pad_44[0x3C];     // +0x44 .. +0x7F
    uint32_t toggle_flags;    // +0x80 as u32 handle (rec+0x20 words)
    uint8_t pad_84[0x0C];     // +0x84 .. +0x8F
    uint32_t slot_names;      // +0x90 as u32 handle (rec+0x24 words)
};

static_assert(offsetof(TimeBarSet, accumulators) == 0x08, "accum offset");
static_assert(offsetof(TimeBarSet, slot_count) == 0x10, "slot_count offset");
static_assert(offsetof(TimeBarSet, start_times) == 0x40, "start_times offset");
static_assert(offsetof(TimeBarSet, toggle_flags) == 0x80, "toggle offset");
static_assert(offsetof(TimeBarSet, slot_names) == 0x90, "names offset");

// ---------------------------------------------------------------------------
// Global state (bss in the original)

static int32_t s_timebar_initialised = 0;

// Table of live sets.  Index 0 is the first real set; callers pass
// set==-1 to mean "set 0, broadcast to every slot" (only used by Reset).
// All other callers use set+1 to skip that broadcast alias, matching the
// original dispatch.
static constexpr int32_t kMaxSets = 64;
static void *s_set_table[kMaxSets] = {};

// ---------------------------------------------------------------------------
// Helpers

static inline TimeBarSet *get_set(int32_t set) {
    // set is the public index (-1 == broadcast alias for set 0).  Normal
    // slots address s_set_table[set+1].
    return static_cast<TimeBarSet *>(s_set_table[set + 1]);
}

static inline TimeBarSet *get_set_zero() {
    return static_cast<TimeBarSet *>(s_set_table[0]);
}

static inline int32_t *accumulator_for_slot(TimeBarSet *rec, int32_t slot) {
    // The active accumulator is the one *not* currently selected by the
    // toggle — the original toggles first, then writes the opposite buffer
    // so a concurrent reader sees a stable value.
    auto *toggle = reinterpret_cast<uint32_t *>(rec->toggle_flags);
    const uint32_t flag = toggle[slot] & 1u;
    const int buffer = 1 - static_cast<int>(flag);
    auto *acc = reinterpret_cast<int32_t *>(rec->accumulators[buffer]);
    return &acc[slot];
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
    if (s_timebar_initialised == 0) {
        return;
    }

    // Broadcast reset: NuRenderThread tail calls Reset(-1, 0..1) for the
    // GPU/CPU summary slots.  The original tests slot==0 && set==-1 and
    // then iterates the whole set.
    if (slot == 0 && set == -1) {
        TimeBarSet *rec = get_set_zero();
        if (rec == nullptr) {
            return;
        }
        const int32_t slot_count = rec->slot_count;
        for (int32_t i = 0; i < slot_count; ++i) {
            reinterpret_cast<uint32_t *>(rec->toggle_flags)[i] ^= 1u;
            *accumulator_for_slot(rec, i) = 0;
        }
        return;
    }

    TimeBarSet *rec = get_set(set);
    if (rec == nullptr) {
        return;
    }
    reinterpret_cast<uint32_t *>(rec->toggle_flags)[slot] ^= 1u;
    *accumulator_for_slot(rec, slot) = 0;
}

// original 0x2d76b0
// Mark the beginning of a timed region.  Records the current tick and
// the label pointer for the slot; the matching End computes the delta.
extern "C" void _NuTimeBarSlotBegin(int32_t set, int32_t slot, const char *name) {
    if (s_timebar_initialised == 0) {
        return;
    }
    TimeBarSet *rec = get_set(set);
    if (rec == nullptr) {
        return;
    }
    NuTimeGet(&reinterpret_cast<NUTIME *>(rec->start_times)[slot]);
    reinterpret_cast<const char **>(rec->slot_names)[slot] = name;
}

// original 0x2d7710
// Close the timed region opened by _NuTimeBarSlotBegin, accumulate the
// elapsed microseconds into the double-buffered slot, and return the
// updated accumulator value.  Returns 0 when profiling is disabled.
extern "C" uint32_t _NuTimeBarSlotEnd(int32_t set, int32_t slot) {
    if (s_timebar_initialised == 0) {
        return 0;
    }

    TimeBarSet *rec = get_set(set);
    if (rec == nullptr) {
        return 0;
    }

    NUTIME now;
    NuTimeGet(&now);

    NUTIME delta;
    NuTimeSub(&delta, &now, &reinterpret_cast<NUTIME *>(rec->start_times)[slot]);

    const float elapsed_us = NuTimeMicroSeconds(&delta);

    reinterpret_cast<uint32_t *>(rec->toggle_flags)[slot] ^= 1u;
    int32_t *acc = accumulator_for_slot(rec, slot);
    *acc += static_cast<int32_t>(elapsed_us);
    return static_cast<uint32_t>(*acc);
}

// original 0x2d77c0
// Directly overwrite the accumulator for a slot (used by the render
// thread to publish GPU timings that come from GL queries rather than
// CPU wall-clock).  Respects the same double-buffer selection as End.
extern "C" void NuTimeBarSlotSet(int32_t set, int32_t slot, int32_t value) {
    if (s_timebar_initialised == 0) {
        return;
    }
    TimeBarSet *rec = get_set(set);
    if (rec == nullptr) {
        return;
    }
    *accumulator_for_slot(rec, slot) = value;
}

// original 0x2d7820
// Update the display name for a slot without touching its timing.
extern "C" void NuTimeBarSlotSetName(int32_t set, int32_t slot, const char *name) {
    if (s_timebar_initialised == 0) {
        return;
    }
    TimeBarSet *rec = get_set(set);
    if (rec == nullptr) {
        return;
    }
    reinterpret_cast<const char **>(rec->slot_names)[slot] = name;
}
