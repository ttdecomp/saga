#pragma once

#include "nu2api/nucore/nuthread.h"

// 4-byte application status value (original passes it by value; the render
// thread wait loop compares it against 1 = "render in progress").
struct NUAPPLICATIONSTATUS {
    i32 status;
};

class NuApplicationState {
  public:
    NuApplicationState();
    ~NuApplicationState();

    NUAPPLICATIONSTATUS status;

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
