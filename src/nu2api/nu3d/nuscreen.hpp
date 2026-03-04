#pragma once

#include "nu2api/nucore/common.h"

class NuScreen {

    static NuScreen *ms_instance;

    f32 width, height;

  public:
    static bool Exists();
    static void Create();

    void SetSceeenDimensions(f32 width, f32 height);
};
