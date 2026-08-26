#include "nu2api/nucore/nucore.hpp"

#include <new>
#include <stddef.h>

#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nucore/NuCopyFilter.h"
#include "nu2api/nucore/NuDataPortManager.h"
#include "nu2api/nucore/NuDeferredFilter.h"
#include "nu2api/nucore/NuDeferredFilterGen.h"
#include "nu2api/nucore/NuDeviceSpecs.h"
#include "nu2api/nucore/NuDynamicLight.h"
#include "nu2api/nucore/NuMainFilter.h"
#include "nu2api/nucore/NuMainFilterGen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/NuMotionAccumFilterGen.h"
#include "nu2api/nucore/NuMotionFilter.h"
#include "nu2api/nucore/NuMotionFilterGen.h"
#include "nu2api/nucore/NuNetEmu.h"
#include "nu2api/nucore/NuPlatform.h"
#include "nu2api/nucore/NuPostFilter.h"
#include "nu2api/nucore/NuPostFilterGen.h"
#include "nu2api/nucore/NuSpeedBlurFilter.h"
#include "nu2api/nucore/NuSpeedBlurFilterGen.h"
#include "nu2api/nucore/NuVoiceAndroid.h"
#include "nu2api/nucore/numemory.h"
#include "nu2api/nucore/nuthread.h"

NuApplicationState *NuCore::m_applicationState;
NuThreadManager *NuCore::m_threadManager;

void NuCore::Initialize() {
    GetApplicationState();

    m_threadManager = new NuThreadManager();
}

NuApplicationState *NuCore::GetApplicationState(void) {
    if (m_applicationState != NULL) {
        return m_applicationState;
    }

    NuApplicationState *state = NU_ALLOC_T(NuApplicationState, 1, "", NUMEMORY_CATEGORY_NONE);
    if (state != NULL) {
        new (state) NuApplicationState();
    }

    m_applicationState = state;

    return state;
}

NuApplicationState::NuApplicationState() {
}

void NuPlatform::Destroy() {
}

void NuPlatform::Exists() {
}

NuPlatform::NuPlatform() {
}

NuPlatform::~NuPlatform() {
}

void NuCopyFilter::destroyResources() {
}

void NuCopyFilter::initResources() {
}

void NuCopyFilter::render(nuframebuffer_s *) {
}

void NuCopyFilter::reset() {
}

void NuMainFilter::initResources() {
}

void NuPostFilter::initSharedResources(i32, i32) {
}

void NuPostFilter::renderFrustum(numtx_s *) {
}

void NuDeviceSpecs::Exists() {
}

NuDeviceSpecs::NuDeviceSpecs() {
}

NuDeviceSpecs::~NuDeviceSpecs() {
}

NuDynamicLight::NuDynamicLight() {
}

void NuDynamicLight::addShadowCasterScene(nugscn_s *) {
}

void NuDynamicLight::bindShaderResources(nushaderprogram_s *) {
}

void NuDynamicLight::clone(variptr_u *, variptr_u) {
}

void NuDynamicLight::computeBoundingSpace(VuVec const *, VuMtx *) {
}

void NuDynamicLight::computeClippingPlanes(VuMtx const &, bool, VuVec &, VuVec &, VuVec &, VuVec &, VuVec &, VuVec &) {
}

void NuDynamicLight::computeFrustumCube(nucamera_s const *, VuVec *, VuVec *) {
}

void NuDynamicLight::computeLightSpace(nuvec_s *, nuvec_s *, numtx_s *, numtx_s *) {
}

void NuDynamicLight::computeShadowClippingPlanes(VuVec const &, VuVec const *, VuVec *) {
}

void NuDynamicLight::computeShadowFrustrumCapsule(VuVec const &, VuVec const *, VuVec &, VuVec &, float &) {
}

void NuDynamicLight::computeWarpEffect(NuDynamicLight::RenderSet &) {
}

void NuDynamicLight::create() {
}

void NuDynamicLight::destroy(NuDynamicLight *) {
}

