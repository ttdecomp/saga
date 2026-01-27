#pragma once

#include "nu2api.saga/nuthread/nuthread.h"

class NuApplicationState {
  private:
    void *field0_0x0;

  public:
    NuApplicationState();
};

namespace NuCore {
    static NuThreadManager *m_threadManager;
    static NuApplicationState *m_applicationState;

    void Initialize();
    NuApplicationState *GetApplicationState();
}; // namespace NuCore
