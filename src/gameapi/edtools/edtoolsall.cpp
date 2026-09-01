#include "gameapi_edtools_types.h"
#include "gameapi/edtools/edcam.h"

void EdTerrInit(void *, void *) {
}

void edDrawLine(nuvec_s *, nuvec_s *, unsigned char, unsigned char, unsigned char) {
}

void EdDrawBegin(i32) {
}

void edpartPlace(i32, nuvec_s *) {
}

void edppDoInput(nupad_s *) {
}

void EdTerrShadow(nuvec_s *, float, float, i32) {
}

void edbriDoInput(nupad_s *) {
}

void edgraDoInput(nupad_s *) {
}

void edpartCreate(nuvec_s *, i32) {
}

void edppPtlPlace(i32, nuvec_s *) {
}

void EdDrawPolyTri(VuVec const &, VuVec const &, VuVec const &, i32) {
}

void edanimDoInput(nupad_s *) {
}

void edbobsDrawBox(nuvec_s *, nuvec_s *, i32) {
}

void edbriFileSave(char *) {
}

void edgraFileSave(char *) {
}

void edpartDestroy(i32) {
}

void edpartDoInput(nupad_s *) {
}

void edppPtlCreate(nuvec_s *, i32) {
}

void edppPtlShelve(i32) {
}

void EdDrawLineCube(VuMtx const &, float, i32) {
}

void EdDrawPolyAxis(VuMtx const &, float, i32) {
}

void edanimFileSave(char *) {
}

void edpartInitType(i32) {
}

void edppDrawCursor() {
}

void edppPtlDestroy(i32) {
}

void EdDrawLineArrow(VuMtx const &, float, i32) {
}

void EdDrawLineCross(VuVec const &, float, i32) {
}

void EdDrawPolyArrow(VuVec const &, VuVec const &, i32, i32, float, float, float, float) {
}

void edbriDrawCursor() {
}

void edgraClumpPlace(i32, nuvec_s *) {
}

void edgraDrawCursor() {
}

void edpartPtlShelve(i32) {
}

void edpartScaleType(i32, float) {
}

void edppSaveEffects(char *, char) {
}

void edrtlAddBurnout(nuvec_s *) {
}

void EdDrawLineSphere(VuVec const &, float, float, i32) {
}

void EdDrawPolySector(VuVec const &, float, i32, i32, i32, i32, i32) {
}

void edGetPadDisabled() {
}

void edSetPadDisabled(i32) {
}

void edanimDrawCursor() {
}

void edbriBridgePlace(i32, nuvec_s *) {
}

void edgraClumpCreate(nuvec_s *) {
}

void edgraClumpReseed(i32) {
}

void edpartDrawCursor() {
}

void edrtlBurnoutSave(char *, burnset_s *) {
}

void edrtlInitBurnset(burnset_s *) {
}

void EdDrawLineCircleX(VuVec const &, float, i32, i32) {
}

void EdDrawLineCircleY(VuVec const &, float, i32, i32) {
}

void EdDrawLineCircleZ(VuVec const &, float, i32, i32) {
}

void EdDrawLineSegment(VuVec const &, VuVec const &, i32) {
}

void edanimParamCreate(i32) {
}

void edbriBridgeCreate(nuvec_s *) {
}

void edbriBridgeUpdate(i32, nugscn_s *) {
}

void edcamSetContoller(i32 invert_pitch) {
    edcam_s *camera = edcamGetEdCam();
    camera->freedoms &= ~EDCAM_FREEDOM_INVERT_PAD_PITCH;
    if (invert_pitch != 0) {
        camera->freedoms |= EDCAM_FREEDOM_INVERT_PAD_PITCH;
    }
}

void edgraClumpDestroy(i32) {
}

void edpartSaveEffects(char *, char) {
}

void edppPtlChangeType(i32, i32) {
}

void edppPtlCreateCopy(nuvec_s *, i32) {
}

void edrtlPlaceBurnout(i32, nuvec_s *) {
}

void edrtlResetBurnset(burnset_s *) {
}

void eduiSetPinnedMenu(eduimenu_s *) {
}

void EdDrawPolyCylinder(VuMtx const &, float, float, float, i32, i32, i32, i32) {
}

void EdDrawPolyCylinder(VuVec const &, VuVec const &, i32, i32, i32, float, float, float) {
}

void edanimParamDestroy(i32) {
}

void edbitsDoSingleDump(i32) {
}

void edbriBridgeDestroy(i32) {
}

void edgraCalculatePage(char, i32) {
}

void edgraInstancePlace(i32, nuvec_s *) {
}

void edpartLookupObject(char *) {
}

void edrtlRemoveBurnout(i32) {
}