void NuDynamicLight::refreshShadowTransform(NuDynamicLight::RenderSet &) {
}

void NuDynamicLight::renderShadowMap(i32, nuframebuffer_s *) {
}

void NuDynamicLight::resetGeometry() {
}

void NuDynamicLight::setCameraViewProj(numtx_s *, numtx_s *) {
}

void NuDynamicLight::setupCustomCameraFrustum(nucamera_s *, float const *, i32) {
}

void NuDynamicLight::testShadowExtrusion(VuVec const &, VuVec const &, i32) {
}

void NuDynamicLight::testShadowExtrusions(VuVec const &, VuVec const &) {
}

void NuMotionFilter::initResources() {
}

void NuRenderDevice::DetermineNominalAspectRatio(u32, u32) const {
}

void NuRenderDevice::IsContextValid() const {
}

void NuRenderDevice::MultiThreadRender() const {
}

void NuRenderDevice::OnAppPaused() {
}

void NuRenderDevice::OnAppRestarted() {
}

void NuRenderDevice::OnAppResume() {
}

void NuRenderDevice::OnAppStarted() {
}

void NuRenderDevice::OnAppStopped() {
}

void NuRenderDevice::OnGainedFocus() {
}

void NuRenderDevice::OnLostFocus() {
}

void NuRenderDevice::OnWindowDestroy() {
}

void NuRenderDevice::OpenglErrorCallback(u32, u32, u32, u32, i32, char const *, void *) {
}

void NuRenderDevice::PreInitialize() {
}

void NuRenderDevice::ResizeDevice(i32, i32, i32, bool, bool, bool, bool) {
}

void NuVoiceAndroid::ApplyHardwareVoiceMix() {
}

void NuVoiceAndroid::CreateHardwareVoice() {
}

void NuVoiceAndroid::DestroyHardwareVoice() {
}

void NuVoiceAndroid::GetInterfaces() {
}

void NuVoiceAndroid::GetPlaybackPositionSamples() {
}

NuVoiceAndroid::NuVoiceAndroid(NuSoundSource *, bool) {
}

void NuVoiceAndroid::OnPlayerEvent(u32) {
}

void NuVoiceAndroid::PauseHardwareVoice() {
}

void NuVoiceAndroid::PlayerCallback(SLPlayItf_ const *const *, void *, u32) {
}

void NuVoiceAndroid::RealiseObject() {
}

void NuVoiceAndroid::ResumeHardwareVoice() {
}

void NuVoiceAndroid::StartHardwareVoice() {
}

void NuVoiceAndroid::StopHardwareVoice() {
}

void NuVoiceAndroid::SubmitBuffer(NuSoundBuffer *) {
}

void NuVoiceAndroid::UpdateHardwareVoice(float) {
}

void NuVoiceAndroid::UpdateQueue() {
}

void NuVoiceAndroid::UpdateSamplePlaybackCount() {
}

void NuVoiceAndroid::UpdateState() {
}

NuVoiceAndroid::~NuVoiceAndroid() {
}

NuMainFilterGen::NuMainFilterGen() {
}

void NuMainFilterGen::destroyResources() {
}

void NuMainFilterGen::destroyTextureResources() {
}

void NuMainFilterGen::initResources() {
}

void NuMainFilterGen::initTextureResources(i32, i32) {
}

void NuMainFilterGen::preprocessBlurTextures(nueffecttex_s *, nueffecttex_s *) {
}

void NuMainFilterGen::preprocessDofMotionBlur(nueffecttex_s *) {
}

void NuMainFilterGen::render() {
}

void NuMainFilterGen::reset() {
}

void NuPostFilterGen::GetSampleOffsets_GaussBlur5x5(i32, i32, VuVec *, float) {
}

void NuPostFilterGen::blend(nueffecttex_s *, nueffecttex_s *, nuframebuffer_s *) {
}

void NuPostFilterGen::blur5x5(nueffecttex_s *, i32, nueffecttex_s *, i32, i32, i32, bool) {
}

