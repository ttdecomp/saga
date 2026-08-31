// Lightweight microsecond profiling slots used by the render thread.

#include "nu2api/nu3d/android/nutimebar_plain.h"

#include "decomp.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/nutime.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>

// ---------------------------------------------------------------------------
struct TimeBarSet {
    i32 uses_buffer;
    i32 slot_count;
    i32 *accumulators[2];
    NUTIME *start_times;
    i32 *field_14;
    i32 *field_18;
    i32 *colours;
    i32 *toggle_flags;
    const char **slot_names;
    i32 field_28;
};

DECOMP_ASSERT(sizeof(TimeBarSet) == 0x2c, "TimeBarSet size");
DECOMP_ASSERT(offsetof(TimeBarSet, slot_count) == 0x04, "TimeBarSet slot count offset");
DECOMP_ASSERT(offsetof(TimeBarSet, accumulators) == 0x08, "TimeBarSet accumulator offset");
DECOMP_ASSERT(offsetof(TimeBarSet, start_times) == 0x10, "TimeBarSet start-time offset");
DECOMP_ASSERT(offsetof(TimeBarSet, toggle_flags) == 0x20, "TimeBarSet toggle offset");
DECOMP_ASSERT(offsetof(TimeBarSet, slot_names) == 0x24, "TimeBarSet name offset");

// ---------------------------------------------------------------------------
static constexpr i32 kMaxTimeBarSets = 16;
static TimeBarSet *NuTimeBar_SetList[kMaxTimeBarSets];
static NUMTL *NuTimeBar_FrameOutMtl;
static i32 NuTimeBar_Initialised;
static const u32 NuTimeBar_DefaultColours[kMaxTimeBarSets] = {
    0x00000040, 0x22222240, 0x00004440, 0x00008840, 0x44000040, 0x88000040, 0x44004440, 0x88008840,
    0x00440040, 0x00880040, 0x00444440, 0x00888840, 0x44440040, 0x88880040, 0x44444440, 0x88888840,
};

// ---------------------------------------------------------------------------
static void *timebar_allocate(VARIPTR *buffer, usize size, usize alignment) {
    if (buffer == NULL) {
        return std::calloc(1, size);
    }

    usize address = ALIGN(buffer->addr, alignment);
    buffer->addr = address + size;
    return reinterpret_cast<void *>(address);
}

template <typename T> static T *timebar_allocate_array(VARIPTR *buffer, i32 count) {
    return static_cast<T *>(timebar_allocate(buffer, static_cast<usize>(count) * sizeof(T), alignof(T)));
}

static i32 CreateTimeBar(VARIPTR *buffer, VARIPTR unused_buffer, i32 *colours, i32 slot_count) {
    (void)unused_buffer;

    TimeBarSet *timebar = timebar_allocate_array<TimeBarSet>(buffer, 1);
    std::memset(timebar, 0, sizeof(*timebar));
    timebar->uses_buffer = buffer != NULL;
    timebar->accumulators[0] = timebar_allocate_array<i32>(buffer, slot_count);
    timebar->accumulators[1] = timebar_allocate_array<i32>(buffer, slot_count);
    timebar->start_times = timebar_allocate_array<NUTIME>(buffer, slot_count);
    timebar->field_14 = timebar_allocate_array<i32>(buffer, slot_count);
    timebar->field_18 = timebar_allocate_array<i32>(buffer, slot_count);
    timebar->colours = timebar_allocate_array<i32>(buffer, slot_count);
    timebar->toggle_flags = timebar_allocate_array<i32>(buffer, slot_count);
    timebar->slot_names = timebar_allocate_array<const char *>(buffer, slot_count);

    std::memset(timebar->accumulators[0], 0, static_cast<usize>(slot_count) * sizeof(i32));
    std::memset(timebar->accumulators[1], 0, static_cast<usize>(slot_count) * sizeof(i32));
    std::memset(timebar->start_times, 0, static_cast<usize>(slot_count) * sizeof(NUTIME));
    std::memset(timebar->field_14, 0, static_cast<usize>(slot_count) * sizeof(i32));
    std::memset(timebar->field_18, 0, static_cast<usize>(slot_count) * sizeof(i32));
    std::memset(timebar->colours, 0, static_cast<usize>(slot_count) * sizeof(i32));
    std::memset(timebar->toggle_flags, 0, static_cast<usize>(slot_count) * sizeof(i32));
    std::memset(timebar->slot_names, 0, static_cast<usize>(slot_count) * sizeof(const char *));

    i32 list_index;
    for (list_index = 0; list_index < kMaxTimeBarSets; ++list_index) {
        if (NuTimeBar_SetList[list_index] == NULL) {
            NuTimeBar_SetList[list_index] = timebar;
            break;
        }
    }

    if (colours != NULL) {
        std::memcpy(timebar->colours, colours, static_cast<usize>(slot_count) * sizeof(i32));
    } else {
        std::memcpy(timebar->colours, NuTimeBar_DefaultColours, static_cast<usize>(slot_count) * sizeof(i32));
    }
    timebar->slot_count = slot_count;

    i32 set = list_index - 1;
    timebar->field_28 = set > 0;
    return set;
}

