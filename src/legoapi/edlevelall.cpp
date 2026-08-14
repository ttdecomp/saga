#include "legoapi_types.h"

void BaseEditor::Initialise(variptr_u &, variptr_u &, i32) {
}

void BaseEditor::ReadBuffer(void **, void *, i32) {
}

void BaseEditor::WriteBeginBlock(i32, i32) {
}

void BaseEditor::WriteEndBlock(i32) {
}

void BaseEditor::WriteMetaData(i32, i32, i32, i32) {
}

void CursorTool::Initialise(variptr_u &, variptr_u &, i32) {
}

void CursorTool::Process(EdInputContext &) {
}

void CursorTool::Render() {
}

ClassEditor::ClassEditor() {
}

void ClassEditor::AddMenuItems(eduimenu_s *) {
}

void ClassEditor::ClearLevel(i32) {
}

void ClassEditor::CreateObject() {
}

void ClassEditor::CreateObject(ClassObject &) {
}

void ClassEditor::CreateObject(EdClass *) {
}

void ClassEditor::CreateObject(i32) {
}

void ClassEditor::Enter() {
}

void ClassEditor::Exit() {
}

void ClassEditor::Flush() {
}

void ClassEditor::Initialise(variptr_u &, variptr_u &, i32) {
}

void ClassEditor::Process(EdInputContext &) {
}

void ClassEditor::ReadBlock(DATAPTR *) {
}

void ClassEditor::Render() {
}

void ClassEditor::Serialise(EdStream &) {
}

void ClassEditor::WriteBlock(i32) {
}

void ClassEditor::DestroySelectedObjects() {
}

void ClassEditor::DestroySelectedObjectsNow() {
}

void ClassEditor::DrawObjectSphere(ClassObject &, i32) {
}

void ClassEditor::Editable(void *, EdClass *, i32) {
}

void ClassEditor::FindNearestObject(VuVec &, ClassObject &, ClassObject &, i32) {
}

void ClassEditor::FindNearestObject(VuVec &, ClassObject &, i32) {
}

void ClassEditor::FindNearestObject(VuVec &, VuVec &, ClassObject &, ClassObject &, i32) {
}

void ClassEditor::FindNearestObject(VuVec &, VuVec &, ClassObject &, i32) {
}

void ClassEditor::FocusSelected() {
}

void ClassEditor::InitialiseObject(ClassObject &) {
}

void ClassEditor::IsSelectedClass(EdClass *) {
}

void ClassEditor::IsSelectedObject(ClassObject &) {
}

void ClassEditor::IsSelectedObject(void *, EdRef *) {
}

void ClassEditor::IsUniqueName(char *) {
}

void ClassEditor::MakeUniqueName(char const *, char *, i32) {
}

void ClassEditor::PostLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
}

void ClassEditor::PostSaveInitialisation() {
}

void ClassEditor::PreLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
}

void ClassEditor::PreSaveInitialisation() {
}

void ClassEditor::RegisterTool(EdTool &) {
}

void ClassEditor::SelectLED(i32) {
}

void ClassEditor::SelectObject(ClassObject &, i32) {
}

void ClassEditor::SetMode(i32) {
}

void ClassEditor::SetViewMenuHilight(eduimenu_s *) {
}

void ClassEditor::SnapPoint(VuVec &) {
}

void ClassEditor::UpdateClassFilter(EdInputContext &) {
}

void ClassEditor::UpdateLists(MemoryBuffer *, MemoryBuffer *) {
}

void ClassEditor::UpdateSelectedObjects(EdInputContext &) {
}

void ClassEditor::UpdateSnapRay(VuVec &) {
}

void ClassEditor::ViewSelected() {
}

void ClassEditor::cbDestroyMenu(eduimenu_s *, eduimenu_s *) {
}