void edSpline_FindAllBeg(nugscn_s *, char *, nugspline_s **, i32) {
}

void edSpline_FindAllSub(nugscn_s *, char *, nugspline_s **, i32) {
}

void edSpline_SplineFind(nugscn_s *, char *) {
}

void edSpline_SplineList(nugscn_s *) {
}

void edanimParticlePlace(i32, nuvec_s *) {
}

void edanimStartAllPages() {
}

void edgraInstanceCreate(nuvec_s *) {
}

void edpartPtlChangeType(i32, i32) {
}

void edppDestroyAllPages() {
}

void edrtlBurnoutLoadSet(char *, burnset_s *) {
}

void edanimParticleCreate(nuvec_s *) {
}

void edgraInstanceDestroy(i32) {
}

void edpartLoadSingleType(part_typedesc_s *, i32, i32) {
}

void edppDetermineNearest(float) {
}

void edppHighlightNearest() {
}

void edppMultipleCopyCopy() {
}

void edbriDetermineNearest(float) {
}

void edgraSortVectorBuffer(i32) {
}

void edpartDestroyAllPages() {
}

void edppMultipleCopyClear() {
}

void edppMultipleCopyPaste() {
}

void edppStartSingleEffect(i32) {
}

void edpartDetermineNearest(float) {
}

void edpartHighlightNearest() {
}

void edpartMultipleCopyCopy() {
}

void edpartMultipleCopyClear() {
}

void edanimPlayerAnimDistance(i32) {
}

void edpartLookupDebrisEffect(char *) {
}

void edanimRenderSoundEmitters(i32) {
}

void edbobs_DrawCoordinateInfo(nuvec_s *, i32, i32) {
}

void edpartLookupObjectInScene(char *, nugscn_s *) {
}

void edrtlDetermineNearestBurn(float, burnset_s *) {
}

void edanimDetermineNearestAnim(float) {
}

void edgraDetermineNearestClump(float) {
}

void eduiItemFileSelectorCreate(u32, eduiiattr_s *, void (*)(eduimenu_s *, eduiitem_s *, u32), char *) {
}

void edanimDetermineNearestSound(float) {
}

void edanimRenderParticleEmitters(i32) {
}

void edgraDetermineNearestInstance(float) {
}

void edanimDetermineNearestParticle(float) {
}

void EdDrawEnd() {
}

void EdDrawMtx(VuMtx const *) {
}

void EdTerrRay(VuVec &, VuVec &) {
}

EdManScale::EdManScale() {
}

void EdManScale::Process(EdInputContext &, ClassObjectList &) {
}

void EdManScale::Render(ClassObjectList &) {
}

void EdRegistry::AddMapping(char *, char *) {
}

void EdRegistry::AddObjectNotifier(EdObjectNotifier *) {
}

void EdRegistry::ClassIFaceProcess(EdClass *, void *, EdInputContext &) {
}

void EdRegistry::ClassIFaceProcess(i32, void *, EdInputContext &) {
}

void EdRegistry::ClassIFaceRender(EdClass *, void *, i32) {
}

void EdRegistry::ClassIFaceRender(i32, void *, i32) {
}

void EdRegistry::CreateObject(EdClassInterface *, void *, i32, i32, i32) {
}

void EdRegistry::DefunctObject(EdClassInterface *, void *, i32, i32) {
}

void EdRegistry::DestroyObject(EdClassInterface *, void *, i32, i32) {
}

void EdRegistry::Flush() {
}

void EdRegistry::GetClass(char *) {
}

void EdRegistry::GetClass(i32) {
}

void EdRegistry::GetClassId(EdClass *) {
}

void EdRegistry::GetClassId(char *) {
}

void EdRegistry::GetStreamClassMapping(EdStream &, i32 *, i32 &, i32) {
}

void EdRegistry::GetType(char *) {
}

void EdRegistry::GetType(i32) {
}

void EdRegistry::GetTypeId(char *) {
}

void EdRegistry::Initialise(variptr_u &, variptr_u &, i32, i32, i32, i32) {
}

void EdRegistry::MapName(char *) {
}

void EdRegistry::NotifyCreateObject(void *, EdClass *, void *, i32, i32, i32) {
}

void EdRegistry::NotifyDefunctObject(void *, EdClass *, i32) {
}

void EdRegistry::NotifyDestroyObject(void *, EdClass *, i32, i32) {
}

void EdRegistry::NotifyReviveObject(void *, EdClass *, i32) {
}

void EdRegistry::RegisterBaseTypes() {
}

void EdRegistry::RegisterClass(char *, EdClassInterface *, i32) {
}

void EdRegistry::RegisterType(char *, i32, void (*)(EdStream &, void *, i32)) {
}

