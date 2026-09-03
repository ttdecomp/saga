#include <string.h>

#include "decomp.h"

#include "gameframework/saveload.h"

#include "nu2api/nucore/numemory.h"

typedef i16 (*hashfn_t)(void);

i32 SAVESLOTS = 3;

void *memcard_savedata = NULL;
i32 memcard_savedatasize = 0;
void *memcard_savedatabuffer = NULL;

void *memcard_extra_savedata = NULL;
i32 memcard_extra_savedatasize = 0;
void *memcard_extra_savedatabuffer = NULL;

hashfn_t memcard_hashfn = NULL;
void (*memcard_drawasiconfn)(void) = NULL;
i32 memcard_autosave = 0;
i32 memcard_autosavestarted = 0;
f32 memcard_autosavepredelay = 0.0f;
f32 memcard_autosavepostdelay = 0.0f;

void SaveSystemInitialise(i32 slots, void *makeSaveHash, void *save, i32 saveSize, i32 saveCount,
                          void (*drawSaveIcon)(void), void *extradata, i32 extradataSize) {
    i32 saveslots;
    if (extradata == NULL) {
        saveslots = 6;
        if (slots < 7) {
            saveslots = slots;
        }
    } else {
        saveslots = 5;
        if (slots < 6) {
            saveslots = slots;
        }
    }
    SAVESLOTS = saveslots;

    memcard_hashfn = (hashfn_t)makeSaveHash;
    memcard_savedata = save;
    memcard_savedatasize = saveSize;
    memcard_extra_savedata = extradata;
    memcard_extra_savedatasize = extradataSize;

    memcard_savedatabuffer = NU_ALLOC(saveSize + 4, 4, 1, "", NUMEMORY_CATEGORY_NONE);
    memcard_extra_savedatabuffer = NU_ALLOC(extradataSize + 4, 4, 1, "", NUMEMORY_CATEGORY_NONE);

    memcard_autosave = saveCount;
    memcard_drawasiconfn = drawSaveIcon;
}

i32 ChecksumSaveData(void *buffer, i32 size) {
    i32 n = size / 4;

    i32 sum = 0x5c0999;

    for (i32 i = 0; i < n; i++) {
        sum += ((i32 *)buffer)[i];
    }

    return sum;
}

bool TriggerExtraDataSave(void) {
    memmove(memcard_extra_savedatabuffer, memcard_extra_savedata, memcard_extra_savedatasize);

    i32 checksum = ChecksumSaveData(memcard_extra_savedatabuffer, memcard_extra_savedatasize);
    *(i32 *)((char *)memcard_extra_savedatabuffer + memcard_extra_savedatasize) = checksum;

    i32 save_error = saveloadSaveSlot(SAVESLOTS, memcard_extra_savedatabuffer, memcard_extra_savedatasize + 4);
    return save_error != 0;
}

i32 TriggerExtraDataLoad(void) {
    if (saveloadLoadSlot(SAVESLOTS, memcard_extra_savedatabuffer, memcard_extra_savedatasize + 4) != 0) {
        i32 correct = ChecksumSaveData(memcard_extra_savedatabuffer, memcard_extra_savedatasize);
        i32 checksum = *(i32 *)((char *)memcard_extra_savedatabuffer + memcard_extra_savedatasize);
        LOG_DEBUG("checksum=%08X, correct=%08X", checksum, correct);
        if (correct == checksum) {
            memmove(memcard_extra_savedata, memcard_extra_savedatabuffer, memcard_extra_savedatasize);
            return 1;
        }
    }

    return 0;
}
