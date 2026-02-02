#include "nu2api.saga/nuthread/nuthread.h"

NuThreadManager::NuThreadManager() {
    this->bitflags = 0;
    this->thread = NuThreadInitPS();
}

int32_t NuThreadManager::AllocTLS() {
    for (int32_t i = 0; i < 32; i++) {
        if ((this->bitflags >> (i & 0x1f) & 1) == 0) {
            this->bitflags |= 1 << ((uint8_t)i & 0x1f);
            return i;
        }
    }

    return -1;
}

NuThread *NuThreadManager::CreateThread(void (*thread_fn)(void *), void *fn_arg, int priority, const char *name,
                                        int stack_size, NUTHREADCAFECORE cafe_core, NUTHREADXBOX360CORE xbox360_core) {
    static int ThreadPriorityMap[] = {0, 1, 2, 3, 4};

    NuThreadCreateParameters params;

    stack_size = stack_size == 0 ? 0x8000 : stack_size;

    params.thread_fn = thread_fn;
    params.fn_arg = fn_arg;

    params.priority = ThreadPriorityMap[priority + 2];

    params.name = name;

    params.stack_size = stack_size;

    params.is_suspended = false;

    params.cafe_core = cafe_core;
    params.xbox360_core = xbox360_core;

    params.use_current = false;

    return new NuThread(params);
}

NuThreadBase *NuThreadManager::GetCurrentThread() {
    return NuThreadGetCurrentThread();
}
