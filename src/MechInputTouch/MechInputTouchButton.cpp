#include <stddef.h>

#include "MechInputTouch_types.h"

void MechInputTouchButton::ClearTouchLocked(bool force) {
    if (!force) {
        MechSystems::Get()->input_touch_system.SetTouchLockedBy(touch_locked_by, NULL, false);
    }
    touch_locked_by = 0xff;
}

void MechInputTouchButton::CouldTouchBeLockedBy(u32) {
}

void MechInputTouchButton::FindPossibleTriggeringIndexFromID(u32) {
}

MechInputTouchButton::MechInputTouchButton(NuTouchInputElement::TYPE type, i32 index, i32 id)
    : NuTouchInputElement(type, index, id) {
}

MechInputTouchButton::MechInputTouchButton(i32, u32, float, float, float, float, i32)
    : NuTouchInputElement(NuTouchInputElement::TYPE(), 0, 0) {
}

__attribute__((weak)) MechInputTouchButton::~MechInputTouchButton() {
}

__attribute__((weak)) void MechInputTouchButton::Render() {
}

__attribute__((weak)) void MechInputTouchButton::Update(NuInputTouchData const *) {
}

__attribute__((weak)) char const *MechInputTouchButton::GetName() {
    return "UNKNOWN";
}

__attribute__((weak)) char const *MechInputTouchButton::GetDebugText() {
    return "";
}

void MechInputTouchButton::Reset() {
    if (has_pending_touches) {
        possible_triggering_touch_ids[0] = 0xff;
        possible_triggering_touches[0] = false;
        possible_triggering_touch_ids[1] = 0xff;
        possible_triggering_touches[1] = false;
        possible_triggering_touch_ids[2] = 0xff;
        possible_triggering_touches[2] = false;
        possible_triggering_touch_ids[3] = 0xff;
        possible_triggering_touches[3] = false;
        possible_triggering_touch_ids[4] = 0xff;
        possible_triggering_touches[4] = false;
        possible_triggering_touch_ids[5] = 0xff;
        possible_triggering_touches[5] = false;
        possible_triggering_touch_ids[6] = 0xff;
        possible_triggering_touches[6] = false;
        possible_triggering_touch_ids[7] = 0xff;
        possible_triggering_touches[7] = false;
        possible_triggering_touch_ids[8] = 0xff;
        possible_triggering_touches[8] = false;
        possible_triggering_touch_ids[9] = 0xff;
        possible_triggering_touches[9] = false;
        has_pending_touches = false;
    }

    if (touch_locked_by != 0xff) {
        ClearTouchLocked(false);
    }
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

MechInputTouchMainDummyButton::MechInputTouchMainDummyButton(MechInputTouchMainController &, u32,
                                                             MechInputTouchMainController::eButtonTypes) {
}

MechInputTouchButtonControlled::MechInputTouchButtonControlled(MechInputTouchMainController &, i32 index)
    : MechInputTouchButton(NuTouchInputElement::TYPE(), 2, 0), controller_index(index) {
}

__attribute__((weak)) MechInputTouchButtonControlled::~MechInputTouchButtonControlled() {
}

__attribute__((weak)) bool MechInputTouchButtonControlled::ControlledUpdate(NuInputTouchData const *) {
    return has_pending_touches;
}

__attribute__((weak)) void MechInputTouchButtonControlled::ControlledRender() {
}

__attribute__((weak)) void MechInputTouchButtonControlled::Reset() {
    ControlledReset();
}

__attribute__((weak)) void MechInputTouchButtonControlled::ControlledReset() {
    MechInputTouchButton::Reset();
}
