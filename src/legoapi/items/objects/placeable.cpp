#include "legoapi/legoapi_types.h"

void PlaceableHelper::Find(char *) {
}

void PlaceableHelper::Find(char *, Placeable **, i32) {
}

void PlaceableHelper::FindObject(char *) {
}

void PlaceableHelper::GetNextObject(void *) {
}

void PlaceableHelper::GetNextObject(void *, i32 (*)(void *)) {
}

void PlaceableHelper::Initialise() {
}

void PlaceableHelper::IsEditorObject(ClassObject &) {
}

PlaceableHelper::PlaceableHelper() {
}

void PlaceableHelper::RegisterObjectType(char *, PlaceableInterface *) {
}

void PlaceableInterface::DebugOutputObjects() {
}

void PlaceableInterface::Find(char *) {
}

void PlaceableInterface::Find(char *, Placeable **, i32) {
}

void PlaceableNameControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

PlaceableNameControl::PlaceableNameControl() {
}

void PlaceableNameControl::Process(EdInputContext &) {
}

void PlaceableNameControl::Render() {
}

void PlaceableNameControl::cbButton(eduimenu_s *, eduiitem_s *, u32) {
}

void PlaceableNameControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void PlaceableNameControl::cbSelectObject(eduimenu_s *, eduiitem_s *, u32) {
}

void Placeable::GetCurrentPosition() const {
}

__attribute__((weak)) void Placeable::Reset() {
}

void Placeable::GetInitialPosition() const {
}

void Placeable::SetCurrentPosition(VuVec const *) {
}

void Placeable::SetInitialPosition(VuVec const *) {
}
