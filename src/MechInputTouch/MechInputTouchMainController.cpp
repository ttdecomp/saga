#include "MechInputTouch_types.h"

#include <string.h>

MechInputTouchMainController::MechInputTouchMainController(i32) {
}

void MechInputTouchMainController::RemoveUnpressedButtons(NuInputTouchData &, NuInputTouchData const &) {
}

void MechInputTouchMainController::Render() {
}

void MechInputTouchMainController::ResetButtons() {
    buttons_repeat = 0;
    buttons_pressed = 0;
    buttons_were_pressed = 0;
    memset(buttons_repeat_timers, 0, sizeof(buttons_repeat_timers));
}

void MechInputTouchMainController::Update(NuInputTouchData const *) {
}

void MechInputTouchMainController::UpdateButtons() {
}

MechInputTouchMainController::~MechInputTouchMainController() {
}
