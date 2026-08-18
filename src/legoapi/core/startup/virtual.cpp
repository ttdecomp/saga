#include "legoapi/legoapi_types.h"

void VirtualControlDPad::Process(float) {
}

void VirtualControlDPad::Render() {
}

VirtualControlDPad::VirtualControlDPad(NuVec2 const &, float, MechInputTouchVirtualConsoleController &) {
}

VirtualControlDPad::~VirtualControlDPad() {
}

void VirtualControlButton::Process(float) {
}

void VirtualControlButton::Render() {
}

VirtualControlButton::VirtualControlButton(NuVec2 const &, float, MechInputTouchMainController::eButtonTypes) {
}

void VirtualControlButtonMover::Process(float) {
}

VirtualControlButtonMover::VirtualControlButtonMover(MechInputTouchVirtualConsoleController &) {
}

void VirtualControlDPad_LockButton::Process(float) {
}

void VirtualControlDPad_LockButton::Render() {
}

VirtualControlDPad_LockButton::VirtualControlDPad_LockButton(VirtualControlDPad &) {
}
