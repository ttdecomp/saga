#pragma once

#include "nu2api.saga/nucore/nuthread.h"

struct bgprocinfo_s;

typedef void bgprocdofn(struct bgprocinfo_s *);
typedef void bgprocackfn(struct bgprocinfo_s *);

typedef struct bgvar_s {
} BGVAR;

typedef struct bgprocinfo_s {
    bgprocdofn *do_fn;
    bgprocackfn *ack_fn;

    i32 unknown_flag_1 : 1;
    i32 unknown_flag_2 : 1;

    // This was padding in Bionicle Heroes and doesn't appear to be used here
    // either.
    i32 padding;

    char data[0x100];

    BGVAR *vars;
    i32 var_count;
} BGPROCINFO;

#ifdef __cplusplus
extern "C" {
#endif
    extern NuThreadBase *g_bgProcThread;
    extern i32 g_performingBgProcWorkCritSec;
    extern i32 multithreaded;
    extern char *g_CrashDumpId;
#ifdef __cplusplus
}
#endif

void bgProcInit();

BGPROCINFO *bgPostRequest(bgprocdofn *do_fn, bgprocackfn *ack_fn, void *data, i32 data_size);

i32 bgProcIsBgThread(void);
