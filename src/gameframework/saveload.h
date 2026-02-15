#pragma once

#include <stddef.h>

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 saveload_status;
    extern i32 saveload_autosave;
    extern i32 saveload_savepresent;

    extern i32 saveload_slotused[6];
    extern i32 saveload_slotcode[6];

    void saveloadInit(VARIPTR *buf, VARIPTR buf_end, i32, char *prodcode, char *iconname, char *unicodename, i32);

    i32 saveloadLoadSlot(i32 slot, void *buffer, usize size);

    i32 TriggerExtraDataLoad(void);

    void SaveSystemInitialise(i32 slots, void *makeSaveHash, void *save, i32 saveSize, i32 saveCount,
                              void *drawSaveIcon, void *extradata, i32 extradataSize);

    i32 ChecksumSaveData(void *buffer, i32 size);

    void saveloadASSave(i32 slot, void *buffer, i32 size, u32 hash);
    void saveloadASLoad(i32 slot, void *buffer, i32 size);
    void saveloadASDelete(i32 slot);
#ifdef __cplusplus
}
#endif
