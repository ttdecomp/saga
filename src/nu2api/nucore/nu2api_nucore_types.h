#ifndef NU2API_NUCORE_TYPES_H
#define NU2API_NUCORE_TYPES_H
#pragma once

#include "nu2api/nucore/fixed_width.h"

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
#include "nu2api/nucore/NuTouchInputButton.h"
#include "nu2api/nucore/NuTouchInputElement.h"
#include "nu2api/nucore/NuTouchInputStick.h"
#include "nu2api/nucore/NuVoiceAndroid.h"

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

struct NuApplicationState {
    void SetStatus(NUAPPLICATIONSTATUS);
    ~NuApplicationState();
};
struct NuButtonLayout;
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
struct NuVirtualTouchDevice {
    void GetAspectRatio();
    void Render();
    void SetCurrentLayoutIndex(u32);
};
#endif // NU2API_NUCORE_TYPES_H
