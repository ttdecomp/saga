#ifndef NU2API_NUCORE_TYPES_H
#define NU2API_NUCORE_TYPES_H
#pragma once

#include "nu2api/nucore/fixed_width.h"

struct NUAPPLICATIONSTATUS;
struct NUGCUTCHAR_s;
struct NUGCUTLOCATOR_s;
struct NUGCUTRIGID_s;
struct NUGCUTSCENE_s;
struct NUJOINTANIM_s;
struct NUPADMOTIONVALUE;
struct NUPADTYPE;
struct NUTEXFORMAT;
struct NUTHREADCAFECORE;
struct NUTHREADXBOX360CORE;
struct NetSmallStats;
struct NuApplicationState;
struct NuButtonLayout;
struct NuCopyFilter;
struct NuDataPortManager;
struct NuDeferredFilter;
struct NuDeferredFilterGen;
struct NuDeviceSpecs;
struct NuDynamicLight;
struct NuFadeObjGType;
struct NuIOS_InAppProduct;
struct NuInputDevice;
struct NuInputDevicePS;
struct NuInputManager;
struct NuInputTouchData;
struct NuMainFilter;
struct NuMainFilterGen;
struct NuMemory;
struct NuMemoryManager;
struct NuMemoryPool;
struct NuMotionAccumFilter;
struct NuMotionAccumFilterGen;
struct NuMotionFilter;
struct NuMotionFilterGen;
struct NuMusic;
struct NuNetEmu;
struct NuPlatform;
struct NuPostFilter;
struct NuPostFilterGen;
struct NuRenderDevice;
struct NuSoundBuffer;
struct NuSoundSource;
struct NuSpeedBlurFilter;
struct NuSpeedBlurFilterGen;
struct NuSymbolQuery;
struct NuThread;
struct NuThreadBase;
struct NuThreadManager;
struct NuTouchInputButton;
struct NuTouchInputElement;
struct NuTouchInputStick;
struct NuVirtualTouchDevice;
struct NuVoiceAndroid;
struct NuWindGType;
struct SLPlayItf_;
struct ShaderObjectKey;
struct VuMtx;
struct VuVec;
struct _vuv_s;
struct ani3_animheader_s;
struct d3dsamplerstate_u;
struct nuanimbuff_s;
struct nuanimdatachunk_s;
struct nucamera_s;
struct nudeferredshadingenum_e;
struct nudisplaylistitem_s;
struct nudisplayscene_s;
struct nudynamiclight_s;
struct nueffecttex_s;
struct nufnt_s;
struct nuframebuffer_s;
struct nugeom_s;
struct nugscn_s;
struct nuhspecial_s;
struct numemblk_s;
struct numtl_s;
struct numtx_s;
struct nunativetex_s;
struct nunetaddr_s;
struct nupad_s;
struct nushaderprogram_s;
struct nutex_s;
struct nutexturetype_e;
struct nuvec4_s;
struct nuvec_s;
struct rndrstream_s;
struct variptr_u;

struct NUAPPLICATIONSTATUS {};
struct NUGCUTCHAR_s {};
struct NUGCUTLOCATOR_s {};
struct NUGCUTRIGID_s {};
struct NUGCUTSCENE_s {};
struct NUJOINTANIM_s {};
struct NUPADMOTIONVALUE {};
struct NUPADTYPE {};
struct NUTEXFORMAT {};
struct NUTHREADCAFECORE {};
struct NUTHREADXBOX360CORE {};
struct NuFadeObjGType {};
struct NuIOS_InAppProduct {};
struct NuInputTouchData {};
struct NuSoundBuffer {};
struct NuSoundSource {};
struct NuSymbolQuery {};
struct NuWindGType {};
struct SLPlayItf_ {};
struct ShaderObjectKey {};
struct VuMtx {};
struct VuVec {};
struct _vuv_s {};
struct ani3_animheader_s {};
struct d3dsamplerstate_u {};
struct nuanimbuff_s {};
struct nuanimdatachunk_s {};
struct nucamera_s {};
struct nudeferredshadingenum_e {};
struct nudisplaylistitem_s {};
struct nudisplayscene_s {};
struct nudynamiclight_s {};
struct nueffecttex_s {};
struct nufnt_s {};
struct nuframebuffer_s {};
struct nugeom_s {};
struct nugscn_s {};
struct nuhspecial_s {};
struct numemblk_s {};
struct numtl_s {};
struct numtx_s {};
struct nunativetex_s {};
struct nunetaddr_s {};
struct nupad_s {};
struct nushaderprogram_s {};
struct nutex_s {};
struct nutexturetype_e {};
struct nuvec4_s {};
struct nuvec_s {};
struct rndrstream_s {};
struct variptr_u {};

