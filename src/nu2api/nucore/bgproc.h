#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/nuthread.h"

struct bgprocinfo_s;

typedef void bgprocdofn(struct bgprocinfo_s *);
typedef void bgprocackfn(struct bgprocinfo_s *);

enum BGPROCARGTYPE {
    BGPROC_ARG_NONE = 0,
    BGPROC_ARG_I32 = 1,
    BGPROC_ARG_FLOAT = 2,
    BGPROC_ARG_STRING = 3,
    BGPROC_ARG_U32 = 4,
    BGPROC_ARG_POINTER = 5,
    BGPROC_ARG_BOOL = 6,
    BGPROC_ARG_END = 7,
};

typedef struct bgvar_s {
    BGPROCARGTYPE type;
    union {
        i32 i32_value;
        u32 u32_value;
        f32 float_value;
        void *pointer_value;
        char *string_value;
    } value;
} BGVAR;

DECOMP_ASSERT(sizeof(BGVAR) == 8, "BGVAR size");

typedef struct bgprocinfo_s {
    bgprocdofn *do_fn;
    bgprocackfn *ack_fn;

    i32 work_started : 1;
    i32 unknown_flag_2 : 1;

    // This was padding in Bionicle Heroes and doesn't appear to be used here
    // either.
    i32 padding;

    char data[0x100];

    BGVAR *vars;
    i32 var_count;
} BGPROCINFO;

DECOMP_ASSERT(sizeof(BGPROCINFO) == 0x118, "BGPROCINFO size");
DECOMP_ASSERT(offsetof(BGPROCINFO, vars) == 0x110, "BGPROCINFO vars offset");

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

BGPROCINFO *bgPostRequestV(bgprocdofn *do_fn, bgprocackfn *ack_fn, i32 first_type, ...);

BGPROCINFO *bgGetProcActive(void);

i32 bgProcIsBgThread(void);
