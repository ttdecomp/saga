#ifndef MECHINPUTTOUCH_TYPES_H
#define MECHINPUTTOUCH_TYPES_H
#pragma once

#include "nu2api/nucore/fixed_width.h"
#include "legoapi/SwipeDecalRenderer.h"
#include "nu2api/nucore/NuTouchInputElement.h"

struct AIPATHCNX_s;
struct AIPATH_s;
struct AISYS_s;
struct GameObject_s;
struct JumpTriggerPacket;
struct MechAddon;
struct MechAutoJumpConnection;
struct MechAutoJumpManager;
struct MechAutofireAddon;
struct MechEdgeStopAddon;
struct MechInputTouchBonusCavalryController;
struct MechInputTouchButton;
struct MechInputTouchButtonControlled;
struct MechInputTouchButtonFaker;
struct MechInputTouchDeathStarTurretController;
struct MechInputTouchGestureBasedController;
struct MechInputTouchGestureTracker;
struct MechInputTouchGestureTrackingSystem;
struct MechInputTouchMainController;
struct MechInputTouchMainDummyButton;
struct MechInputTouchMainDummyStick;
struct MechInputTouchMenuController;
struct MechInputTouchPodraceController;
struct MechInputTouchSpeederChaseController;
struct MechInputTouchSystem;
struct MechInputTouchVirtualConsoleController;
struct MechJumpAutoPilotAddon;
struct MechObjectInterface;
struct MechSystems;
struct MechTempPosInterface;
struct MechTouchTask;
struct MechTouchTaskAstroJetPack;
struct MechTouchTaskAttack;
struct MechTouchTaskBigJump;
struct MechTouchTaskBlock;
struct MechTouchTaskBuildIt;
struct MechTouchTaskGoTo;
struct MechTouchTaskHatMachine;
struct MechTouchTaskJump;
struct MechTouchTaskPanel;
struct MechTouchTaskPlannedDoubleClickGoTo;
struct MechTouchTaskPlannedGoTo;
struct MechTouchTaskPullLever;
struct MechTouchTaskTag;
struct MechTouchTaskUseForce;
struct MechTouchTaskUseTeleport;
struct MechTouchTaskUseZipUp;
struct MechTouchUI;
struct MechTouchUICharIcon;
struct MechTouchUIElement;
struct MechTouchUIPartySelector;
struct MechTouchUIPauseButton;
struct MechTouchUIPlayerButton;
struct MechTouchUITagButton;
struct MechTouchUITexButton;
struct NuInputTouch;
struct NuInputTouchData;
struct NuTouchInputElement;
struct NuVec2;
struct NuVirtualTouchDevice;
struct SwipeDecalRenderer;
struct ThingProcessData;
struct ThingRenderData;
struct ThingResetData;
struct TouchHolder;
struct VuVec;
struct WORLDINFO_s;
struct nuvec_s;

struct AIPATHCNX_s;
struct AIPATH_s;
struct AISYS_s;
struct JumpTriggerPacket {};
struct MechAutoJumpConnection {};
struct MechInputTouchGestureTracker {};
struct MechTouchUIElement {};
struct NuInputTouch;
struct NuInputTouchData;
struct NuVec2 {};
struct NuVirtualTouchDevice;
struct ThingProcessData {};
struct ThingRenderData {};
struct ThingResetData {};
struct TouchHolder {};
struct VuVec;
struct WORLDINFO_s;
struct nuvec_s;

