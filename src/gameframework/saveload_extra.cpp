#include "gameframework/saveload.h"

#include <string.h>

bool TriggerExtraDataSave(void) {
    memmove(memcard_extra_savedatabuffer, memcard_extra_savedata, memcard_extra_savedatasize);

    i32 checksum = ChecksumSaveData(memcard_extra_savedatabuffer, memcard_extra_savedatasize);
    *(i32 *)((char *)memcard_extra_savedatabuffer + memcard_extra_savedatasize) = checksum;

    i32 save_error = saveloadSaveSlot(SAVESLOTS, memcard_extra_savedatabuffer, memcard_extra_savedatasize + 4);
    return save_error != 0;
}

i32 TriggerExtraDataLoad(void) {
    if (saveloadLoadSlot(SAVESLOTS, memcard_extra_savedatabuffer, memcard_extra_savedatasize + 4) != 0) {
        if (ChecksumSaveData(memcard_extra_savedatabuffer, memcard_extra_savedatasize) ==
            *(i32 *)((usize)memcard_extra_savedatabuffer + memcard_extra_savedatasize)) {
            memmove(memcard_extra_savedata, memcard_extra_savedatabuffer, memcard_extra_savedatasize);
            return 1;
        }
    }

    return 0;
}
