#include "MechInputTouch_types.h"

void MechTouchUI::AddUIElement(MechTouchUIElement &) {
}

void MechTouchUI::Init() {
}

MechTouchUI::MechTouchUI() {
}

void MechTouchUI::OnClick(GameObject_s &, TouchHolder &) {
}

void MechTouchUI::OnDoubleClick(GameObject_s &, TouchHolder &) {
}

void MechTouchUI::OnDown(GameObject_s &, TouchHolder &) {
}

void MechTouchUI::OnHold(GameObject_s &, TouchHolder &) {
}

void MechTouchUI::OnRelease(GameObject_s &, TouchHolder &) {
}

void MechTouchUI::PickElement(NuVec2 &) {
}

void MechTouchUI::Process(float) {
}

void MechTouchUI::RemoveUIElement(MechTouchUIElement &) {
}

void MechTouchUI::Render() {
}

MechTouchUI::~MechTouchUI() {
}

MechTouchUICharIcon::MechTouchUICharIcon(MechTouchUIPartySelector &, VuVec const &, i32, float) {
}

void MechTouchUICharIcon::Process(float) {
}

void MechTouchUICharIcon::Render() {
}

void MechTouchUICharIcon::SetupDisabled() {
}

void MechTouchUITagButton::FadeOut() {
}

MechTouchUITagButton::MechTouchUITagButton(GameObject_s &, TouchHolder &) {
}

void MechTouchUITagButton::Process(float) {
}

void MechTouchUITagButton::Render() {
}

MechTouchUITagButton::~MechTouchUITagButton() {
}

MechTouchUITexButton::MechTouchUITexButton(VuVec const &, float) {
}

void MechTouchUITexButton::Process(float) {
}

void MechTouchUITexButton::Render() {
}

void MechTouchUITexButton::UpdateTexture(i16) {
}

MechTouchUITexButton::~MechTouchUITexButton() {
}

MechTouchUIPauseButton::MechTouchUIPauseButton() {
}

void MechTouchUIPauseButton::Process(float) {
}

void MechTouchUIPauseButton::Render() {
}

MechTouchUIPlayerButton::MechTouchUIPlayerButton() {
}

void MechTouchUIPlayerButton::Process(float) {
}

void MechTouchUIPlayerButton::SetupTargetIds() {
}

void MechTouchUIPlayerButton::ShowChooser() {
}

void MechTouchUIPlayerButton::TriggerTagNext() {
}

void MechTouchUIPartySelector::BlendOut() {
}

void MechTouchUIPartySelector::BlendedOut() {
}

void MechTouchUIPartySelector::Cleanup() {
}

MechTouchUIPartySelector::MechTouchUIPartySelector(MechTouchUIPlayerButton &, i32 *) {
}

MechTouchUIPartySelector::~MechTouchUIPartySelector() {
}
