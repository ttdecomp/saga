// BaseThing — the per-frame object interface dispatched by ThingManager.
//
// Vtable layout verified against `vtable for BaseThing` @0x66a4a0 (libTTapp.so):
// relative to the stored vptr the slots are
//   0x00 dtor (D1)          0x04 dtor (D0)
//   0x08 GetName            0x0c RemoveDependancies
//   0x10 EnterLevel(ThingLevelData)   0x14 ExitLevel(ThingLevelData)
//   0x18 Reset              0x1c Process
//   0x20 ProcessEvenWhenPaused        0x24 ProcessOnlyWhenPaused
//   0x28 Render             0x2c Display           0x30 Effects
// (the original emits a 0 slot for the pure GetName; we give it a NULL body
// so the target build needs no __cxa_pure_virtual — the slot index is the ABI).
//
// Data layout (BaseThing::BaseThing @0x425840 zeroes 0x4/0x8/0xc):
//   0x4  id matched by ThingManager::EnableActions
//   0x8  flags bitmask (0x10 process / 0x20 process-even-when-paused /
//        0x40 process-only-when-paused / 0x80 render skip bits)
//   0xc  profiling handle: when non-NULL the manager brackets the virtual
//        call with NuTimeBarSlotBegin/End
#pragma once

#include "nu2api/nucore/fixed_width.h"

struct ThingLevelData;
struct ThingRemoveData;
struct ThingProcessData;
struct ThingRenderData;
struct ThingResetData;

struct BaseThing {
    BaseThing();
    virtual ~BaseThing();
    virtual char const *GetName();
    virtual i32 RemoveDependancies(ThingRemoveData *);
    virtual void EnterLevel(ThingLevelData *);
    virtual void ExitLevel(ThingLevelData *);
    virtual void Reset(ThingResetData *);
    virtual void Process(ThingProcessData *);
    virtual void ProcessEvenWhenPaused(ThingProcessData *);
    virtual void ProcessOnlyWhenPaused(ThingProcessData *);
    virtual void Render(ThingRenderData *);
    virtual void Display(ThingRenderData *);
    virtual void Effects(ThingRenderData *);

    u32 field_0x4;
    u32 flags;
    void *profiling_0xc;
};
