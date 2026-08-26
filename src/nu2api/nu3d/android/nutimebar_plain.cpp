// NuTimeBar profiling slot API — transcribed originals.
//
//   NuTimeBarSlotReset   original 0x2d7630
//   _NuTimeBarSlotBegin  original 0x2d76b0
//   _NuTimeBarSlotEnd    original 0x2d7710
//   NuTimeBarSlotSet     original 0x2d77c0
//   NuTimeBarSlotSetName original 0x2d7820
//
// All of these no-op unless a timebar set has been created via the
// NuTimeBarInit/CreateSet family, exactly like on device (the game only
// creates them when profiling is enabled).

#include "decomp.h"
#include "nu2api/nucore/nutime.h"

static i32 NuTimeBar_Initialised;   // bss
static void *NuTimeBar_SetList[64]; // set record table (bss)

// Per-set record (built by NuTimeBarCreateSet): slot name array @+0x10
// (NUTIME pairs), start times @+0x10, double-buffered accumulators @+8/+0xc
// selected by the per-slot toggle flag array @+0x20, names @+0x24.
void NuTimeBarSlotReset(i32 set, i32 slot) {
    u32 *flag;
    i32 i;
    i32 slots;
    i32 *rec;

    if (NuTimeBar_Initialised != 0) {
        if (slot == 0 && set == -1) {
            rec = (i32 *)NuTimeBar_SetList[0];
            slots = *(i32 *)(usize)(rec + 4);
            if (0 < slots) {
                u32 *flags = (u32 *)(usize) * (i32 *)(usize)(rec + 0x20);
                for (i = 0; i < slots; i++) {
                    flags[i] ^= 1;
                    *(i32 *)(usize)(*(i32 *)(usize)((usize)rec + 8 + (usize)(flags[i] == 0) * 4) + i * 4) = 0;
                }
            }
        } else {
            rec = (i32 *)NuTimeBar_SetList[set + 1];
            flag = (u32 *)(usize)(*(i32 *)(usize)(rec + 0x20) + slot * 4);
            *flag ^= 1;
            *(i32 *)(usize)(*(i32 *)(usize)((usize)rec + 8 + (usize)(*flag == 0) * 4) + slot * 4) = 0;
        }
    }
}

void _NuTimeBarSlotBegin(i32 set, i32 slot, const char *name) {
    if (NuTimeBar_Initialised != 0) {
        i32 *rec = (i32 *)NuTimeBar_SetList[set + 1];
        NuTimeGet((NUTIME *)(usize)(*(i32 *)(usize)(rec + 0x10) + slot * 8));
        *(const char **)(usize)(*(i32 *)(usize)(rec + 0x24) + slot * 4) = name;
    }
}

u32 _NuTimeBarSlotEnd(i32 set, i32 slot) {
    NUTIME now;
    NUTIME delta;

    if (NuTimeBar_Initialised == 0) {
        return 0;
    }
    i32 *rec = (i32 *)NuTimeBar_SetList[set + 1];
    NuTimeGet(&now);
    NuTimeSub(&delta, &now, (NUTIME *)(usize)(*(i32 *)(usize)(rec + 0x10) + slot * 8));
    f32 us = NuTimeMicroSeconds(&delta);
    u32 *flags = (u32 *)(usize) * (i32 *)(usize)(rec + 0x20);
    flags[slot] ^= 1;
    i32 *acc = (i32 *)(usize)(*(i32 *)(usize)((usize)rec + 8 + (usize)(1 - (flags[slot] & 1)) * 4) + slot * 4);
    *acc += (i32)us;
    return (u32)*acc;
}

void NuTimeBarSlotSet(i32 set, i32 slot, i32 value) {
    if (NuTimeBar_Initialised != 0) {
        i32 *rec = (i32 *)NuTimeBar_SetList[set + 1];
        i32 *flags = (i32 *)(usize) * (i32 *)(usize)(rec + 0x20);
        *(i32 *)(usize)(*(i32 *)(usize)((usize)rec + 8 + (usize)(1 - (flags[slot] & 1)) * 4) + slot * 4) = value;
    }
}

void NuTimeBarSlotSetName(i32 set, i32 slot, const char *name) {
    if (NuTimeBar_Initialised != 0) {
        i32 *rec = (i32 *)NuTimeBar_SetList[set + 1];
        *(const char **)(usize)(*(i32 *)(usize)(rec + 0x24) + slot * 4) = name;
    }
}
