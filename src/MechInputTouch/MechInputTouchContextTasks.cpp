#include "MechInputTouch_types.h"

MechTouchTask::MechTouchTask(MechInputTouchGestureBasedController &) {
}

MechTouchTask::~MechTouchTask() {
}

MechTouchTaskTag::MechTouchTaskTag(MechInputTouchGestureBasedController &, GameObject_s &) {
}

void MechTouchTaskTag::Update() {
}

MechTouchTaskGoTo::MechTouchTaskGoTo(MechInputTouchGestureBasedController &, MechObjectInterface *) {
}

void MechTouchTaskGoTo::OnStart() {
}

void MechTouchTaskGoTo::OnStop() {
}

void MechTouchTaskGoTo::Render() {
}

void MechTouchTaskGoTo::Update() {
}

void MechTouchTaskGoTo::UpdateStuck() {
}

void MechTouchTaskGoTo::UpdateTarget(MechObjectInterface &) {
}

MechTouchTaskGoTo::~MechTouchTaskGoTo() {
}

MechTouchTaskJump::MechTouchTaskJump(MechInputTouchGestureBasedController &, JumpTriggerPacket const &, bool, bool) {
}

void MechTouchTaskJump::OnStop() {
}

void MechTouchTaskJump::Update() {
}

MechTouchTaskBlock::MechTouchTaskBlock(MechInputTouchGestureBasedController &) {
}

void MechTouchTaskBlock::Update() {
}

MechTouchTaskPanel::MechTouchTaskPanel(MechInputTouchGestureBasedController &, MechObjectInterface *, VuVec const &) {
}

void MechTouchTaskPanel::Update() {
}

MechTouchTaskAttack::MechTouchTaskAttack(MechInputTouchGestureBasedController &, MechObjectInterface *, VuVec const &) {
}

void MechTouchTaskAttack::OnStart() {
}

void MechTouchTaskAttack::OnStop() {
}

void MechTouchTaskAttack::Render() {
}

void MechTouchTaskAttack::Update() {
}

MechTouchTaskBigJump::MechTouchTaskBigJump(MechInputTouchGestureBasedController &, MechObjectInterface &, signed char) {
}

MechTouchTaskBigJump::MechTouchTaskBigJump(MechInputTouchGestureBasedController &, nuvec_s &, signed char) {
}

void MechTouchTaskBigJump::Update() {
}

MechTouchTaskBuildIt::MechTouchTaskBuildIt(MechInputTouchGestureBasedController &, MechObjectInterface *,
                                           VuVec const &) {
}

void MechTouchTaskBuildIt::Update() {
}

MechTouchTaskUseForce::MechTouchTaskUseForce(MechInputTouchGestureBasedController &, MechObjectInterface *,
                                             VuVec const &) {
}

void MechTouchTaskUseForce::OnStart() {
}

void MechTouchTaskUseForce::OnStop() {
}

void MechTouchTaskUseForce::Update() {
}

MechTouchTaskUseZipUp::MechTouchTaskUseZipUp(MechInputTouchGestureBasedController &) {
}

void MechTouchTaskUseZipUp::OnStart() {
}

void MechTouchTaskUseZipUp::Update() {
}

MechTouchTaskPullLever::MechTouchTaskPullLever(MechInputTouchGestureBasedController &, MechObjectInterface *,
                                               VuVec const &) {
}

void MechTouchTaskPullLever::Update() {
}

MechTouchTaskHatMachine::MechTouchTaskHatMachine(MechInputTouchGestureBasedController &, MechObjectInterface *,
                                                 VuVec const &) {
}

void MechTouchTaskHatMachine::Update() {
}

void MechTouchTaskPlannedGoTo::AnalysePath() {
}

void MechTouchTaskPlannedGoTo::BackgroundProcess() {
}

void MechTouchTaskPlannedGoTo::GenerateWaypoints() {
}

MechTouchTaskPlannedGoTo::MechTouchTaskPlannedGoTo(MechInputTouchGestureBasedController &, MechObjectInterface *,
                                                   bool *) {
}

void MechTouchTaskPlannedGoTo::OnResume() {
}

void MechTouchTaskPlannedGoTo::OnStart() {
}

void MechTouchTaskPlannedGoTo::OnStop() {
}

void MechTouchTaskPlannedGoTo::SetupForAnalysis() {
}

void MechTouchTaskPlannedGoTo::Update() {
}

MechTouchTaskPlannedGoTo::~MechTouchTaskPlannedGoTo() {
}

MechTouchTaskUseTeleport::MechTouchTaskUseTeleport(MechInputTouchGestureBasedController &, MechObjectInterface *,
                                                   VuVec const &) {
}

void MechTouchTaskUseTeleport::Update() {
}

MechTouchTaskAstroJetPack::MechTouchTaskAstroJetPack(MechInputTouchGestureBasedController &) {
}

void MechTouchTaskAstroJetPack::Update() {
}

void MechTouchTaskPlannedDoubleClickGoTo::BackgroundProcess() {
}

MechTouchTaskPlannedDoubleClickGoTo::MechTouchTaskPlannedDoubleClickGoTo(MechInputTouchGestureBasedController &,
                                                                         MechObjectInterface *) {
}

void MechTouchTaskPlannedDoubleClickGoTo::OnResume() {
}

void MechTouchTaskPlannedDoubleClickGoTo::OnStart() {
}

void MechTouchTaskPlannedDoubleClickGoTo::OnStop() {
}

void MechTouchTaskPlannedDoubleClickGoTo::Update() {
}

MechTouchTaskPlannedDoubleClickGoTo::~MechTouchTaskPlannedDoubleClickGoTo() {
}