void EdRegistry::Serialise(EdStream &) {
}

void EdRegistry::SerialiseObjects(EdStream &, EdRegistry *) {
}

EdManRotate::EdManRotate() {
}

void EdManRotate::Process(EdInputContext &, ClassObjectList &) {
}

void EdManRotate::Render(ClassObjectList &) {
}

void EdManRotate::RotateItem(EdInputContext &, ClassObjectList &, i32, i32) {
}

void EdRefSpline::GetMemberData(void *, i32, void *, i32) {
}

void EdRefSpline::SetMemberData(void *, i32, void *, i32, i16 *) {
}

void EdBitControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

void EdBitControl::Refresh() {
}

void EdBitControl::cbButton(eduimenu_s *, eduiitem_s *, u32) {
}

void EdBitControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void EdBitControl::cbSelectItem(eduimenu_s *, eduiitem_s *, u32) {
}

void EdDefunctList::ReviveAll(i32) {
}

void EdEnumControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

void EdEnumControl::GetEnumString(i32) {
}

void EdEnumControl::GetEnumValue(char *) {
}

void EdEnumControl::Refresh() {
}

void EdEnumControl::cbButton(eduimenu_s *, eduiitem_s *, u32) {
}

void EdEnumControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void EdEnumControl::cbSelectItem(eduimenu_s *, eduiitem_s *, u32) {
}

void EdInputStream::SerialiseString(char **) {
}

void EdInputStream::SerialiseString(char **, i32) {
}

void EdInputStream::SerialiseString(char *, i32) {
}

void EdManipulator::DrawAxis(VuVec &, VuMtx *) {
}

void EdManipulator::DrawRotator(VuVec &) {
}

void EdManipulator::GetAxisLocators(VuVec &, VuVec *, VuMtx *) {
}

void EdManipulator::Process(EdInputContext &, ClassObjectList &) {
}

void EdManipulator::Render(ClassObjectList &) {
}

void EdManipulator::SelectAxis(EdInputContext &, VuVec &, VuVec &, VuVec &, VuMtx *) {
}

void EdManipulator::SelectRotator(EdInputContext &, VuVec &, VuVec &) {
}

void EdInputContext::Clear(i32) {
}

EdInputContext::EdInputContext() {
}

void EdInputContext::Get(i32) {
}

void EdInputContext::GetHold(i32) {
}

void EdInputContext::GetPress(i32) {
}

void EdInputContext::GetRelease(i32) {
}

void EdInputContext::GetRepeat(i32) {
}

void EdInputContext::Set(i32, float, float) {
}

void EdInputContext::Update(nucamera_s *, nupad_s *, float, bool) {
}

void EdOutputStream::SerialiseString(char **) {
}

void EdOutputStream::SerialiseString(char **, i32) {
}

void EdOutputStream::SerialiseString(char *, i32) {
}

void EdRefPlaceable::GetMemberData(void *, i32, void *, i32) {
}

void EdRefPlaceable::SetMemberData(void *, i32, void *, i32, i16 *) {
}

void EdColourControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

EdColourControl::EdColourControl() {
}

void EdColourControl::Refresh() {
}

void EdColourControl::cbButton(eduimenu_s *, eduiitem_s *, u32) {
}

void EdColourControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void EdColourControl::cbColourSelected(eduimenu_s *, eduiitem_s *, u32) {
}

void EdMatrixControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

void EdMatrixControl::Destroy() {
}

EdMatrixControl::EdMatrixControl() {
}

void EdMatrixControl::Refresh() {
}

void EdMatrixControl::SetMenuItemAttr(i32, eduiitem_s *, eduiiattr_s *, eduiiattr_s *) {
}

void EdMatrixControl::cbButton(eduimenu_s *, eduiitem_s *, u32) {
}

void EdMatrixControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void EdMatrixControl::cbSelected(eduimenu_s *, eduiitem_s *, u32) {
}

void EdStringControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

EdStringControl::EdStringControl() {
}

void EdStringControl::GetVal(char *, i32) {
}

void EdStringControl::Refresh() {
}

void EdStringControl::SetVal(char const *) {
}

void EdStringControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void EdStringControl::cbPress(eduimenu_s *, eduiitem_s *, u32) {
}

void EdVectorControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

void EdVectorControl::Destroy() {
}

EdVectorControl::EdVectorControl() {
}

void EdVectorControl::Refresh() {
}

void EdVectorControl::cbButton(eduimenu_s *, eduiitem_s *, u32) {
}

void EdVectorControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void EdVectorControl::cbSelected(eduimenu_s *, eduiitem_s *, u32) {
}

