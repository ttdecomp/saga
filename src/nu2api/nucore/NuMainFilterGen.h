#pragma once

#include "nu2api/nucore/NuPostFilterGen.h"

struct nueffecttex_s;

struct NuMainFilterGen : NuPostFilterGen {
    NuMainFilterGen();
    void destroyResources();
    void destroyTextureResources();
    void initResources();
    void initTextureResources(i32, i32);
    void preprocessBlurTextures(nueffecttex_s *, nueffecttex_s *);
    void preprocessDofMotionBlur(nueffecttex_s *);
    void render();
    void reset();

    u8 unknown_00c[0x88 - 0x0c];
    bool blur_enabled;
    bool dof_enabled;
    bool motion_blur_enabled;
    u8 unknown_08b;
    i32 active_filter_count;
};
