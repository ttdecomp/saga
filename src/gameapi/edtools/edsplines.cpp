#include "gameapi_edtools_types.h"

void SplineLength(nugspline_s *, i32) {
}

void KnotHelper::DistanceToObject(VuVec &, VuVec &, void *, EdRef **) {
}

void KnotHelper::CreateObject(void *, i32, i32) {
}

void KnotHelper::DestroyObject(void *, i32) {
}

void KnotHelper::GetNextObject(void *) {
}

void KnotHelper::GetNumObjects() {
}

void KnotHelper::Process(void *, EdInputContext &) {
}

void KnotHelper::Render(void *, i32) {
}

void SplineKnot::Smooth() {
}

void SplineTool::Initialise(variptr_u &, variptr_u &, i32) {
}

void SplineTool::Process(EdInputContext &) {
}

void SplineTool::Render() {
}

void SplineHelper::AddMenuItems(eduimenu_s *) {
}

void SplineHelper::ClearLevel(i32) {
}

void SplineHelper::CreateObject(void *, i32, i32) {
}

void SplineHelper::DestroyObject(void *, i32) {
}

void SplineHelper::Find(char *) {
}

void SplineHelper::Find(char *, SplineObject **, i32) {
}

void SplineHelper::GetNextObject(void *) {
}

void SplineHelper::GetNumObjects() {
}

void SplineHelper::Initialise() {
}

void SplineHelper::PostLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
}

void SplineHelper::PreLoadInitialisation(MemoryBuffer *, MemoryBuffer *) {
}

void SplineHelper::Process(void *, EdInputContext &) {
}

void SplineHelper::Render(void *, i32) {
}

void SplineHelper::SerialiseObject(EdStream &, void *) {
}

void SplineHelper::cbEdSplineAutoGenPoints(eduimenu_s *, eduiitem_s *, u32) {
}

void SplineHelper::cbEdSplineReGenPoints(eduimenu_s *, eduiitem_s *, u32) {
}

void SplineHelper::cbEdSplineReverseSpline(eduimenu_s *, eduiitem_s *, u32) {
}

void SplineHelper::cbEdSplineSmoothKnot(eduimenu_s *, eduiitem_s *, u32) {
}

void SplineHelper::cbEdSplineSmoothSpline(eduimenu_s *, eduiitem_s *, u32) {
}

void SplineObject::Clone() {
}

void SplineObject::Draw(i32, i32, i32, float) {
}

void SplineObject::DropPoint(VuVec &) {
}

void SplineObject::GenBezierPoints() {
}

void SplineObject::GenLinearPoints() {
}

void SplineObject::GenPoints() {
}

void SplineObject::ReverseKnots() {
}

void SplineObject::SmoothKnots() {
}

void SplineKnotList::GetPoint(i32, VuVec &) {
}

void SplinePointList::AddPoint(VuVec &) {
}

void SplinePointList::Clear() {
}

void SplinePointList::Draw() {
}

void SplinePointList::GetNumPoints() {
}

void SplinePointList::GetPoint(i32, VuVec &) {
}

void SplinePointBlock::Draw() {
}

SplinePointBlock::SplinePointBlock() {
}

SplinePointBlock::SplinePointBlock(i32) {
}

SplinePointBlock::~SplinePointBlock() {
}