struct NetSmallStats {
    struct eInfo {};
};
struct NuApplicationState {
    void SetStatus(NUAPPLICATIONSTATUS);
    ~NuApplicationState();
};
struct NuButtonLayout {
    void ActivateLayout();
    void DeactivateLayout();
    void Render();
    void Update(NuInputTouchData const *);
    void UpdateButtons(i32);
    ~NuButtonLayout();
};
struct NuCopyFilter {
    void destroyResources();
    void initResources();
    void render(nuframebuffer_s *);
    void reset();
};
struct NuDataPortManager {
    void registerPort(char const *, void *);
};
struct NuDeferredFilter {
    void initResources();
};
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
struct NuDeviceSpecs {
    void Exists();
    NuDeviceSpecs();
    ~NuDeviceSpecs();
};
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
struct NuInputDevice {
    void DisableDPD();
    void EnableDPD();
    void GetAttachmentType() const;
    void GetCaps() const;
    void GetIndexByType() const;
    void GetLastValidIndexByType() const;
    void GetLastValidType() const;
    void GetMotionValue(NUPADMOTIONVALUE) const;
    void GetMouseData() const;
    void GetPort() const;
    void GetTouchData() const;
    void GetVolume() const;
    void HasHeadphonesConnected() const;
    void IsButtonPressed(u32) const;
    void IsIntercepted() const;
    void KillRumble();
    void ProcessTouchData();
    void SetMotors(float, float);
    void SupportsCaps(u32) const;
    ~NuInputDevice();
};
struct NuInputDevicePS {
    void DisableDPDPS(u32);
    void EnableDPDPS(u32);
    void GetIdentifierPS(u32);
    void HandleGamePadAxis_ANDROID_SPECIFIC(float, float, float, float, float, float);
    void HandleSensor_ANDROID_SPECIFIC(i32, float, float, float);
    void HandleTouch_ANDROID_SPECIFIC(i32, i32, i32, float, float);
};
struct NuInputManager {
    void GetDevice(u32) const;
    void GetFirstDeviceByType(NUPADTYPE) const;
    void KillRumbleAll();
};
struct NuMainFilter {
    void initResources();
};
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
struct NuMemoryPool {
    struct FreeBlock {};
    struct IEventHandler {};
    struct IVisitor {};
    struct Page {};
    void GetAllocatedBytes();
    void GetDebugName() const;
    void GetFreeBytes();
    void GetLargeBlockBytes();
    void GetPagedBytes();
    void InterlockedPop(NuMemoryPool::FreeBlock volatile **);
    void InterlockedPush(NuMemoryPool::FreeBlock volatile **, void *);
    void Merge(NuMemoryPool::FreeBlock volatile *, NuMemoryPool::FreeBlock volatile *);
    void Merge(NuMemoryPool::Page *, NuMemoryPool::Page *);
    void MergeSort(NuMemoryPool::FreeBlock volatile *, u32);
    void MergeSort(NuMemoryPool::Page *, u32);
    NuMemoryPool(NuMemoryPool::IEventHandler *, u32, char const *);
    void PageAlloc(u32, char const *);
    void ReleaseAllPages();
    void ReleaseUnreferencedPages();
    void ReleaseUnreferencedPages_OLD();
    void VisitPools(NuMemoryPool::IVisitor *);
    ~NuMemoryPool();
};
struct NuMemoryManager {
    struct Context {};
    struct DebugHeader {};
    struct ErrorCode {};
    struct FreeHeader {};
    struct Header {};
    struct IPageVisitor {};
    struct IVisitor {};
    void ClearBlockDebugContext(void *);
    void DumpBlock(u32, NuSymbolQuery *, NuMemoryManager::Header *, u32, u32, u32);
    void DumpBlocksForContext(u32, NuSymbolQuery *, NuMemoryManager::Context *, u32);
    void FindAndTouchMatchingBlocks(NuMemoryManager::DebugHeader *, u32 *, u32);
    void GetAllocatedBytes();
    void GetBlockAlignment(void *);
    void GetBlockDebugBackTrace(void *, void **);
    void GetBlockDebugContext(void *);
    void GetBlockSize(void *);
    void GetCategoryAllocatedBytes(u16);
    void GetCurrentContextID() const;
    void GetCurrentContextName() const;
    void GetDebugName() const;
    void GetFreeBytes() const;
    void GetNumFreeFragments() const;
    void GetOverrideCategory();
    void GetOverrideCategoryBGThread();
    void GetPagedBytes();
    void GetSmallBinSize(u32);
    void IsZombie();
    void MergeLargeBinSegments(NuMemoryManager::FreeHeader *, NuMemoryManager::FreeHeader *);
    void PushContext(char const *);
    void ReleaseExternalPage(void *);
    void SetBlockDebugContext(void *, u32);
    void SetBlockDebugName(void *, char const *);
    void SetOverrideCategory(u16);
    void SetOverrideCategoryBGThread(u16);
    void SortLargeBin(u32);
    void SortLargeBinSegment(NuMemoryManager::FreeHeader *, u32);
    void UnTouchAllBlocks();
    void ValidateBlock(void *);
    void ValidateBlockDeferredContent(NuMemoryManager::Header *, char const *);
    void VisitManagers(NuMemoryManager::IVisitor *);
    void VisitPages(NuMemoryManager::IPageVisitor *);
    void _MultiBlockAlloc(u32, u32, u32, void **, u32, char const *, u16);
};
struct NuMemory {
    struct MemErrorHandler {
        void CloseDump(NuMemoryManager *, u32);
        void Dump(NuMemoryManager *, u32, char const *);
        void HandleError(NuMemoryManager *, NuMemoryManager::ErrorCode, char const *);
        void OpenDump(NuMemoryManager *, char const *, u32 &);
    };
    void CreateDynamicMemoryPool(u32, char const *);
    void CreateFixedMemoryPool(u32, u32, char const *);
    void CreateMemoryPool(NuMemoryPool::IEventHandler *, u32, char const *);
    void DestroyMemoryManager(NuMemoryManager *);
    void DestroyMemoryPool(NuMemoryPool *);
    void MoveFreeMem2IntoMem1();
    void SetSoakTestMode();
};
struct NuMotionAccumFilter {
    void initResources();
};
struct NuMotionAccumFilterGen {
    void GetTiming(i32 *);
    NuMotionAccumFilterGen();
    void destroyResources();
    void destroyTextureResources();
    void initResources();
    void initTextureResources(i32, i32);
    void render();
};
struct NuMotionFilter {
    void initResources();
};
struct NuMotionFilterGen {
    NuMotionFilterGen();
    void render();
};
struct NuMusic {
    struct Track {
        void ManageEntryTime();
        void SetEntryTime(float);
    };
    struct Album {
        void GetTracks(u32, NuMusic::Track **);
    };
    struct VOICE_STATUS {};
    struct Voice {
        void Cue();
        void Load(NuMusic::Track *, i32);
        void SetStatusFn(i32, i32);
        void Unload();
    };
    void ClassToName(u32);
    void CueTrack(u32);
    void Debug(i32, i32);
    void FindVoiceByClassAndStatus(u32, NuMusic::VOICE_STATUS);
    void FindVoiceByTrack(NuMusic::Track *);
    void GetAlbumHandle(char const *);
    void GetPlaybackTime(u32);
    void GetPlayer();
    void GetStatus(u32, i32 *);
    void NoMusic(i32);
    NuMusic();
    void PauseTrack(u32);
    void PlayTrack(u32, u32);
    void PlayTrackI(u32, u32);
    void Process(float);
    void ResumeTrack(u32);
    void SelectTrack(u32, char const *);
    void SetAlbum(char const *);
    void SetAlbum(i32);
    void SetClassVolume(u32, float);
    void SetFader(float, float);
    void SetMasterVolume(float);
    void SetTrackEntryTimeByClass(u32, float);
    void StopTrack(u32, i32);
    ~NuMusic();
};
struct NuNetEmu {
    struct EmuPacket {
        void AddPayload(void *, i32);
        EmuPacket(nunetaddr_s *);
        ~EmuPacket();
    };
    struct PackStats {
        void Draw(float, float, float, float, NetSmallStats::eInfo) const;
    };
    struct eConditions {};
    void FindPacket(nunetaddr_s *, i32);
    NuNetEmu();
    void RecvFrom(void *, i32, nunetaddr_s &);
    void SendTo(void *, i32, nunetaddr_s *, i32);
    void SetConditions(NuNetEmu::eConditions);
    void SplitSendPacket(NuNetEmu::EmuPacket *);
    void Update();
};
struct NuPlatform {
    void Destroy();
    void Exists();
    NuPlatform();
    ~NuPlatform();
};
struct NuPostFilter {
    void initSharedResources(i32, i32);
    void renderFrustum(numtx_s *);
};
struct NuPostFilterGen {
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
};
struct NuRenderDevice {
    void DetermineNominalAspectRatio(u32, u32) const;
    void IsContextValid() const;
    void MultiThreadRender() const;
    void OnAppPaused();
    void OnAppRestarted();
    void OnAppResume();
    void OnAppStarted();
    void OnAppStopped();
    void OnGainedFocus();
    void OnLostFocus();
    void OnWindowDestroy();
    void OpenglErrorCallback(u32, u32, u32, u32, i32, char const *, void *);
    void PreInitialize();
    void ResizeDevice(i32, i32, i32, bool, bool, bool, bool);
};
struct NuSpeedBlurFilter {
    void initResources();
};
struct NuSpeedBlurFilterGen {
    NuSpeedBlurFilterGen();
    void computeSpeedBlur(VuVec &);
    void destroyTextureResources();
    void initTextureResources(i32, i32);
    void render();
};
struct NuThread {
    void Resume();
    void SetDebugName(char const *);
};
struct NuThreadBase {
    void GetDebugName() const;
};
struct NuThreadManager {
    void CreateThreadSuspended(void (*)(void *), void *, i32, char const *, i32, NUTHREADCAFECORE, NUTHREADXBOX360CORE);
    void FreeTLS(i32);
    void GetMainThread();
};
struct NuTouchInputButton {
    NuTouchInputButton(i32, u32);
    NuTouchInputButton(i32, u32, float, float, float, float);
    void Render();
    void Update(NuInputTouchData const *);
};
struct NuTouchInputElement {
    struct TYPE {};
    NuTouchInputElement(NuTouchInputElement::TYPE, i32, u32);
    NuTouchInputElement(NuTouchInputElement::TYPE, i32, u32, float, float, float, float);
};
struct NuTouchInputStick {
    NuTouchInputStick(NuTouchInputElement::TYPE, i32, u32, float, float, float, float);
    void Render();
    void Update(NuInputTouchData const *);
};
struct NuVirtualTouchDevice {
    void GetAspectRatio();
    void Render();
    void SetCurrentLayoutIndex(u32);
};
struct NuVoiceAndroid {
    void ApplyHardwareVoiceMix();
    void CreateHardwareVoice();
    void DestroyHardwareVoice();
    void GetInterfaces();
    void GetPlaybackPositionSamples();
    NuVoiceAndroid(NuSoundSource *, bool);
    void OnPlayerEvent(u32);
    void PauseHardwareVoice();
    void PlayerCallback(SLPlayItf_ const *const *, void *, u32);
    void RealiseObject();
    void ResumeHardwareVoice();
    void StartHardwareVoice();
    void StopHardwareVoice();
    void SubmitBuffer(NuSoundBuffer *);
    void UpdateHardwareVoice(float);
    void UpdateQueue();
    void UpdateSamplePlaybackCount();
    void UpdateState();
    virtual ~NuVoiceAndroid();
};

#endif // NU2API_NUCORE_TYPES_H