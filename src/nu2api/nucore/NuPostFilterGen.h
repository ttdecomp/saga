#pragma once

struct VuVec;
struct nueffecttex_s;
struct nuframebuffer_s;
struct nushaderprogram_s;
struct numtx_s;

struct NuPostFilterGen {
    virtual ~NuPostFilterGen() {
    }

    bool enabled;
    u8 unknown_005[0x08 - 0x05];
    nuframebuffer_s *input_fbo;

    void GetSampleOffsets_GaussBlur5x5(i32, i32, VuVec *, float);
    void blend(nueffecttex_s *, nueffecttex_s *, nuframebuffer_s *);
    void blur5x5(nueffecttex_s *, i32, nueffecttex_s *, i32, i32, i32, bool);
    void blur7x7Loopback(nueffecttex_s *, i32, nueffecttex_s *, i32, i32, i32, bool, float, nushaderprogram_s *);
    void blur7x7Separate(nueffecttex_s *, i32, nueffecttex_s *, i32, i32, i32, bool, float, nushaderprogram_s *);
    void copy(nueffecttex_s *, i32, nueffecttex_s *, i32, nushaderprogram_s *, nueffecttex_s *);
    void copy(nueffecttex_s *, nueffecttex_s *, nuframebuffer_s *);
    void copy(nueffecttex_s *, nuframebuffer_s *);
    void copyDepth(nueffecttex_s *, nuframebuffer_s *);
    void destroyResources();
    void destroySharedResources();
    void destroySharedTextureResources();
    void initResources();
    void initSharedResources();
    void initSharedTextureResources(i32, i32);
    void renderFrustum(numtx_s *);
    void renderQuad();
    void renderQuadGrid();
    void reset();
    void resetAll();
};
