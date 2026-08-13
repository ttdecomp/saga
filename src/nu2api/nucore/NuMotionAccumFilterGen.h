#pragma once

struct NuMotionAccumFilterGen {
    void GetTiming(i32 *);
    NuMotionAccumFilterGen();
    void destroyResources();
    void destroyTextureResources();
    void initResources();
    void initTextureResources(i32, i32);
    void render();
};