void ClassEditor::cbDestroyObject(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassDeleteObject(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassExportMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassFileMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassImportMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassModeMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassNewMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassNewObject(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassRemoveDuplicates(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassSelectClassMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassSelectObject(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassSelectObjectMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassSetMode(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassSetPinned(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassSetSnap(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassSetView(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassSnapMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassToolsMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdClassViewMenu(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdCopySelectedObject(EdInputContext &) {
}

void ClassEditor::cbEdCreateClassNewObject(i32) {
}

void ClassEditor::cbEdFilterLED(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdLevelDeselectAll(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdLevelSelectAll(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbEdPadSetManipulatorMode(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassEditor::cbFileSelected(eduimenu_s *, eduiitem_s *, u32) {
}

void ClassObject::GetName(char *, i32) {
}

void ClassObject::Set(char *) {
}

void LevelEditor::AddInfoText(char *) {
}

void LevelEditor::AddScene(char *, nugscn_s *, i32) {
}

void LevelEditor::AddText(char *) {
}

void LevelEditor::BeginMultiLoad(variptr_u *, variptr_u *) {
}

void LevelEditor::ClearLevel(i32) {
}

void LevelEditor::CloseMenu() {
}

void LevelEditor::CreateEditorList(eduimenu_s *, eduiitem_s *) {
}

void LevelEditor::CreateMenu() {
}

void LevelEditor::Display(ThingRenderData *) {
}

void LevelEditor::DrawInfoText(char **, i32, i32, i32, i32, i32, i32, i32) {
}

void LevelEditor::EndMultiLoad(variptr_u *, variptr_u *) {
}

void LevelEditor::Enter() {
}

void LevelEditor::Exit() {
}

void LevelEditor::FindSceneId(char *) {
}

void LevelEditor::Flush() {
}

void LevelEditor::GetEdScene(i32) {
}

void LevelEditor::GetScene(char *) {
}

void LevelEditor::GetScene(i32) {
}

void LevelEditor::Initalise(variptr_u &, variptr_u &, i32) {
}

void LevelEditor::IsActiveScene(nugscn_s *) {
}

void LevelEditor::IsEditable(i32) {
}

LevelEditor::LevelEditor() {
}

void LevelEditor::Load(char *, variptr_u *, variptr_u *, i32) {
}

void LevelEditor::LoadState(variptr_u *, variptr_u *, variptr_u *, variptr_u *, variptr_u *, variptr_u *) {
}

void LevelEditor::ProcessEvenWhenPaused(ThingProcessData *) {
}

void LevelEditor::ReadStream(EdFileInputStream &) {
}

void LevelEditor::RegisterEditor(BaseEditor &) {
}

void LevelEditor::Reset() {
}

void LevelEditor::Save() {
}

void LevelEditor::SaveState(i32, variptr_u *, variptr_u *) {
}

void LevelEditor::SaveState(variptr_u *, variptr_u *) {
}

void LevelEditor::SetNextMenu(eduimenu_s *) {
}

void LevelEditor::SetPadText(i32, char *) {
}

void LevelEditor::SetSaveFilename(char *) {
}

void LevelEditor::WriteStream(EdFileOutputStream &) {
}

void PropertyMenu::AddObject(ClassObject &) {
}

void PropertyMenu::ClearObjecs() {
}

void PropertyMenu::ContainsObject(ClassObject &) {
}

void PropertyMenu::ContainsObject(void *) {
}

void PropertyMenu::Destroy() {
}

void PropertyMenu::SelectAttr(i32) {
}

void PropertyTool::AddPropertyMenuItems(eduimenu_s *, EdClass *, void *, eduiitem_s *) {
}

void PropertyTool::AutoLocateMenu(PropertyMenu *) {
}

void PropertyTool::BringToFront(PropertyMenu *) {
}

void PropertyTool::CreatePropertyMenu(ClassObject &) {
}

void PropertyTool::FindItemMenu(PropertyMenu *, ClassItem *) {
}

void PropertyTool::GetActiveMenu(PropertyMenu *) {
}

void PropertyTool::GetClassName(EdRef *, char *) {
}

void PropertyTool::GetNextActiveMenu() {
}

void PropertyTool::GetNextDefaultActiveMenu(eduimenu_s *) {
}

void PropertyTool::GetTypeName(EdRef *, char *) {
}

void PropertyTool::HasActiveMenu() {
}

void PropertyTool::Initialise(variptr_u &, variptr_u &, i32) {
}

void PropertyTool::Process(EdInputContext &) {
}

void PropertyTool::ProcessControls(EdInputContext &) {
}

void PropertyTool::ProcessMenu(EdInputContext &) {
}

PropertyTool::PropertyTool() {
}

void PropertyTool::RefreshMenuControls(PropertyMenu *) {
}

void PropertyTool::Render() {
}

void PropertyTool::RenderMenu(PropertyMenu *) {
}

void PropertyTool::RetrievePropertyMenu(ClassObject *, PropertyMenuList *) {
}

void PropertyTool::SelectAttr(i32) {
}

void PropertyTool::SetDefaultActiveMenu(PropertyMenu *) {
}

void PropertyTool::SetMenuControl(eduimenu_s *, EdControl *) {
}

void PropertyTool::ToggleActiveMenu() {
}

void PropertyTool::ediGetMenuStartMetrics() {
}

void PropertyTool::ediMenuRetrieveMetrics(eduimenu_s *) {
}

void PropertyTool::ediMenuStoreMetrics(eduimenu_s *) {
}

void ClassObjectList::GetAveragePosition(VuVec &) {
}

void ClassObjectList::GetAveragePosition(VuVec &, float &) {
}

void ClassObjectList::IsInList(ClassObject) {
}

void ClassObjectList::IsInList(EdClass *) {
}

void ClassObjectList::IsInList(void *, EdRef *) {
}
