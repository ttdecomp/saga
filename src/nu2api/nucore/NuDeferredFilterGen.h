#pragma once

struct NuDynamicLight;

struct NuDeferredFilterGen {
    NuDeferredFilterGen();
    void destroyResources();
    void destroyTextureResources();
    void initResources();
    void initTextureResources(i32, i32);
    void render();
    void renderStencilMask(NuDynamicLight &);
    void resetAll();
};
