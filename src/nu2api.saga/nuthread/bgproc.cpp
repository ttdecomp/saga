#include "nu2api.saga/nucore/nucore.h"
#include "nu2api.saga/nuthread/nuthread.h"

NuThreadBase *g_bgProcThread;

int bgProcIsBgThread(void) {
    return NuCore::m_threadManager->GetCurrentThread() == g_bgProcThread;
}
