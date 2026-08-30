#pragma once

#include "nu2api/nucore/nuthread.h"

// 4-byte application status value. The original returns it directly in EAX;
// making it an enum preserves that ABI while retaining a distinct type.
enum NUAPPLICATIONSTATUS : i32 {
    NUAPPLICATIONSTATUS_IDLE = 0,
    NUAPPLICATIONSTATUS_RENDERING = 1,
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
