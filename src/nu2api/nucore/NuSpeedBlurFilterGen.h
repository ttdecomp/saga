#pragma once

struct VuVec;

struct NuSpeedBlurFilterGen {
    NuSpeedBlurFilterGen();
    void computeSpeedBlur(VuVec &);
    void destroyTextureResources();
    void initTextureResources(i32, i32);
    void render();
};
