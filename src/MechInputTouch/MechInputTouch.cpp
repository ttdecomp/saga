#include "MechInputTouch_types.h"

i32 MechInputTouchSystem::s_baseControlMode = 1;
i32 MechInputTouchSystem::s_actualTouchMode = 2;

MechInputTouchSystem::~MechInputTouchSystem() {
}

char const *MechInputTouchSystem::GetName() {
    return "MechInputTouchSystem";
}

void MechAutoJumpGetBest(JumpTriggerPacket const &, i32) {
}

void MechAutoJumpSetIsUsing(GameObject_s &, MechAutoJumpConnection &) {
}

void MechHintUIButton_OnClick_Callback(MechTouchUIElement &, TouchHolder &) {
}

void MechTouchUITagButton_OnClick_Callback(MechTouchUIElement &, TouchHolder &) {
}

void MechTouchUIPauseButton_OnClick_Callback(MechTouchUIElement &, TouchHolder &) {
}

void MechTouchUIPartySelector_OnRelease_Callback(MechTouchUIElement &, TouchHolder &) {
}

void MechInputTouchSystem::AddChangeLayoutButtons(NuVirtualTouchDevice &, i32) {
}

void MechInputTouchSystem::ChooseTouchLayout(bool) {
}

void MechInputTouchSystem::ConvertToScreenCoords(float, float, float &, float &) {
}

void MechInputTouchSystem::CouldTouchBeLockedBy(u32, MechInputTouchButton *) {
}

void MechInputTouchSystem::CreateGamePanels() {
}

void MechInputTouchSystem::CreateGamePlayLayoutBlank(NuVirtualTouchDevice &, i32) {
}

void MechInputTouchSystem::CreateGamePlayLayoutConsoleMode(NuVirtualTouchDevice &, i32) {
}

void MechInputTouchSystem::CreateGamePlayLayoutGestureBased(NuVirtualTouchDevice &, i32) {
}

void MechInputTouchSystem::CreateGamePlayLayoutGestureBased_Cavalry(NuVirtualTouchDevice &, i32) {
}

void MechInputTouchSystem::CreateGamePlayLayoutGestureBased_DeathStarTurret(NuVirtualTouchDevice &, i32) {
}

void MechInputTouchSystem::CreateGamePlayLayoutGestureBased_Podrace(NuVirtualTouchDevice &, i32) {
}

void MechInputTouchSystem::CreateGamePlayLayoutGestureBased_SpeederChase(NuVirtualTouchDevice &, i32) {
}

void MechInputTouchSystem::DetermineMoveDir2D(GameObject_s &, VuVec const &, bool, VuVec &) {
}

void MechInputTouchSystem::FindTargetForce(WORLDINFO_s *, GameObject_s &, VuVec const &, VuVec const &, float &,
                                           MechObjectInterface *&, bool &, bool) {
}

void MechInputTouchSystem::FindTargetObject(GameObject_s &, VuVec const &, i32, MechObjectInterface *,
                                            MechTempPosInterface *) {
}

void MechInputTouchSystem::Init() {
}

MechInputTouchSystem::MechInputTouchSystem() {
}

void MechInputTouchSystem::ProcessEvenWhenPaused(ThingProcessData *) {
}

void MechInputTouchSystem::ResetAllOwners() {
}

void MechInputTouchSystem::SetTouchLockedBy(u32, MechInputTouchButton *, bool) {
}

void MechInputTouchSystem::TouchLockedBy(u32) {
}