struct MechAddon {
    struct ProcessStage {};
};
struct MechAutoJumpManager {
    void AddAutoJumpConnection(AIPATH_s *, AIPATHCNX_s *, i32, bool, i32, bool);
    void DeleteJumpConnection(MechAutoJumpConnection *);
    void DeleteJumpConnectionsAndStreaks();
    void Init();
    MechAutoJumpManager(AISYS_s *);
    void PreProcessJumpConnections();
    void Process();
    void ProcessJumpConnections();
    void Render();
    virtual ~MechAutoJumpManager();
};
struct MechAutofireAddon {
    MechAutofireAddon(MechObjectInterface &);
    void OnProcess(MechAddon::ProcessStage, float);
    virtual ~MechAutofireAddon();
};
struct MechEdgeStopAddon {
    MechEdgeStopAddon(MechObjectInterface &);
    void OnProcess(MechAddon::ProcessStage, float);
    virtual ~MechEdgeStopAddon();
};
struct MechInputTouchBonusCavalryController {
    void Activate();
    void Deactivate();
    MechInputTouchBonusCavalryController(i32);
    void OnDown(GameObject_s &, TouchHolder &);
    void OnRelease(GameObject_s &, TouchHolder &);
    void Update(NuInputTouchData const *);
    virtual ~MechInputTouchBonusCavalryController();
};
struct MechInputTouchButton {
    void ClearTouchLocked(bool);
    void CouldTouchBeLockedBy(u32);
    void FindPossibleTriggeringIndexFromID(u32);
    MechInputTouchButton(NuTouchInputElement::TYPE, i32, i32);
    MechInputTouchButton(i32, u32, float, float, float, float, i32);
    void Reset();
    void SetTouchLocked(u32, bool);
};
struct MechInputTouchButtonControlled {
    MechInputTouchButtonControlled(MechInputTouchMainController &, i32);
};
struct MechInputTouchButtonFaker {
    MechInputTouchButtonFaker(i32, u32, float, float, float, float);
    void Render();
    void Update(NuInputTouchData const *);
};
struct MechInputTouchDeathStarTurretController {
    void Activate();
    void Deactivate();
    MechInputTouchDeathStarTurretController(i32);
    void OnDown(GameObject_s &, TouchHolder &);
    void OnRelease(GameObject_s &, TouchHolder &);
    void OnSwipe(GameObject_s &, TouchHolder &, i32);
    void Update(NuInputTouchData const *);
    virtual ~MechInputTouchDeathStarTurretController();
};
struct MechInputTouchGestureBasedController {
    struct StickMode {};
    void Activate();
    void Deactivate();
    void KillTasks(bool);
    MechInputTouchGestureBasedController(i32, MechInputTouchGestureBasedController::StickMode);
    void MenuDisable();
    void OnClick(GameObject_s &, TouchHolder &);
    void OnDoubleClick(GameObject_s &, TouchHolder &);
    void OnDown(GameObject_s &, TouchHolder &);
    void OnHold(GameObject_s &, TouchHolder &);
    void OnRelease(GameObject_s &, TouchHolder &);
    void OnSwipe(GameObject_s &, TouchHolder &, i32);
    void PerformCloseMechanic(GameObject_s &, TouchHolder &);
    void ProcessAutoJumpOverGap(GameObject_s *);
    void ProcessAutoJumpWhenStuck(GameObject_s &);
    void ProcessDragMovement(GameObject_s &);
    void Render();
    void StartJumpUsingAIPath(JumpTriggerPacket const &, i32);
    void StartNewTask(MechTouchTask *, TouchHolder &, bool, bool);
    void TriggerJumpTask(JumpTriggerPacket const &, bool, bool, bool);
    void Update(NuInputTouchData const *);
    virtual ~MechInputTouchGestureBasedController();
};
struct MechInputTouchGestureTrackingSystem {
    void GetTouch(NuInputTouch const &);
    void LookForClicks(GameObject_s &);
    void LookForDown(GameObject_s &);
    void LookForGestures(GameObject_s &);
    void LookForHold(GameObject_s &);
    void LookForRelease(GameObject_s &);
    void LookForSwipe(GameObject_s &);
    MechInputTouchGestureTrackingSystem();
    void Process(GameObject_s &, NuInputTouchData const &);
    void ReadData(GameObject_s &, NuInputTouchData const &);
    void RegisterGestureTracker(MechInputTouchGestureTracker &, i32);
    void UnregisterGestureTracker(MechInputTouchGestureTracker &);
    void Update(NuInputTouchData const *);
    virtual ~MechInputTouchGestureTrackingSystem();
};
struct MechInputTouchMainController {
    struct eButtonTypes {};
    MechInputTouchMainController(i32);
    void RemoveUnpressedButtons(NuInputTouchData &, NuInputTouchData const &);
    void Render();
    void ResetButtons();
    void Update(NuInputTouchData const *);
    void UpdateButtons();
    virtual ~MechInputTouchMainController();
};
struct MechInputTouchMainDummyButton {
    void IsPressed() const;
    MechInputTouchMainDummyButton(MechInputTouchMainController &, u32, MechInputTouchMainController::eButtonTypes);
};
struct MechInputTouchMainDummyStick {
    MechInputTouchMainDummyStick(MechInputTouchMainController &, NuTouchInputElement::TYPE);
};
struct MechInputTouchMenuController {
    void Activate();
    void Deactivate();
    MechInputTouchMenuController(i32);
    void OnClick(GameObject_s &, TouchHolder &);
    void OnDoubleClick(GameObject_s &, TouchHolder &);
    void OnDown(GameObject_s &, TouchHolder &);
    void OnHold(GameObject_s &, TouchHolder &);
    void OnRelease(GameObject_s &, TouchHolder &);
    void OnSwipe(GameObject_s &, TouchHolder &, i32);
    void Render();
    void Update(NuInputTouchData const *);
    void UpdateButtons(i32);
    virtual ~MechInputTouchMenuController();
};
struct MechInputTouchPodraceController {
    void Activate();
    void Deactivate();
    MechInputTouchPodraceController(i32);
    void OnDown(GameObject_s &, TouchHolder &);
    void OnRelease(GameObject_s &, TouchHolder &);
    void Update(NuInputTouchData const *);
    virtual ~MechInputTouchPodraceController();
};
struct MechInputTouchSpeederChaseController {
    void Activate();
    void Deactivate();
    void IsDownSwipe(NuVec2 const &, NuVec2 const &);
    void IsSwipeAgainstDirection(NuVec2 const &, NuVec2 const &, bool);
    void IsSwipeWithDirection(NuVec2 const &, NuVec2 const &, bool);
    void IsUpSwipe(NuVec2 const &, NuVec2 const &);
    MechInputTouchSpeederChaseController(i32);
    void OnClick(GameObject_s &, TouchHolder &);
    void OnDoubleClick(GameObject_s &, TouchHolder &);
    void OnDown(GameObject_s &, TouchHolder &);
    void OnRelease(GameObject_s &, TouchHolder &);
    void OnSwipe(GameObject_s &, TouchHolder &, i32);
    void Update(NuInputTouchData const *);
    virtual ~MechInputTouchSpeederChaseController();
};
struct MechInputTouchSystem {
    void AddChangeLayoutButtons(NuVirtualTouchDevice &, i32);
    void ChooseTouchLayout(bool);
    void ConvertToScreenCoords(float, float, float &, float &);
    void CouldTouchBeLockedBy(u32, MechInputTouchButton *);
    void CreateGamePanels();
    void CreateGamePlayLayoutBlank(NuVirtualTouchDevice &, i32);
    void CreateGamePlayLayoutConsoleMode(NuVirtualTouchDevice &, i32);
    void CreateGamePlayLayoutGestureBased(NuVirtualTouchDevice &, i32);
    void CreateGamePlayLayoutGestureBased_Cavalry(NuVirtualTouchDevice &, i32);
    void CreateGamePlayLayoutGestureBased_DeathStarTurret(NuVirtualTouchDevice &, i32);
    void CreateGamePlayLayoutGestureBased_Podrace(NuVirtualTouchDevice &, i32);
    void CreateGamePlayLayoutGestureBased_SpeederChase(NuVirtualTouchDevice &, i32);
    void DetermineMoveDir2D(GameObject_s &, VuVec const &, bool, VuVec &);
    void FindTargetForce(WORLDINFO_s *, GameObject_s &, VuVec const &, VuVec const &, float &, MechObjectInterface *&,
                         bool &, bool);
    void FindTargetObject(GameObject_s &, VuVec const &, i32, MechObjectInterface *, MechTempPosInterface *);
    void Init();
    MechInputTouchSystem();
    void ProcessEvenWhenPaused(ThingProcessData *);
    void ResetAllOwners();
    void SetTouchLockedBy(u32, MechInputTouchButton *, bool);
    void TouchLockedBy(u32);
};
struct MechInputTouchVirtualConsoleController {
    void Activate();
    void Deactivate();
    void LoadPerm();
    MechInputTouchVirtualConsoleController(i32);
    void OnDown(GameObject_s &, TouchHolder &);
    void OnRelease(GameObject_s &, TouchHolder &);
    void ProcessDragMovement(GameObject_s &);
    void ResetButtonPositionsToDefault();
    void ShouldBeActive();
    void Update(NuInputTouchData const *);
    void UpdateButtonPositions();
    void UpdateDPadPos();
    virtual ~MechInputTouchVirtualConsoleController();
};
struct MechJumpAutoPilotAddon {
    void AnalyseJumpTrajectory();
    void CalculateModifiedJumpTrajectory();
    void LookForBottomInt(VuVec const &);
    void LookForLandingPoint();
    void LookForLandingSpotAroundPoint(VuVec const &);
    void LookForTerrInt(VuVec const &);
    MechJumpAutoPilotAddon(MechObjectInterface &);
    void ModifyJump();
    void OnProcess(MechAddon::ProcessStage, float);
    void ProcJumpingToCertainDoom();
    void Recalculate();
    virtual ~MechJumpAutoPilotAddon();
};
struct MechObjectInterface {
    void GetFloorTargetPos(VuVec &, i32) const;
};
struct MechSystems {
    void Display(ThingRenderData *);
    void EnterLevel(WORLDINFO_s *);
    void ExitLevel(WORLDINFO_s *);
    void FindMoveToMarkerAtPos(VuVec const &, bool);
    void Get();
    void HookUpClickToPressStart();
    void Init();
    void LoadPerm();
    MechSystems();
    void NewMoveToMarker(MechObjectInterface &);
    void NewRadarPulse(VuVec const &, bool);
    void NewSwipeMarker(TouchHolder &, i32, SwipeDecalRenderer::Style);
    void NewTagButton(GameObject_s &, TouchHolder &);
    void Process(ThingProcessData *);
    void ProcessEvenWhenPaused(ThingProcessData *);
    void ProcessOnlyWhenPaused(ThingProcessData *);
    void Render(ThingRenderData *);
    void RenderCurrentPlayerHighlight();
    void Reset(ThingResetData *);
    void UnhookClickToPressStart();
    virtual ~MechSystems();
};
struct MechTempPosInterface {
    void GetFloorTargetPos(VuVec &, i32) const;
    MechTempPosInterface(VuVec const &);
    MechTempPosInterface(nuvec_s const &);
};
struct MechTouchTask {
    MechTouchTask(MechInputTouchGestureBasedController &);
    virtual ~MechTouchTask();
};
struct MechTouchTaskAstroJetPack {
    MechTouchTaskAstroJetPack(MechInputTouchGestureBasedController &);
    void Update();
};
struct MechTouchTaskAttack {
    MechTouchTaskAttack(MechInputTouchGestureBasedController &, MechObjectInterface *, VuVec const &);
    void OnStart();
    void OnStop();
    void Render();
    void Update();
};
struct MechTouchTaskBigJump {
    MechTouchTaskBigJump(MechInputTouchGestureBasedController &, MechObjectInterface &, signed char);
    MechTouchTaskBigJump(MechInputTouchGestureBasedController &, nuvec_s &, signed char);
    void Update();
};
struct MechTouchTaskBlock {
    MechTouchTaskBlock(MechInputTouchGestureBasedController &);
    void Update();
};
struct MechTouchTaskBuildIt {
    MechTouchTaskBuildIt(MechInputTouchGestureBasedController &, MechObjectInterface *, VuVec const &);
    void Update();
};
struct MechTouchTaskGoTo {
    MechTouchTaskGoTo(MechInputTouchGestureBasedController &, MechObjectInterface *);
    void OnStart();
    void OnStop();
    void Render();
    void Update();
    void UpdateStuck();
    void UpdateTarget(MechObjectInterface &);
    virtual ~MechTouchTaskGoTo();
};
struct MechTouchTaskHatMachine {
    MechTouchTaskHatMachine(MechInputTouchGestureBasedController &, MechObjectInterface *, VuVec const &);
    void Update();
};
struct MechTouchTaskJump {
    MechTouchTaskJump(MechInputTouchGestureBasedController &, JumpTriggerPacket const &, bool, bool);
    void OnStop();
    void Update();
};
struct MechTouchTaskPanel {
    MechTouchTaskPanel(MechInputTouchGestureBasedController &, MechObjectInterface *, VuVec const &);
    void Update();
};
struct MechTouchTaskPlannedDoubleClickGoTo {
    void BackgroundProcess();
    MechTouchTaskPlannedDoubleClickGoTo(MechInputTouchGestureBasedController &, MechObjectInterface *);
    void OnResume();
    void OnStart();
    void OnStop();
    void Update();
    virtual ~MechTouchTaskPlannedDoubleClickGoTo();
};
struct MechTouchTaskPlannedGoTo {
    void AnalysePath();
    void BackgroundProcess();
    void GenerateWaypoints();
    MechTouchTaskPlannedGoTo(MechInputTouchGestureBasedController &, MechObjectInterface *, bool *);
    void OnResume();
    void OnStart();
    void OnStop();
    void SetupForAnalysis();
    void Update();
    virtual ~MechTouchTaskPlannedGoTo();
};
struct MechTouchTaskPullLever {
    MechTouchTaskPullLever(MechInputTouchGestureBasedController &, MechObjectInterface *, VuVec const &);
    void Update();
};
struct MechTouchTaskTag {
    MechTouchTaskTag(MechInputTouchGestureBasedController &, GameObject_s &);
    void Update();
};
struct MechTouchTaskUseForce {
    MechTouchTaskUseForce(MechInputTouchGestureBasedController &, MechObjectInterface *, VuVec const &);
    void OnStart();
    void OnStop();
    void Update();
};
struct MechTouchTaskUseTeleport {
    MechTouchTaskUseTeleport(MechInputTouchGestureBasedController &, MechObjectInterface *, VuVec const &);
    void Update();
};
struct MechTouchTaskUseZipUp {
    MechTouchTaskUseZipUp(MechInputTouchGestureBasedController &);
    void OnStart();
    void Update();
};
struct MechTouchUI {
    void AddUIElement(MechTouchUIElement &);
    void Init();
    MechTouchUI();
    void OnClick(GameObject_s &, TouchHolder &);
    void OnDoubleClick(GameObject_s &, TouchHolder &);
    void OnDown(GameObject_s &, TouchHolder &);
    void OnHold(GameObject_s &, TouchHolder &);
    void OnRelease(GameObject_s &, TouchHolder &);
    void PickElement(NuVec2 &);
    void Process(float);
    void RemoveUIElement(MechTouchUIElement &);
    void Render();
    ~MechTouchUI();
};
struct MechTouchUICharIcon {
    MechTouchUICharIcon(MechTouchUIPartySelector &, VuVec const &, i32, float);
    void Process(float);
    void Render();
    void SetupDisabled();
};
struct MechTouchUIPartySelector {
    void BlendOut();
    void BlendedOut();
    void Cleanup();
    MechTouchUIPartySelector(MechTouchUIPlayerButton &, i32 *);
    ~MechTouchUIPartySelector();
};
struct MechTouchUIPauseButton {
    MechTouchUIPauseButton();
    void Process(float);
    void Render();
};
struct MechTouchUIPlayerButton {
    MechTouchUIPlayerButton();
    void Process(float);
    void SetupTargetIds();
    void ShowChooser();
    void TriggerTagNext();
};
struct MechTouchUITagButton {
    void FadeOut();
    MechTouchUITagButton(GameObject_s &, TouchHolder &);
    void Process(float);
    void Render();
    virtual ~MechTouchUITagButton();
};
struct MechTouchUITexButton {
    MechTouchUITexButton(VuVec const &, float);
    void Process(float);
    void Render();
    void UpdateTexture(i16);
    virtual ~MechTouchUITexButton();
};

#endif // MECHINPUTTOUCH_TYPES_H