extern "C" i32 NuTimeBarCreateSetEx(VARIPTR *buffer, VARIPTR unused_buffer, i32 *colours) {
    return CreateTimeBar(buffer, unused_buffer, colours, 16);
}

extern "C" void NuTimeBarInitEx(VARIPTR *buffer, VARIPTR unused_buffer) {
    std::memset(NuTimeBar_SetList, 0, sizeof(NuTimeBar_SetList));
    CreateTimeBar(buffer, unused_buffer, NULL, 7);
    NuTimeBarCreateSetEx(buffer, unused_buffer, NULL);

    NuTimeBar_FrameOutMtl = NuMtlCreate(1);
    u8 *attributes = reinterpret_cast<u8 *>(&NuTimeBar_FrameOutMtl->attribs);
    attributes[0] = static_cast<u8>((attributes[0] & 0xf0) | 2);
    NuMtlUpdate(NuTimeBar_FrameOutMtl);
    NuTimeBar_Initialised = 1;
}

// ---------------------------------------------------------------------------
// Public API — original addresses noted per function
// ---------------------------------------------------------------------------

// original 0x2d7630
// Reset the accumulator for a single slot, or — when called as
// Reset(-1, 0) — reset every slot in set 0.  The per-slot toggle is
// flipped so the next End/SlotSet lands in the other buffer, then the
// newly-inactive accumulator is zeroed.
extern "C" void NuTimeBarSlotReset(i32 set, i32 slot) {
    if (NuTimeBar_Initialised == 0) {
        return;
    }

    if (slot == 0 && set == -1) {
        TimeBarSet *timebar = NuTimeBar_SetList[0];
        if (timebar->slot_count > 0) {
            do {
                timebar->toggle_flags[slot] ^= 1;
                timebar->accumulators[timebar->toggle_flags[slot] == 0][slot] = 0;
                ++slot;
            } while (slot < timebar->slot_count);
        }
    } else {
        TimeBarSet *timebar = NuTimeBar_SetList[set + 1];
        timebar->toggle_flags[slot] ^= 1;
        timebar->accumulators[timebar->toggle_flags[slot] == 0][slot] = 0;
    }
}

// original 0x2d76b0
// Mark the beginning of a timed region.  Records the current tick and
// the label pointer for the slot; the matching End computes the delta.
extern "C" void _NuTimeBarSlotBegin(i32 set, i32 slot, const char *name) {
    const i32 initialised = NuTimeBar_Initialised;
    if (initialised == 0) {
        return;
    }

    TimeBarSet *timebar = NuTimeBar_SetList[set + 1];
    NuTimeGet(&timebar->start_times[slot]);
    timebar->slot_names[slot] = name;
}

// original 0x2d7710
// Close the timed region opened by _NuTimeBarSlotBegin, accumulate the
// elapsed microseconds into the double-buffered slot, and return the
// updated accumulator value.  Returns 0 when profiling is disabled.
extern "C" u32 _NuTimeBarSlotEnd(i32 set, i32 slot) {
    u32 value = 0;
    if (NuTimeBar_Initialised != 0) {
        TimeBarSet *timebar = NuTimeBar_SetList[set + 1];
        NUTIME now;
        NuTimeGet(&now);

        NUTIME delta;
        NuTimeSub(&delta, &now, &timebar->start_times[slot]);
        f32 elapsed_us = NuTimeMicroSeconds(&delta);

        i32 accumulator = 1 - timebar->toggle_flags[slot];
        timebar->accumulators[accumulator][slot] += static_cast<i32>(elapsed_us);
        value = static_cast<u32>(timebar->accumulators[accumulator][slot]);
    }
    return value;
}

// original 0x2d77c0
// Directly overwrite the accumulator for a slot (used by the render
// thread to publish GPU timings that come from GL queries rather than
// CPU wall-clock).  Respects the same double-buffer selection as End.
extern "C" void NuTimeBarSlotSet(i32 set, i32 slot, i32 value) {
    if (NuTimeBar_Initialised != 0) {
        TimeBarSet *timebar = NuTimeBar_SetList[set + 1];
        i32 accumulator = 1 - timebar->toggle_flags[slot];
        timebar->accumulators[accumulator][slot] = value;
    }
}

// original 0x2d7820
// Update the display name for a slot without touching its timing.
extern "C" void NuTimeBarSlotSetName(i32 set, i32 slot, const char *name) {
    if (NuTimeBar_Initialised != 0) {
        TimeBarSet *timebar = NuTimeBar_SetList[set + 1];
        timebar->slot_names[slot] = name;
    }
}
