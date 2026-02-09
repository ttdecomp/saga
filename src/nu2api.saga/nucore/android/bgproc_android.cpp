#include "nu2api.saga/nucore/bgproc.h"

#include "nu2api.saga/nucore/android/NuThread_android.h"
#include "nu2api.saga/nucore/nucore.hpp"
#include "nu2api.saga/nucore/nulst.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nucore/nuthread.h"

NuThreadBase *g_bgProcThread;

static NULSTHDR *procinfo_pool;
static i32 g_bgCritSec;

i32 multithreaded = 1;

i32 g_performingBgProcWorkCritSec = -1;

char *g_CrashDumpId = "CRASHDUMP_ID=LEGOSAGAANDROID_ANDROID_Feb_17_2014_01_01_01$";

static char dummyBuf[0x80];
static BGPROCINFO *cur_pi;

static NuThreadSemaphore events[2] = {
    NuThreadSemaphore(1),
    NuThreadSemaphore(1),
};

static void bgThreadMain(void *data) {
    NuStrNCpy(dummyBuf, g_CrashDumpId, 0x20);

    events[0].Wait();

    NuThreadCriticalSectionBegin(g_bgCritSec);
    cur_pi = (BGPROCINFO *)NuLstGetNext(procinfo_pool, NULL);
    NuThreadCriticalSectionEnd(g_bgCritSec);

    while (true) {
        cur_pi->unknown_flag_1 = true;

        NuThreadCriticalSectionBegin(g_performingBgProcWorkCritSec);

        if (cur_pi->do_fn != NULL) {
            (*cur_pi->do_fn)(cur_pi);
        }

        if (cur_pi->ack_fn != NULL) {
            (*cur_pi->ack_fn)(cur_pi);
        }

        NuThreadCriticalSectionEnd(g_performingBgProcWorkCritSec);

        NuThreadCriticalSectionBegin(g_bgCritSec);
        NuLstFree((NULNKHDR *)cur_pi);

        while (true) {
            cur_pi = (BGPROCINFO *)NuLstGetNext(procinfo_pool, NULL);

            NuThreadCriticalSectionEnd(g_bgCritSec);

            if (cur_pi != NULL) {
                break;
            }

            events[0].Wait();

            NuThreadCriticalSectionBegin(g_bgCritSec);
        }
    }
}

void bgProcInit() {
    static i32 init;

    if (init == 0) {
        init = 1;

        procinfo_pool = NuLstCreate(0x10, sizeof(BGPROCINFO));

        g_bgCritSec = NuThreadCreateCriticalSection();
        g_performingBgProcWorkCritSec = NuThreadCreateCriticalSection();

        g_bgProcThread = NuCore::m_threadManager->CreateThread(
            bgThreadMain, NULL, 0, "bgProc", 0x100000, NUTHREADCAFECORE_UNKNOWN_1, NUTHREADXBOX360CORE_UNKNOWN_1);
    }
}

i32 bgProcIsBgThread(void) {
    return NuCore::m_threadManager->GetCurrentThread() == g_bgProcThread;
}
