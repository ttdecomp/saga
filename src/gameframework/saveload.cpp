#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "decomp.h"

#include "gameframework/saveload.h"

#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nucore/nutime.h"

#include "export.h"

#if defined(HOST_BUILD) && defined(_WIN32)
#include <io.h>
#include <sys/stat.h>

#define mkdir(path, mode) mkdir((path))
#endif

i32 saveload_status;
i32 saveload_autosave = -1;
i32 saveload_savepresent;
i32 saveload_cardtype = 2;
i32 saveload_cardformatted = 1;
i32 saveload_freespace = 0x800;
i32 saveload_filecorrupt;
i32 SAVESIZE_ADDITIONAL = 3;

i32 PCSaveSlot(i32 slot, void *extradata, i32 extradata_size, u32 hash);

i32 saveload_slotused[6] = {0};
i32 saveload_slotcode[6] = {0};

char *slotname(i32 index) {
    static char name[4096];
    sprintf(name, "SaveGame%d.%s_SavedGame", index, "LEGO Star Wars - The Complete Saga");
    return name;
}

char *slotfolder(i32 index) {
    static char name[4096];

    NuStrCpy(name, NuIOS_GetDocumentsPath());
    strcpy(&name[__builtin_strlen(name)], "SavedGames");

    return name;
}

char *fullslotname(i32 index) {
    static char name[4096];

    strcpy(name, slotfolder(index));
    strcat(name, "/");
    strcat(name, slotname(index));

    return name;
}

static i32 saveload_getinfo(void) {
    i32 i;

    saveload_savepresent = 0;
    i32 count = 0;

    for (i = 0; i < 6; i = i + 1) {
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

void saveloadInit(VARIPTR *buf, VARIPTR buf_end, i32, char *prodcode, char *iconname, char *unicodename, i32 unk) {
    // The original truncates the last argument into a 4-byte stack local it
    // never reads again. Its real type and purpose are unknown; the array is
    // sized to reproduce the original's frame layout.
    u16 unk_local[2];
    unk_local[0] = unk;

    saveload_getinfo();
    saveload_status = 1;
    saveload_autosave = -1;
}

i32 saveloadLoadSlot(i32 slot, void *buffer, i32 size) {
    char *filename = fullslotname(slot);
    FILE *file = fopen(filename, "rb");

    LOG_DEBUG("slot=%d, filename=%s, file=%p", slot, filename, file);

    if (file != NULL) {
        SaveLoad buf;

        fread(&buf, 0x2028, 1, file);
        if (buf.extradata_offset != 0) {
            fseek(file, buf.extradata_offset, SEEK_CUR);
        }

        fread(buffer, size, 1, file);
        fclose(file);

        return 1;
    }

    return 0;
}

i32 saveloadSaveSlot(i32 slot, void *buffer, i32 size) {
    return PCSaveSlot(slot, buffer, size, static_cast<u32>(-1));
}

void createslotfolder(i32 slot) {
    mkdir(slotfolder(slot), 0777);
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
    if (file == NULL) {
        NuThreadCriticalSectionEnd(g_writingSaveCriticalSection);
        return 0;
    }

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

    NuThreadCriticalSectionEnd(g_writingSaveCriticalSection);

    return 1;
}

static i32 statuswait;
static NUTIME savetimer;
i32 saveload_slotid;

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
    saveload_autosave = saveload_slotid;
}

void saveloadASDelete(i32 slot) {
    char *path = slotname(slot);
    remove(path);

    saveload_slotid = slot;
    if (saveload_autosave == slot) {
        saveload_autosave = -1;
    }
}

void saveloadASCallEachFrame(void) {
    if (statuswait == 0) {
        return;
    }

    NUTIME now;
    NUTIME elapsed;
    NuTimeGet(&now);
    NuTimeSub(&elapsed, &now, &savetimer);
    if (NuTimeSeconds(&elapsed) > 1.0f) {
        statuswait = 0;
        saveload_getinfo();
        saveload_status = 1;
    }
}
