#pragma once

#include "nu2api/nucore/nuthread.h"

enum class NuApplicationStatus {
    ZERO = 0,
    NUSTATUS_ERROR = 1
};

class NuApplicationState {
  private:
    NuApplicationStatus status;

  public:
    NuApplicationState();

    NuApplicationStatus GetStatus() const;
    void SetStatus(NuApplicationStatus status);
};

class NuCore {
  public:
    static NuThreadManager *m_threadManager;
    static NuApplicationState *m_applicationState;

    static void Initialize();
    static NuApplicationState *GetApplicationState();
};
