#pragma once

struct TouchHolder;

struct SwipeDecalRenderer {
    struct Style {};
    SwipeDecalRenderer(TouchHolder &, i32, SwipeDecalRenderer::Style);
    void Process(float);
    void Render();
};
