#pragma once

struct nueffecttex_s;

struct NuMainFilterGen {
    NuMainFilterGen();
    void destroyResources();
    void destroyTextureResources();
    void initResources();
    void initTextureResources(i32, i32);
    void preprocessBlurTextures(nueffecttex_s *, nueffecttex_s *);
    void preprocessDofMotionBlur(nueffecttex_s *);
    void render();
    void reset();
};
