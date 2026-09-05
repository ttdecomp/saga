#include "MechInputTouch_types.h"

#include <stddef.h>
#include <new>

u8 MechSystems::SkipTextScroll = 0;

// vtable for MechSystems @0x66b320 slot 0x08: returns the static name string
// @0x573c42 (from MechSystems::GetName @0x45def0).
char const *MechSystems::GetName() {
    return "MechSystems";
}

void MechSystems::Display(ThingRenderData *) {
    TouchUI().Render();
}

void MechSystems::EnterLevel(WORLDINFO_s *) {
    if (initialized == 0) {
        Init();
    }
    if (initialized != 0) {
        PlayerButton().SetupTargetIds();
    }
}

void MechSystems::ExitLevel(WORLDINFO_s *) {
}

void MechSystems::FindMoveToMarkerAtPos(VuVec const &, bool) {
}

i32 MechInputTouchMenuController::AnyTouchesThisFrame = 0;
i32 MechInputTouchMenuController::PackButtonPressed = 0;
i32 MechInputTouchMenuController::PackButtonID = 0;

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
    if (initialized == 0) {
        input_touch_system.Init();
        TouchUI().Init();
        TouchUI().AddUIElement(PlayerButton());
        TouchUI().AddUIElement(PauseButton());
        initialized = 1;
    }
}

void MechSystems::LoadPerm() {
}

MechSystems::MechSystems() {
    new (ui_storage) MechTouchUI();
    new (player_button_storage) MechTouchUIPlayerButton();
    new (pause_button_storage) MechTouchUIPauseButton();
    field_0x4 = 0;
    flags = 0;
    profiling_0xc = NULL;
    for (i32 i = 0; i < 6; ++i) {
        unknown_0x10[i] = 0;
    }
    marker_manager_vptr = 0;
    for (i32 i = 0; i < 0x80; ++i) {
        marker_storage[i] = 0;
    }
    for (i32 i = 0; i < 11; ++i) {
        renderers_and_buttons[i] = NULL;
    }
    initialized = 0;
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
    if (initialized == 0) {
        Init();
    }
}

void MechSystems::ProcessEvenWhenPaused(ThingProcessData *data) {
    if (initialized == 0) {
        Init();
    }
    if (initialized != 0) {
        input_touch_system.ProcessEvenWhenPaused(data);
        TouchUI().Process(data->t);
    }
}

void MechSystems::ProcessOnlyWhenPaused(ThingProcessData *) {
    if (initialized == 0) {
        Init();
    }
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
    PauseButton().~MechTouchUIPauseButton();
    PlayerButton().~MechTouchUIPlayerButton();
    TouchUI().~MechTouchUI();
}
