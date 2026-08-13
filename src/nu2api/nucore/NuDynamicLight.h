#pragma once

#include "nu2api/nucore/common.h"

struct nuframebuffer_s;
struct nugscn_s;
struct nushaderprogram_s;
struct VuVec;
struct VuMtx;
struct nucamera_s;
struct nuvec_s;
struct numtx_s;

struct NuDynamicLight {
    struct RenderSet {
        RenderSet();
    };
    NuDynamicLight();
    void addShadowCasterScene(nugscn_s *);
    void bindShaderResources(nushaderprogram_s *);
    void clone(variptr_u *, variptr_u);
    void computeBoundingSpace(VuVec const *, VuMtx *);
    void computeClippingPlanes(VuMtx const &, bool, VuVec &, VuVec &, VuVec &, VuVec &, VuVec &, VuVec &);
    void computeFrustumCube(nucamera_s const *, VuVec *, VuVec *);
    void computeLightSpace(nuvec_s *, nuvec_s *, numtx_s *, numtx_s *);
    void computeShadowClippingPlanes(VuVec const &, VuVec const *, VuVec *);
    void computeShadowFrustrumCapsule(VuVec const &, VuVec const *, VuVec &, VuVec &, float &);
    void computeWarpEffect(NuDynamicLight::RenderSet &);
    void create();
    void destroy(NuDynamicLight *);
    void refreshShadowTransform(NuDynamicLight::RenderSet &);
    void renderShadowMap(i32, nuframebuffer_s *);
    void resetGeometry();
    void setCameraViewProj(numtx_s *, numtx_s *);
    void setupCustomCameraFrustum(nucamera_s *, float const *, i32);
    void testShadowExtrusion(VuVec const &, VuVec const &, i32);
    void testShadowExtrusions(VuVec const &, VuVec const &);
};