void NuPostFilterGen::blur7x7Loopback(nueffecttex_s *, i32, nueffecttex_s *, i32, i32, i32, bool, float,
                                      nushaderprogram_s *) {
}

void NuPostFilterGen::blur7x7Separate(nueffecttex_s *, i32, nueffecttex_s *, i32, i32, i32, bool, float,
                                      nushaderprogram_s *) {
}

void NuPostFilterGen::copy(nueffecttex_s *, i32, nueffecttex_s *, i32, nushaderprogram_s *, nueffecttex_s *) {
}

void NuPostFilterGen::copy(nueffecttex_s *, nueffecttex_s *, nuframebuffer_s *) {
}

void NuPostFilterGen::copy(nueffecttex_s *, nuframebuffer_s *) {
}

void NuPostFilterGen::copyDepth(nueffecttex_s *, nuframebuffer_s *) {
}

void NuPostFilterGen::destroyResources() {
}

void NuPostFilterGen::destroySharedResources() {
}

void NuPostFilterGen::destroySharedTextureResources() {
}

void NuPostFilterGen::initResources() {
}

void NuPostFilterGen::initSharedResources() {
}

void NuPostFilterGen::initSharedTextureResources(i32, i32) {
}

void NuPostFilterGen::renderFrustum(numtx_s *) {
}

void NuPostFilterGen::renderQuad() {
}

void NuPostFilterGen::renderQuadGrid() {
}

void NuDeferredFilter::initResources() {
}

void NuDataPortManager::registerPort(char const *, void *) {
}

NuMotionFilterGen::NuMotionFilterGen() {
}

void NuMotionFilterGen::render() {
}

void NuSpeedBlurFilter::initResources() {
}

void NuApplicationState::SetStatus(NUAPPLICATIONSTATUS value) {
    status = value;
}

NUAPPLICATIONSTATUS NuApplicationState::GetStatus() const {
    return status;
}

NuApplicationState::~NuApplicationState() {
}

NuDeferredFilterGen::NuDeferredFilterGen() {
}

void NuDeferredFilterGen::destroyResources() {
}

void NuDeferredFilterGen::destroyTextureResources() {
}

void NuDeferredFilterGen::initResources() {
}

void NuDeferredFilterGen::initTextureResources(i32, i32) {
}

void NuDeferredFilterGen::render() {
}

void NuDeferredFilterGen::renderStencilMask(NuDynamicLight &) {
}

void NuDeferredFilterGen::resetAll() {
}

void NuMotionAccumFilter::initResources() {
}

NuSpeedBlurFilterGen::NuSpeedBlurFilterGen() {
}

void NuSpeedBlurFilterGen::computeSpeedBlur(VuVec &) {
}

void NuSpeedBlurFilterGen::destroyTextureResources() {
}

void NuSpeedBlurFilterGen::initTextureResources(i32, i32) {
}

void NuSpeedBlurFilterGen::render() {
}

void NuMotionAccumFilterGen::GetTiming(i32 *) {
}

NuMotionAccumFilterGen::NuMotionAccumFilterGen() {
}

void NuMotionAccumFilterGen::destroyResources() {
}

void NuMotionAccumFilterGen::destroyTextureResources() {
}

void NuMotionAccumFilterGen::initResources() {
}

void NuMotionAccumFilterGen::initTextureResources(i32, i32) {
}

void NuMotionAccumFilterGen::render() {
}

void NuNetEmu::FindPacket(nunetaddr_s *, i32) {
}

NuNetEmu::NuNetEmu() {
}

void NuNetEmu::RecvFrom(void *, i32, nunetaddr_s &) {
}

void NuNetEmu::SendTo(void *, i32, nunetaddr_s *, i32) {
}

void NuNetEmu::SetConditions(NuNetEmu::eConditions) {
}

void NuNetEmu::SplitSendPacket(NuNetEmu::EmuPacket *) {
}

void NuNetEmu::Update() {
}
