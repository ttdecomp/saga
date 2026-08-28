#include <format>
#include <regex>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <strings.h>
#include <vector>

#include <string>

#include "decomp.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/world/level.h"
#include "legogame/startup.h"
#include "legogame/target.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nudevicespecs.hpp"
#include "nu2api/nuplatform/nuplatform.h"
#include "legoapi/world/area.h"

#ifdef _WIN32
static const char *strcasestr(const char *haystack, const char *needle) {
    if (!*needle)
        return haystack;

    for (; *haystack; ++haystack) {
        const char *h = haystack;
        const char *n = needle;

        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            ++h;
            ++n;
        }

        if (!*n)
            return haystack;
    }

    return NULL;
}
#endif

char buf[0x1000000];

// HOST-ONLY: walk the dat's hash-name section and print "index name" pairs
// (mirrors the scan in NuDatFileFindHash).
static void DatListNames(NUDATHDR *dat, const char *filter) {
    if (dat->hash_count == 0) {
        printf("no hash-name section\n");
        return;
    }
    const char *cur = dat->hashes;
    for (i32 i = 0; i < dat->hash_count; i++) {
        const char *name = cur;
        usize len = strlen(name);
        cur += len + 1;
        if ((uintptr_t)cur & 1) {
            cur++;
        }
        i16 idx = *(const i16 *)cur;
        cur += 2;
        if ((uintptr_t)cur & 1) {
            cur++;
        }
        if (filter == NULL || strcasestr(name, filter) != NULL) {
            NUDATFINFO *info = dat->file_info != NULL ? &dat->file_info[idx] : NULL;
            fprintf(stderr, "%d %s (len=%d decomp=%d mode=%d)\n", idx, name, info ? info->file_len : -1,
                    info ? info->decompressed_len : -1, info ? info->compression_mode : -1);
        }
        cur += 2; // trailing pad before next entry (NuStrCmp stride is len+3)
        // NOTE: original advances ptr by StrLen+3 then aligns; replicate below.
    }
}

int test_load(int argc, char **argv) {
    VARIPTR buf_ptr = VARIPTR{.void_ptr = &buf};
    const char *cmd = argc > 1 ? argv[1] : "";
    VARIPTR end_ptr = VARIPTR{.void_ptr = &buf[sizeof(buf)]};

    NUDATHDR *dat = NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &buf_ptr, 0);
    NuDatSet(dat);

    if (strcmp(cmd, "list") == 0) {
        DatListNames(dat, argc > 2 ? argv[2] : NULL);
        _exit(0); // skip static dtors: engine globals free non-heap pointers
    }

    if (strcmp(cmd, "extract") == 0 && argc > 2) {
        NUFILE f = NuFileOpen((char *)argv[2], NUFILE_READ);
        if (f == 0) {
            fprintf(stderr, "not found: %s\n", argv[2]);
            _exit(1);
        }
        i32 size = NuFileOpenSize(f);
        std::vector<char> data(size);
        NuFileRead(f, data.data(), size);
        NuFileClose(f);
        const char *out = argc > 3 ? argv[3] : ".work/extracted.bin";
        FILE *fp = fopen(out, "wb");
        fwrite(data.data(), 1, size, fp);
        fclose(fp);
        fprintf(stderr, "extracted %d bytes -> %s\n", size, out);
        _exit(0); // skip static dtors: engine globals free non-heap pointers
    }

    i32 area_count = 0;
    AREADATA_s *ADataList = Areas_ConfigureList("levels\\areas.txt", &buf_ptr, &end_ptr, 9999, &area_count);
    LOG_INFO("Loaded %d areas", area_count);
    for (i32 i = 0; i < area_count; i++) {
        AREADATA_s *area = &ADataList[i];
        LOG_INFO("Area %d: %s \\ %s", i, area->dir, area->file);
    }

    i32 level_count = 0;
    LEVELDATA *levels = Levels_ConfigureList("levels\\levels.txt", &buf_ptr, &end_ptr, 9999, &level_count, NULL);

    LOG_INFO("Loaded %d levels", level_count);
    for (i32 i = 0; i < level_count; i++) {
        LEVELDATA *level = &levels[i];
        LOG_INFO("Level %d: %s \\ %s", i, level->dir, level->name);
    }

    StartPerm();
    LoadPerm();
    EndPerm();

    LOG_INFO("COMPLETIONPOINTS=%d", COMPLETIONPOINTS);
    for (i32 i = 0; i < 3; i++) {
        f32 completion = (f32)saveload_slotcode[i] * 100.0f / COMPLETIONPOINTS;
        LOG_INFO("slot %d used=%d completion=%f%% (%.1f%%)", i, saveload_slotused[i], completion, completion);
    }

    LEVELDATA *level = Level_FindByName("titles", NULL);
    LOG_INFO("titles level: %p", level);

    while (true) {
        NuThreadSleep(1);
    }

    return 0;
}
