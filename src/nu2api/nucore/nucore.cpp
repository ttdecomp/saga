#include "nu2api/nucore/nucore.hpp"

#include <new>
#include <stddef.h>

#include "nu2api/nucore/numemory.h"
#include "nu2api/nucore/nuthread.h"

NuApplicationState *NuCore::m_applicationState;
NuThreadManager *NuCore::m_threadManager;

void NuCore::Initialize() {
    GetApplicationState();

    m_threadManager = new NuThreadManager();
}

NuApplicationStatus NuApplicationState::GetStatus() const {
    return this->status;
}

void NuApplicationState::SetStatus(NuApplicationStatus status) {
    this->status = status;
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
    this->status = NuApplicationStatus::ZERO;
}
