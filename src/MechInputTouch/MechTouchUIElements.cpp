#include "MechInputTouch_types.h"

#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/render/core/render.h"
#include "legoapi/world/level.h"

extern i32 CutSceneWaiting;
extern i32 editor_active;
extern i32 NewMode;
extern i32 PANELOFF;
extern i32 Paused;

i32 GetMenuID();
float GetAspectRatio();
void PlayerButton_OnClick_Callback_NextButton(MechTouchUIElement &, TouchHolder &);
void PlayerButton_OnHold_Callback(MechTouchUIElement &, TouchHolder &);
void PlayerButton_OnLeave_Callback(MechTouchUIElement &, TouchHolder &);
void MechTouchUIPauseButton_OnClick_Callback(MechTouchUIElement &, TouchHolder &);

bool MechInputTouchGestureTracker::OnDown(GameObject_s &, TouchHolder &) {
    return false;
}

bool MechInputTouchGestureTracker::OnRelease(GameObject_s &, TouchHolder &) {
    return false;
}

bool MechInputTouchGestureTracker::OnClick(GameObject_s &, TouchHolder &) {
    return false;
}

bool MechInputTouchGestureTracker::OnDoubleClick(GameObject_s &, TouchHolder &) {
    return false;
}

bool MechInputTouchGestureTracker::OnHold(GameObject_s &, TouchHolder &) {
    return false;
}

bool MechInputTouchGestureTracker::OnSwipe(GameObject_s &, TouchHolder &, i32) {
    return false;
}

MechTouchUIElement::~MechTouchUIElement() {
}

void MechTouchUIElement::Process(float) {
}

void MechTouchUIElement::Render() {
}

bool MechTouchUI::AddUIElement(MechTouchUIElement &element) {
    for (i32 i = 0; i < 32; ++i) {
        if (elements[i] == NULL) {
            elements[i] = &element;
            return true;
        }
    }
    return false;
}

void MechTouchUI::Init() {
    MechSystems *systems = MechSystems::Get();
    MechInputTouchGestureTrackingSystem *tracking =
        reinterpret_cast<MechInputTouchGestureTrackingSystem *>(reinterpret_cast<u8 *>(systems) + 0x84);
    tracking->RegisterGestureTracker(*this, 100);
}

MechTouchUI::MechTouchUI() {
    for (i32 i = 0; i < 32; ++i) {
        elements[i] = NULL;
    }
}

bool MechTouchUI::OnClick(GameObject_s &, TouchHolder &holder) {
    for (i32 i = 0; i < 32; ++i) {
        MechTouchUIElement *element = elements[i];
        if (element != NULL && element->owner == &holder) {
            if (element->hovered != 0 && element->on_click != NULL && element->disabled == 0) {
                element->on_click(*element, holder);
            }
            return true;
        }
    }
    return false;
}

bool MechTouchUI::OnDoubleClick(GameObject_s &, TouchHolder &holder) {
    for (i32 i = 0; i < 32; ++i) {
        MechTouchUIElement *element = elements[i];
        if (element != NULL && element->owner == &holder) {
            return true;
        }
    }
    return false;
}

bool MechTouchUI::OnDown(GameObject_s &, TouchHolder &holder) {
    MechTouchUIElement *element = PickElement(holder.touch_position);
    if (element == NULL) {
        return false;
    }
    element->owner = &holder;
    if (element->on_down != NULL && element->disabled == 0) {
        element->on_down(*element, holder);
    }
    return true;
}

bool MechTouchUI::OnHold(GameObject_s &, TouchHolder &holder) {
    for (i32 i = 0; i < 32; ++i) {
        MechTouchUIElement *element = elements[i];
        if (element != NULL && element->owner == &holder) {
            if (PickElement(holder.touch_position) == element && element->on_hold != NULL && element->disabled == 0) {
                element->on_hold(*element, holder);
            } else {
                holder.field_0x0[7] = 1;
            }
            return true;
        }
    }
    return false;
}

bool MechTouchUI::OnRelease(GameObject_s &, TouchHolder &holder) {
    for (i32 i = 0; i < 32; ++i) {
        MechTouchUIElement *element = elements[i];
        if (element != NULL && element->owner == &holder) {
            if (element->on_release != NULL && element->disabled == 0) {
                element->on_release(*element, holder);
            }
            element->owner = NULL;
        }
    }
    return false;
}

MechTouchUIElement *MechTouchUI::PickElement(NuVec2 &point) {
    MechTouchUIElement *picked = NULL;
    float picked_depth = -1000000000.0f;
    for (i32 i = 0; i < 32; ++i) {
        MechTouchUIElement *element = elements[i];
        if (element == NULL || (element->visible == 0 && CUTSTOPGAME == 0) || element->disabled != 0) {
            continue;
        }

        if (element->rectangular == 0) {
            const float x = point.x - element->position.x;
            const float y = point.y - element->position.y;
            if (1.0f <
                x * x / (element->radius_x * element->radius_x) + y * y / (element->radius_y * element->radius_y)) {
                continue;
            }
        } else {
            const float half_x = element->radius_x * 0.5f;
            const float half_y = element->radius_y * 0.5f;
            if (point.x < element->position.x - half_x || point.x > element->position.x + half_x ||
                point.y < element->position.y - half_y || point.y > element->position.y + half_y) {
                continue;
            }
        }
        if (element->position.z > picked_depth) {
            picked = element;
            picked_depth = element->position.z;
        }
    }
    return picked;
}

