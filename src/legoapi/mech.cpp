#include "legoapi/gameobject.h"

MechObjectInterface::~MechObjectInterface() {}

CharacterObjectInterface::CharacterObjectInterface(GameObject_s &game_object) : object(&game_object) {
    objects = NULL;
    game_object.mech_object_interface = this;
}

MechAddon::~MechAddon() {}

void MechAddon::OnAdded() {}

void MechAddon::OnRemoved() {}

bool MechAddon::OnProcess(ProcessStage, f32) {
    return true;
}

void MechAddon::OnRender(RenderStage) {}

MechEdgeStopAddon::MechEdgeStopAddon(MechObjectInterface &mech_interface) {
    mech_object_interface = &mech_interface;
    next_mech_object = NULL;
    previous_mech_object = NULL;
    next_addon = NULL;
}

MechAutofireAddon::MechAutofireAddon(MechObjectInterface &mech_interface) {
    mech_object_interface = &mech_interface;
    next_mech_object = NULL;
    previous_mech_object = NULL;
    next_addon = NULL;
}

MechTouchTask::~MechTouchTask() {}
