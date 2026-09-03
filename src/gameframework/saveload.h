#pragma once

#include <stddef.h>

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 saveload_status;
    extern i32 saveload_autosave;
    extern i32 saveload_savepresent;
    extern i32 saveload_slotid;
    extern i32 saveload_cardtype;
    extern i32 saveload_cardformatted;
    extern i32 saveload_freespace;
    extern i32 saveload_filecorrupt;

    extern i32 saveload_slotused[6];
    extern i32 saveload_slotcode[6];

    extern i32 SAVESLOTS;
    extern i32 SAVESIZE_ADDITIONAL;
    extern void *memcard_savedata;
    extern i32 memcard_savedatasize;
    extern void *memcard_savedatabuffer;
    extern void *memcard_extra_savedata;
    extern i32 memcard_extra_savedatasize;
    extern void *memcard_extra_savedatabuffer;
    extern void (*memcard_drawasiconfn)(void);
    extern i32 memcard_autosavestarted;
    extern f32 memcard_autosavepredelay;
    extern f32 memcard_autosavepostdelay;

    void saveloadInit(VARIPTR *buf, VARIPTR buf_end, i32, char *prodcode, char *iconname, char *unicodename, i32 unk);

    i32 saveloadLoadSlot(i32 slot, void *buffer, i32 size);
    i32 saveloadSaveSlot(i32 slot, void *buffer, usize size);

    i32 TriggerExtraDataLoad(void);
    bool TriggerExtraDataSave(void);

    void SaveSystemInitialise(i32 slots, void *makeSaveHash, void *save, i32 saveSize, i32 saveCount,
                              void (*drawSaveIcon)(void), void *extradata, i32 extradataSize);

    i32 ChecksumSaveData(void *buffer, i32 size);

    void saveloadASSave(i32 slot, void *buffer, i32 size, u32 hash);
    void saveloadASLoad(i32 slot, void *buffer, i32 size);
    void saveloadASDelete(i32 slot);
    void saveloadASCallEachFrame(void);
#ifdef __cplusplus
}
#endif
