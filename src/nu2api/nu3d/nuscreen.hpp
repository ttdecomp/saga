#pragma once

#include "nu2api/nucore/common.h"

class NuScreen {

    static NuScreen *ms_instance;

    f32 width, height;

  public:
    NuScreen();
    ~NuScreen();

    static bool Exists();
    static void Create();

    void Destroy();
    void SetSceeenDimensions(f32 width, f32 height);
};
