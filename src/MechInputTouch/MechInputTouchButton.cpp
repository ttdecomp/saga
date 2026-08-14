#include "MechInputTouch_types.h"

void MechInputTouchButton::ClearTouchLocked(bool) {
}

void MechInputTouchButton::CouldTouchBeLockedBy(u32) {
}

void MechInputTouchButton::FindPossibleTriggeringIndexFromID(u32) {
}

MechInputTouchButton::MechInputTouchButton(NuTouchInputElement::TYPE, i32, i32) {
}

MechInputTouchButton::MechInputTouchButton(i32, u32, float, float, float, float, i32) {
}

void MechInputTouchButton::Reset() {
}

void MechInputTouchButton::SetTouchLocked(u32, bool) {
}

MechInputTouchButtonFaker::MechInputTouchButtonFaker(i32, u32, float, float, float, float) {
}

void MechInputTouchButtonFaker::Render() {
}

void MechInputTouchButtonFaker::Update(NuInputTouchData const *) {
}

MechInputTouchMainDummyStick::MechInputTouchMainDummyStick(MechInputTouchMainController &, NuTouchInputElement::TYPE) {
}

void MechInputTouchMainDummyButton::IsPressed() const {
}

MechInputTouchMainDummyButton::MechInputTouchMainDummyButton(MechInputTouchMainController &, u32,
                                                             MechInputTouchMainController::eButtonTypes) {
}

MechInputTouchButtonControlled::MechInputTouchButtonControlled(MechInputTouchMainController &, i32) {
}
