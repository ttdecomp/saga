#include "gameframework/saveload.h"

#include "nu2api/nucore/numemory.h"

typedef i16 (*hashfn_t)(void);

extern i32 SAVESLOTS;
extern void *memcard_savedata;
extern i32 memcard_savedatasize;
extern void *memcard_savedatabuffer;
extern void *memcard_extra_savedata;
extern i32 memcard_extra_savedatasize;
extern void *memcard_extra_savedatabuffer;
extern hashfn_t memcard_hashfn;
extern void *memcard_drawasiconfn;
extern i32 memcard_autosave;

void SaveSystemInitialise(i32 slots, void *makeSaveHash, void *save, i32 saveSize, i32 saveCount, void *drawSaveIcon,
                          void *extradata, i32 extradataSize) {
    i32 saveSlots;
    if (extradata == NULL) {
        saveSlots = 6;
        if (slots < 7) {
            saveSlots = slots;
        }
    } else {
        saveSlots = 5;
        if (slots < 6) {
            saveSlots = slots;
        }
    }
    SAVESLOTS = saveSlots;

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
