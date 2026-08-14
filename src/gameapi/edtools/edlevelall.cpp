#include "gameapi_edtools_types.h"

void EditorSettings::AddMenuItems(eduimenu_s *) {
}

EditorSettings::EditorSettings() {
}

void EditorSettings::Serialise(EdStream &) {
}

void EdClass::AddType(EdRef *) {
}

void EdClass::CopyObject(void *, void *) {
}

void EdClass::FindMember(EdMember *, void *, i32, i32) {
}

void EdClass::FindObject(char *) {
}

void EdClass::FindTypeRef(char *, i32) {
}

void EdClass::FindTypeRef(i32, i32) {
}

void EdClass::GetStreamClasses(EdStream &, i32 *, i32 &, i32) {
}

void EdClass::Serialise(EdStream &, i32 *) {
}

void EdClass::SerialiseObject(EdStream &, void *) {
}

void EdClass::SerialiseObject(EdStream &, void *, EdClass *, EdRegistry *) {
}

void EdClass::SerialiseObjectHeader(EdStream &, void *) {
}
