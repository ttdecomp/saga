#pragma once

#include "nu2api/nucore/common.h"

class NuDeviceSpecs {
  public:
    i32 specs;
    static NuDeviceSpecs *ms_instance;

    static void Create();
    static void Destroy();

  private:
    void DetermineDeviceSpecs();
};
