#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "decomp.h"

#include "saveload/saveload.h"

#include "nu2api.saga/nuandroid/nuios.h"
#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nucore/nutime.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include "export.h"

i32 saveload_status = 0;
i32 saveload_autosave = 0;
i32 saveload_savepresent = 0;

i32 saveload_slotused[6] = {0};
i32 saveload_slotcode[6] = {0};

char *slotname(i32 index) {
    static char name[4096];
    sprintf(name, "SaveGame%d.%s_SavedGame", index, "LEGO Star Wars - The Complete Saga");
    return name;
}

char *slotfolder(i32 index) {
    static char name[4096];

    char *path = NuIOS_GetDocumentsPath();
    NuStrCpy(name, path);

    i32 len = strlen(name);

    strcpy(&name[len], "SavedGames");

    return name;
}

char *fullslotname(i32 index) {
    static char name[4096];

    char *folder = slotfolder(index);
    strcpy(name, folder);
    strcat(name, "/");
    strcat(name, slotname(index));

    return name;
}

static i32 saveload_getinfo(void) {
    saveload_savepresent = 0;
    i32 count = 0;

    for (i32 i = 0; i < 6; i = i + 1) {
        saveload_slotused[i] = 0;
        saveload_slotcode[i] = 0;

        char *path = fullslotname(i);
        FILE *file = fopen(path, "rb");

        if (file != NULL) {
            count = count + 1;
            saveload_slotused[i] = 1;

            fseek(file, -4, 2);
            fread(&saveload_slotcode[i], 4, 1, file);

            saveload_savepresent = 1;

            fclose(file);
        }
    }

    return count;
}

void saveloadInit(VARIPTR *buf, VARIPTR buf_end, i32, char *prodcode, char *iconname, char *unicodename, i32) {
    saveload_getinfo();
    saveload_status = 1;
    saveload_autosave = -1;
}

i32 saveloadLoadSlot(i32 slot, void *buffer, usize size) {
    char *filename = fullslotname(slot);
    FILE *file = fopen(filename, "rb");

    LOG_DEBUG("slot=%d, filename=%s, file=%p", slot, filename, file);

    if (file == NULL) {
        return 0;
    } else {
        SaveLoad buf;

        fread(&buf, 0x2028, 1, file);
        if (buf.extradata_offset != 0) {
            fseek(file, buf.extradata_offset, SEEK_CUR);
        }

        fread(buffer, size, 1, file);
        fclose(file);

        return 1;
    }
}

typedef i16 (*hashfn_t)(void);

i32 SAVESLOTS = 3;

void *memcard_savedata = NULL;
i32 memcard_savedatasize = 0;
void *memcard_savedatabuffer = NULL;

void *memcard_extra_savedata = NULL;
i32 memcard_extra_savedatasize = 0;
void *memcard_extra_savedatabuffer = NULL;

hashfn_t memcard_hashfn = NULL;
void *memcard_drawasiconfn = NULL;
i32 memcard_autosave = 0;

void SaveSystemInitialise(i32 slots, void *makeSaveHash, void *save, i32 saveSize, i32 saveCount, void *drawSaveIcon,
                          void *extradata, i32 extradataSize) {
    if (extradata == NULL) {
        SAVESLOTS = 6;
        if (slots < 7) {
            SAVESLOTS = slots;
        }
    } else {
        SAVESLOTS = 5;
        if (slots < 6) {
            SAVESLOTS = slots;
        }
    }

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

i32 TriggerExtraDataLoad(void) {
    void *buffer = memcard_extra_savedatabuffer;

    if (saveloadLoadSlot(SAVESLOTS, buffer, memcard_extra_savedatasize + 4) != 0) {
        i32 checksum = *(i32 *)((usize)buffer + memcard_extra_savedatasize);
        i32 correct = ChecksumSaveData(buffer, memcard_extra_savedatasize);
        LOG_DEBUG("checksum=%08X, correct=%08X", checksum, correct);
        if (correct == checksum) {
            memmove(memcard_extra_savedata, buffer, memcard_extra_savedatasize);
            return 1;
        }
    }

    return 0;
}

void createslotfolder(i32 slot) {
    char *path = slotfolder(slot);
    mkdir(path, 0777);
}

extern "C" {
    i32 g_writingSaveCriticalSection = -1;
};

i32 PCSaveSlot(i32 slot, void *extradata, i32 extradataSize, u32 hash) {
    if (saveload_slotused[slot] == 0) {
        createslotfolder(slot);
    }

    if (g_writingSaveCriticalSection == -1) {
        g_writingSaveCriticalSection = NuThreadCreateCriticalSection();
    }

    char *path = fullslotname(slot);
    NuThreadCriticalSectionBegin(g_writingSaveCriticalSection);

    char buf[512];
    NuStrCpy(buf, path);
    NuStrCat(buf, ".incomplete");

    FILE *file = fopen(buf, "wb");
    if (file != NULL) {
        SaveLoad save;

        memset(&save, 0, sizeof(SaveLoad));
        save.field0_0x0 = 0x52474d48;
        save.field1_0x4 = 1;
        save.size = sizeof(SaveLoad);
        save.field3_0xc = 0;
        save.field4_0x10 = 0;
        save.extradata_offset = 0;
        memset(save.field6_0x18, 0, 0x10);
        save.field11_0x1028 = 0;
        save.field13_0x1828 = 0;
        save.field7_0x28 = 0;
        save.field9_0x828 = 0;

        fwrite(&save, sizeof(SaveLoad), 1, file);
        fwrite(extradata, extradataSize, 1, file);
        fwrite(&hash, 4, 1, file);

        fflush(file);
        fclose(file);

        rename(buf, path);
    }

    NuThreadCriticalSectionEnd(g_writingSaveCriticalSection);

    return file != NULL;
}

static i32 statuswait = 0;
static NUTIME savetimer;
static i32 saveload_slotid = -1;

void saveloadASSave(i32 slot, void *buffer, i32 size, u32 hash) {
    statuswait = 1;
    NuTimeGet(&savetimer);
    saveload_status = 11;
    PCSaveSlot(slot, buffer, size, hash);
    saveload_autosave = slot;
    saveload_slotid = slot;
}

void saveloadASLoad(i32 slot, void *buffer, i32 size) {
    saveload_status = 7;
    saveloadLoadSlot(slot, buffer, size);
    statuswait = 1;
    NuTimeGet(&savetimer);
    saveload_slotid = slot;
    saveload_autosave = slot;
}

void saveloadASDelete(i32 slot) {
    char *path = slotname(slot);
    remove(path);

    saveload_slotid = slot;
    if (saveload_autosave == slot) {
        saveload_autosave = -1;
    }
}
