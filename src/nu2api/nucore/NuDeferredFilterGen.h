#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/common.h"

struct NuDynamicLight;

struct NuDeferredFilterGenLayout {
    u8 reserved_00[0x54];
    i32 dynamic_light_count;
    NuDynamicLight *dynamic_lights[32];
    i32 deferred_geometry_count;
    u8 reserved_dc[0x2f8 - 0xdc];
};
DECOMP_ASSERT(sizeof(NuDeferredFilterGenLayout) == 0x2f8, "NuDeferredFilterGenLayout size");

struct NuDeferredFilterGen {
    NuDeferredFilterGenLayout layout;

    NuDeferredFilterGen();
    void destroyResources();
    void destroyTextureResources();
    void initResources();
    void initTextureResources(i32, i32);
    void render();
    void renderStencilMask(NuDynamicLight &);
    void resetAll();
};
