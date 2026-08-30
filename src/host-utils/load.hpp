#pragma once

#include <cctype>
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <vector>

#include "decomp.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legogame/startup.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nufile/nufile.h"

static const char *host_strcasestr(const char *haystack, const char *needle) {
#ifdef _WIN32
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
#else
    return strcasestr(haystack, needle);
#endif
}

enum class HostLoadAction {
    inspect,
    list,
    extract,
};

struct HostLoadOptions {
    HostLoadAction action = HostLoadAction::inspect;
    const char *filter = nullptr;
    const char *dat_path = nullptr;
    const char *output_path = ".work/extracted.bin";
};

static char host_load_buffer[0x1000000];

// HOST-ONLY: walk the dat's hash-name section and print "index name" pairs
// (mirrors the scan in NuDatFileFindHash).
static void host_dat_list_names(NUDATHDR *dat, const char *filter) {
    fprintf(stderr, "dat version=%d files=%d hashes=%d hash_bytes=%d\n", dat->version, dat->file_count, dat->hash_count,
            dat->hashes_len);
    if (dat->hash_count == 0) {
        fprintf(stderr, "no hash-name section\n");
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
        if (filter == NULL || host_strcasestr(name, filter) != NULL) {
            NUDATFINFO *info = dat->file_info != NULL ? &dat->file_info[idx] : NULL;
            fprintf(stderr, "%d %s (len=%d decomp=%d mode=%d)\n", idx, name, info ? info->file_len : -1,
                    info ? info->decompressed_len : -1, info ? info->compression_mode : -1);
        }
        cur += 2; // trailing pad before next entry (NuStrCmp stride is len+3)
        // NOTE: original advances ptr by StrLen+3 then aligns; replicate below.
    }
}

static i32 host_run_load(const HostLoadOptions &options) {
    VARIPTR buf_ptr = VARIPTR{.void_ptr = &host_load_buffer};
    VARIPTR end_ptr = VARIPTR{.void_ptr = &host_load_buffer[sizeof(host_load_buffer)]};

    NUDATHDR *dat = NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &buf_ptr, 0);
    NuDatSet(dat);

    if (options.action == HostLoadAction::list) {
        host_dat_list_names(dat, options.filter);
        return 0;
    }

    if (options.action == HostLoadAction::extract) {
        i32 node_idx = NuDatFileFindTree(dat, const_cast<char *>(options.dat_path));
        if (node_idx >= 0 && dat->file_info != NULL) {
            NUDATFINFO *info = &dat->file_info[node_idx];
            fprintf(stderr, "entry=%d len=%d decomp=%d mode=%d offset=%d\n", node_idx, info->file_len,
                    info->decompressed_len, info->compression_mode, info->file_offset);
        }
        NUFILE f = NuFileOpen(const_cast<char *>(options.dat_path), NUFILE_READ);
        if (f == 0) {
            fprintf(stderr, "not found: %s\n", options.dat_path);
            return 1;
        }
        i32 size = NuFileOpenSize(f);
        std::vector<char> data(size);
        NuFileRead(f, data.data(), size);
        NuFileClose(f);
        FILE *fp = fopen(options.output_path, "wb");
        fwrite(data.data(), 1, size, fp);
        fclose(fp);
        fprintf(stderr, "extracted %d bytes -> %s\n", size, options.output_path);
        return 0;
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
