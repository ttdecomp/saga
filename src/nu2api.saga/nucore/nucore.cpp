#include "nucore.h"

#include <new>

namespace NuCore {
    void Initialize() {
        GetApplicationState();

        m_threadManager = new NuThreadManager();
    }

    NuApplicationState *GetApplicationState(void) {
        if (m_applicationState != NULL) {
            return m_applicationState;
        }

        NuApplicationState *state = (NuApplicationState *)NuMemoryGet()->GetThreadMem()->_BlockAlloc(4, 4, 1, "", 0);
        if (state != NULL) {
            new (state) NuApplicationState();
        }

        m_applicationState = state;
    }
} // namespace NuCore

NuApplicationState::NuApplicationState() {
    this->field0_0x0 = 0;
}