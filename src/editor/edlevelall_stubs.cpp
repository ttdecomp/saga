#include "legoapi/legoapi_types.h"

void BaseEditor::Initialise(variptr_u &, variptr_u &, i32) {
}

void ClassEditor::RegisterTool(EdTool &) {
}

void ClassEditor::UpdateSnapRay(VuVec &) {
}

void LevelEditor::GetScene(i32) {
}

void LevelEditor::RegisterEditor(BaseEditor &) {
}

void LevelEditor::SetNextMenu(eduimenu_s *) {
}

void PropertyMenu::ContainsObject(void *) {
}

void PropertyTool::HasActiveMenu() {
}

void PropertyTool::SetMenuControl(eduimenu_s *, EdControl *) {
}

void ClassObjectList::IsInList(ClassObject) {
}

void ClassObjectList::IsInList(EdClass *) {
}

void cbEdLevelSetSliderInt(eduimenu_s *, eduiitem_s *, u32) {
}

void cbEdLevelSetSliderFloat(eduimenu_s *, eduiitem_s *, u32) {
}