void MechTouchUI::Process(float dt) {
    for (i32 i = 0; i < 32; ++i) {
        MechTouchUIElement *element = elements[i];
        if (element == NULL) {
            continue;
        }
        element->Process(dt);
        const bool was_hovered = element->hovered != 0;
        if (element->owner != NULL && PickElement(element->owner->touch_position) == element) {
            element->hovered = 1;
        } else {
            element->hovered = 0;
            if (was_hovered && element->on_leave != NULL && element->owner != NULL) {
                element->on_leave(*element, *element->owner);
            }
        }
    }
}

bool MechTouchUI::RemoveUIElement(MechTouchUIElement &element) {
    for (i32 i = 0; i < 32; ++i) {
        if (elements[i] == &element) {
            element.hovered = 0;
            element.owner = NULL;
            elements[i] = NULL;
            return true;
        }
    }
    return false;
}

void MechTouchUI::Render() {
    if (CUTSTOPGAME != 0 && GetMenuID() == -1) {
        return;
    }
    for (i32 i = 0; i < 32; ++i) {
        MechTouchUIElement *element = elements[i];
        if (element != NULL && element->visible != 0) {
            element->Render();
        }
    }
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
    position = VuVec(0.7725f, 0.7525f, 0.0f, 1.0f);
    radius_x = GetAspectRatio() * 0.16f;
    radius_y = 0.16f;
    on_click = MechTouchUIPauseButton_OnClick_Callback;
    disable_timer = 0.0f;
    skip_prompt_timer = 0.0f;
}

void MechTouchUIPauseButton::Process(float dt) {
    const float next_disable_timer = disable_timer - dt;
    visible = 0;
    if (next_disable_timer < -10.0f) {
        disable_timer = -1.0f;
        disabled = 0;
    } else {
        disable_timer = next_disable_timer;
        disabled = next_disable_timer > 0.0f;
    }

    if (Paused == 0) {
        skip_prompt_timer -= dt;
    }
    if (MechInputTouchMenuController::AnyTouchesThisFrame > 0) {
        skip_prompt_timer = 3.0f;
    }

    if (NewMode != 0 || NewLData != NULL || editor_active != 0 || GameTimer.time_elapsed <= 0.0f ||
        GameTimer.update_count == 0 || WORLD == NULL || WORLD->current_level == TITLES_LDATA || CutSceneWaiting != 0) {
        return;
    }

    if (CUTSTOPGAME != 0) {
        if (CutScene_IsSkippable(static_cast<CUTINFO *>(CutStopInfo)) == 0 || skip_prompt_timer <= 0.0f) {
            return;
        }
    }

    if (MiniCutCam != 0 || (PANELOFF != 0 && GetMenuID() == -1) || WORLD->current_level == CREDITS_LDATA ||
        (WORLD->current_level->flags & LEVEL_STATUS) != 0 || memcard_autosavestarted != 0 ||
        memcard_autosavepostdelay > 0.0f || memcard_autosavepredelay > 0.0f) {
        return;
    }

    visible = 1;
}

void MechTouchUIPauseButton::Render() {
    if (NewMode != 0 || NewLData != NULL || editor_active != 0 || GameTimer.time_elapsed <= 0.0f ||
        GameTimer.update_count == 0 || WORLD == NULL || WORLD->current_level == TITLES_LDATA ||
        WORLD->current_level == CREDITS_LDATA) {
        return;
    }

    if (Paused != 0) {
        const i32 menu_id = GetMenuID();
        DrawTouchPrompt(
            const_cast<char *>(menu_id == LEGO_MENU_PAUSE_MAIN || menu_id == LEGO_MENU_PAUSE_CUTSCENE ? ">" : "<<"),
            NULL, hovered != 0, true);
        return;
    }

    if (GameMenu[GameMenuLevel].menu != -1 || CutSceneWaiting != 0 || MiniCutCam != 0 || memcard_autosavestarted != 0 ||
        memcard_autosavepostdelay > 0.0f || memcard_autosavepredelay > 0.0f ||
        (CUTSTOPGAME != 0 && CutScene_IsSkippable(static_cast<CUTINFO *>(CutStopInfo)) == 0)) {
        if (GetMenuID() != -1) {
            DrawTouchPrompt(const_cast<char *>("<<"), NULL, hovered != 0, true);
        }
        return;
    }

    DrawTouchPrompt(const_cast<char *>("II"), NULL, hovered != 0, false);
}

MechTouchUIPlayerButton::MechTouchUIPlayerButton() {
    position = VuVec(-0.7725f, 0.7525f, 0.0f, 1.0f);
    radius_x = GetAspectRatio() * 0.16f;
    radius_y = 0.16f;
    on_click = PlayerButton_OnClick_Callback_NextButton;
    on_hold = PlayerButton_OnHold_Callback;
    on_leave = PlayerButton_OnLeave_Callback;
    field_0x3c[0] = field_0x3c[1] = field_0x3c[2] = field_0x3c[3] = 0;
    field_0x3c[4] = 1;
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
