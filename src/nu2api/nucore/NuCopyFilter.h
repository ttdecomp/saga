#pragma once

struct nuframebuffer_s;

struct NuCopyFilter {
    void destroyResources();
    void initResources();
    void render(nuframebuffer_s *);
    void reset();
};
