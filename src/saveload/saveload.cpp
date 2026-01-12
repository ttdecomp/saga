#include "saveload/saveload.h"

#include "nu2api.saga/nuandroid/nuios.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nucore/nutime.h"
#include "nu2api.saga/numemory/numemory.h"
#include "nu2api.saga/nuthread/nuthread.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "export.h"

int32_t saveload_status = 0;
int32_t saveload_autosave = 0;
int32_t saveload_savepresent = 0;

int32_t saveload_slotused[6] = {0};
char *saveload_slotcode[6] = {0};

char *slotname(int32_t index) {
    static char name[4096];
    sprintf(name, "SaveGame%d.%s_SavedGame", index, "LEGO Star Wars - The Complete Saga");
    return name;
}

char *slotfolder(int32_t index) {
    static char name[4096];

    char *path = NuIOS_GetDocumentsPath();
    NuStrCpy(name, path);

    int32_t len = strlen(name);

    strcpy(&name[len], "SavedGames");

    return name;
}

char *fullslotname(int32_t index) {
    static char name[4096];

    strcpy(name, slotfolder(index));

    int32_t len = strlen(name);

    strcat(name, "/");
    strcat(name, slotname(index));

    return name;
}

static int32_t saveload_getinfo(void) {
    saveload_savepresent = 0;
    int32_t count = 0;

    for (int32_t i = 0; i < 6; i = i + 1) {
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

void saveloadInit(void **bufferStart, void *bufferEnd, int32_t, char *prodcode, char *iconname, char *unicodename,
                  int32_t) {
    saveload_getinfo();
    saveload_status = 1;
    saveload_autosave = -1;
}

int32_t saveloadLoadSlot(int32_t slot, void *buffer, size_t size) {
    char *filename = fullslotname(slot);
    FILE *file = fopen(filename, "rb");

    LOG("slot=%d, filename=%s, file=%p", slot, filename, file);

    if (file == NULL) {
        return 0;
    } else {
        SaveLoad buf;

        fread(&buf, 0x2028, 1, file);
        if (buf.extradataOffset != 0) {
            fseek(file, buf.extradataOffset, SEEK_CUR);
        }

        fread(buffer, size, 1, file);
        fclose(file);

        return 1;
    }
}

typedef short (*hashfn_t)(void);

int32_t SAVESLOTS = 3;

void *memcard_savedata = NULL;
int32_t memcard_savedatasize = 0;
void *memcard_savedatabuffer = NULL;

void *memcard_extra_savedata = NULL;
int32_t memcard_extra_savedatasize = 0;
void *memcard_extra_savedatabuffer = NULL;

hashfn_t memcard_hashfn = NULL;
void *memcard_drawasiconfn = NULL;
int32_t memcard_autosave = 0;

void SaveSystemInitialise(int32_t slots, void *makeSaveHash, void *save, int32_t saveSize, int32_t saveCount,
                          void *drawSaveIcon, void *extradata, int32_t extradataSize) {
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

    NuMemory *mem = NuMemoryGet();
    NuMemoryManager *manager = mem->GetThreadMem();

    memcard_savedatabuffer = manager->_BlockAlloc(saveSize + 4, 4, 1, "", 0);
    mem = NuMemoryGet();

    manager = mem->GetThreadMem();

    memcard_extra_savedatabuffer = (void *)manager->_BlockAlloc(extradataSize + 4, 4, 1, "", 0);

    memcard_autosave = saveCount;
    memcard_drawasiconfn = drawSaveIcon;
}

int32_t ChecksumSaveData(void *buffer, int32_t size) {
    int32_t n = size / 4;

    int32_t sum = 0x5c0999;

    for (int32_t i = 0; i < n; i++) {
        sum += ((int32_t *)buffer)[i];
    }

    return sum;
}

int32_t TriggerExtraDataLoad(void) {
    void *buffer = memcard_extra_savedatabuffer;

    if (saveloadLoadSlot(SAVESLOTS, buffer, memcard_extra_savedatasize + 4) != 0) {
        int32_t checksum = *(int32_t *)((size_t)buffer + memcard_extra_savedatasize);
        if (ChecksumSaveData(buffer, memcard_extra_savedatasize) == checksum) {
            memmove(memcard_extra_savedata, buffer, memcard_extra_savedatasize);
            return 1;
        }
    }

    return 0;
}

void createslotfolder(int32_t slot) {
    char *path = slotfolder(slot);
    mkdir(path, 0777);
}

extern "C" {
    int32_t g_writingSaveCriticalSection = -1;
};

int32_t PCSaveSlot(int32_t slot, void *extradata, int32_t extradataSize, uint32_t hash) {
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
        save.extradataOffset = 0;
        memset(save.field6_0x18, 0, 0x10);
        save.field4101_0x1028 = 0;
        save.field6148_0x1828 = 0;
        save.field7_0x28 = 0;
        save.field2054_0x828 = 0;

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

static int32_t statuswait = 0;
static NUTIME savetimer;
static int32_t saveload_slotid = -1;

extern "C" void saveloadASSave(int slot, void *buffer, int size, int hash) {
    statuswait = 1;
    NuTimeGet(&savetimer);
    saveload_status = 11;
    PCSaveSlot(slot, buffer, size, hash);
    saveload_autosave = slot;
    saveload_slotid = slot;
}
