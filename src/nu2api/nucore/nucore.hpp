#pragma once

#include "nu2api/nucore/nuthread.h"

struct NUAPPLICATIONSTATUS {};

class NuApplicationState {
  public:
    NuApplicationState();
    ~NuApplicationState();

    void SetStatus(NUAPPLICATIONSTATUS status);
    NUAPPLICATIONSTATUS GetStatus() const;
};

class NuCore {
  public:
    static NuThreadManager *m_threadManager;
    static NuApplicationState *m_applicationState;

    static void Initialize();
    static NuApplicationState *GetApplicationState();
};