void EdClassInterface::DistanceToObject(VuVec &, VuVec &, void *, EdRef **) {
}

void EdClassInterface::DistanceToObject(VuVec &, void *, EdRef **) {
}

void EdClassInterface::GetNextObject(void *, i32 (*)(void *)) {
}

void EdSfxNameControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

EdSfxNameControl::EdSfxNameControl() {
}

void EdSfxNameControl::cbButton(eduimenu_s *, eduiitem_s *, u32) {
}

void EdSfxNameControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void EdSfxNameControl::cbSelectSfx(eduimenu_s *, eduiitem_s *, u32) {
}

void EdFileInputStream::BeginBlock(char const *) {
}

void EdFileInputStream::Eat(i32, i32) {
}

void EdFileInputStream::EndBlock() {
}

void EdFileInputStream::Open(i32, i32) {
}

void EdFileInputStream::SerialiseBuffer(void *, i32, i32) {
}

void EdFileOutputStream::BeginBlock(char const *) {
}

void EdFileOutputStream::Eat(i32, i32) {
}

void EdFileOutputStream::EndBlock() {
}

void EdFileOutputStream::Open(i32, i32) {
}

void EdFileOutputStream::SerialiseBuffer(void *, i32, i32) {
}

void EdRefSpecialObject::GetMemberData(void *, i32, void *, i32) {
}

void EdRefSpecialObject::SetMemberData(void *, i32, void *, i32, i16 *) {
}

EdSpecialObjectControl::EdSpecialObjectControl() {
}

void EdSpecialObjectControl::cbButton(eduimenu_s *, eduiitem_s *, u32) {
}

void EdSpecialObjectControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void EdSpecialObjectControl::cbSelectObject(eduimenu_s *, eduiitem_s *, u32) {
}

void EdSpecialObjectControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

void EdSpecialObjectControl::Process(EdInputContext &) {
}

void EdSpecialObjectControl::Render() {
}

void EdClassObjectNameControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

EdClassObjectNameControl::EdClassObjectNameControl() {
}

void EdClassObjectNameControl::Process(EdInputContext &) {
}

void EdClassObjectNameControl::Render() {
}

void EdClassObjectNameControl::cbButton(eduimenu_s *, eduiitem_s *, u32) {
}

void EdClassObjectNameControl::cbChanged(eduimenu_s *, eduiitem_s *, u32) {
}

void EdClassObjectNameControl::cbSelectClass(eduimenu_s *, eduiitem_s *, u32) {
}

void EdClassObjectNameControl::cbSelectObject(eduimenu_s *, eduiitem_s *, u32) {
}

void EdRef::CheckType(i32) {
}

EdRef::EdRef(char *, char *, i32, i32, i32, EdControl *, i32) {
}

void EdRef::GetAttributeData(void *, i32, i32, void *, i32) {
}

void EdRef::GetMemberData(void *, i32, void *, i32) {
}

void EdRef::GetMemberObject(void *) {
}

void EdRef::GetTypeSize(i32, i32) {
}

void EdRef::Serialise(EdStream &, i32 *) {
}

void EdRef::SetAttributeData(void *, i32, i32, void *, i32) {
}

void EdRef::SetMemberData(void *, i32, void *, i32, i16 *) {
}

void EdType::Serialise(EdStream &) {
}

EdStream::EdStream() {
}

EdStream::EdStream(MemoryBuffer *) {
}

EdStream::EdStream(MemoryBuffer *, MemoryBuffer *) {
}

void EdString::Set(char const *) {
}

EdString::~EdString() {
}

void EdSystem::Initalise(variptr_u &, variptr_u &, i32) {
}

void EdSystem::Process(float) {
}

void EdSystem::RegisterSubSystem(EdSubSystem *) {
}

void EdSystem::Render() {
}

void EdSystem::Reset() {
}

void EdControl::AddMenuItem(eduimenu_s *, EdRef *, void *) {
}

void EdControl::Process(EdInputContext &) {
}

void EdControl::Render() {
}

void EdControl::SelectSubObject() {
}

void EdControl::Refresh() {
}

void EdControl::SetMenuItemAttr(i32, eduiitem_s *, eduiiattr_s *, eduiiattr_s *) {
}

void EdControl::cbSelected(eduimenu_s *, eduiitem_s *, u32) {
}

EdManMove::EdManMove() {
}

void EdManMove::Process(EdInputContext &, ClassObjectList &) {
}

void EdManMove::Render(ClassObjectList &) {
}

void EdRefKnot::GetMemberData(void *, i32, void *, i32) {
}

void EdRefKnot::SetMemberData(void *, i32, void *, i32, i16 *) {
}
