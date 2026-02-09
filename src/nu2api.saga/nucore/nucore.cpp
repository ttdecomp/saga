#include "nu2api.saga/nucore/nucore.hpp"

#include <new>
#include <stddef.h>

#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/nuthread/nuthread.h"

NuApplicationState *NuCore::m_applicationState;
NuThreadManager *NuCore::m_threadManager;

void NuCore::Initialize() {
    GetApplicationState();

    m_threadManager = new NuThreadManager();
}

NuApplicationState *NuCore::GetApplicationState(void) {
    if (m_applicationState != NULL) {
        return m_applicationState;
    }

    NuApplicationState *state = NU_ALLOC_T(NuApplicationState, 1, "", NUMEMORY_CATEGORY_NONE);
    if (state != NULL) {
        new (state) NuApplicationState();
    }

    m_applicationState = state;

    return state;
}

NuApplicationState::NuApplicationState() {
    this->field0_0x0 = 0;
}
