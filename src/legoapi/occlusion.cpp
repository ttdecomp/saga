#include "legoapi_types.h"
#include "decomp.h"
struct nuvisiboxtree_s;
struct nuvisiboxtreenode_s;

void OccluderSet::AddOccluder(nuvec_s const *, nuvec_s const *, nuvec_s const *, nuvec_s const *) {
}

void OccluderSet::Clear() {
}

void OccluderSet::Init(u32, variptr_u *, variptr_u) {
}

void OccluderSet::IsOccludedOBB(nuvec_s const *, nuvec_s const *, numtx_s const *) {
}

void OccluderSet::IsOccludedSphere(nuvec_s const *, float) {
}

OccluderSet::OccluderSet() {
}

void OccluderSet::OnCameraSet() {
}

void OccluderSet::PrepareForQueries(numtx_s const *, numtx_s const *) {
}

void OccluderSet::RenderOccluders(bool) const {
}

void OccluderSet::SortByDepth(void const *, void const *) {
}

OccluderSet::~OccluderSet() {
}

void OcclusionManager::AddOccluder(nuvec_s const *, float) {
}

void OcclusionManager::AddOccluder(nuvec_s const *, nuvec_s const *, numtx_s const *) {
}

void OcclusionManager::AddOccluder(nuvec_s const *, nuvec_s const *, nuvec_s const *, nuvec_s const *) {
}

void OcclusionManager::BeginFrame() {
}

void OcclusionManager::EndFrame() {
}

void OcclusionManager::Init(u32, variptr_u *, variptr_u) {
}

void OcclusionManager::IsOccludedOBB(nuvec_s const *, nuvec_s const *, numtx_s const *) {
}

void OcclusionManager::IsOccludedSphere(nuvec_s const *, float) {
}

OcclusionManager::OcclusionManager() {
}

void OcclusionManager::OnCameraSet() {
}

void OcclusionManager::RenderStats() const {
}

void OcclusionManager::RenderZPass() const {
}

void OcclusionManager::SetEnabled(bool) {
}

OcclusionManager::~OcclusionManager() {
}

static __used__ void BoxTreeRndrRec(nuvisiboxtree_s*, unsigned char*, nuvisiboxtreenode_s*, int, float, nugscn_s*) {}
