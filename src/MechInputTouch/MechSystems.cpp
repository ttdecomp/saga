#include "MechInputTouch_types.h"

#include <stddef.h>

// vtable for MechSystems @0x66b320 slot 0x08: returns the static name string
// @0x573c42 (from MechSystems::GetName @0x45def0).
char const *MechSystems::GetName() {
    return "MechSystems";
}

void MechSystems::Display(ThingRenderData *) {
}

void MechSystems::EnterLevel(WORLDINFO_s *) {
}

void MechSystems::ExitLevel(WORLDINFO_s *) {
}

void MechSystems::FindMoveToMarkerAtPos(VuVec const &, bool) {
}

int MechInputTouchMenuController::PackButtonPressed = 0;
int MechInputTouchMenuController::PackButtonID = 0;

MechSystems *MechSystems::Get() {
    static MechSystems *instance = NULL;
    if (instance == NULL) {
        instance = new MechSystems();
    }
    return instance;
}

void MechSystems::HookUpClickToPressStart() {
}

void MechSystems::Init() {
}

void MechSystems::LoadPerm() {
}

MechSystems::MechSystems() {
}

void MechSystems::NewMoveToMarker(MechObjectInterface &) {
}

void MechSystems::NewRadarPulse(VuVec const &, bool) {
}

void MechSystems::NewSwipeMarker(TouchHolder &, i32, SwipeDecalRenderer::Style) {
}

void MechSystems::NewTagButton(GameObject_s &, TouchHolder &) {
}

void MechSystems::Process(ThingProcessData *) {
}

void MechSystems::ProcessEvenWhenPaused(ThingProcessData *) {
}

void MechSystems::ProcessOnlyWhenPaused(ThingProcessData *) {
}

void MechSystems::Render(ThingRenderData *) {
}

void MechSystems::RenderCurrentPlayerHighlight() {
}

void MechSystems::Reset(ThingResetData *) {
}

void MechSystems::UnhookClickToPressStart() {
}

MechSystems::~MechSystems() {
}
