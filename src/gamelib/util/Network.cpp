#include "gamelib_util_types.h"

void NetworkSyncPause() {
}

void NetMessage::DebugPrint() const {
}

void NetMessage::RaiseError() {
}

void NetRotator2::PredictValue(EdClass const *, void *, NetPredictor::PredictorTime *, NetPredictor::PredictorData **,
                               float *, i32) {
}

void NetPredictor::AllowPush(EdClass const *, void const *, ReplicatorData &, i32, i32) {
}

void NetPredictor::CheckPredictionError(EdClass const *, void *, float *, float *, i32) {
}

void NetPredictor::DoPrediction(EdClass const *, void *, ReplicatorData &, NetPredictor::PredictorTime *, i32) {
}

void NetPredictor::DoPrediction(EdClass const *, void *, ReplicatorData &, i32) {
}

void NetPredictor::SerialiseObject(EdStream &, NetPeer *, EdClass const *, void *, ReplicatorData &,
                                   NetPredictor::PredictorTime *, i16 *) {
}

void NetPredictor::SerialiseObject(EdStream &, NetPeer *, EdClass const *, void *, ReplicatorData &, i16 *) {
}

void NetPredictor::StoreSampleData(EdClass const *, void *, NetPredictor::PredictorTime *,
                                   NetPredictor::PredictorData **, float *, i32) {
}

void NetPredictor2::PredictValue(EdClass const *, void *, NetPredictor::PredictorTime *, NetPredictor::PredictorData **,
                                 float *, i32) {
}

void NetPredictor3::PredictValue(EdClass const *, void *, NetPredictor::PredictorTime *, NetPredictor::PredictorData **,
                                 float *, i32) {
}

NetReplicator::NetReplicator(i32, float, float) {
}

void NetReplicator::SerialiseObject(EdStream &, NetPeer *, EdClass const *, void *, ReplicatorData &, i16 *) {
}

void NetworkObject::Destroy() {
}

void NetworkObject::Initialise(i32, void *, EdClass *, NetPeer const &, i32) {
}

void NetListenerList::Find(NetListenerBinding *) {
}

void NetConstReplicator::AllowPush(EdClass const *, void const *, ReplicatorData &, i32, i32) {
}

NetListenerBinding::NetListenerBinding(NetListenerInterface *, unsigned char, char *) {
}

void NetListenerBinding::operator=(NetListenerBinding const &) {
}

void NetListenerBinding::operator==(NetListenerBinding const &) {
}

void NetSimpleReplicator::AllowPush(EdClass const *, void const *, ReplicatorData &, i32, i32) {
}

void NetChangedReplicator::AllowPush(EdClass const *, void const *, ReplicatorData &, i32, i32) {
}

void NetChangedReplicator::CheckSum(unsigned char const *, u32, u32 &) const {
}

void NetChangedReplicator::CheckSumObject(EdClass const *, void const *, u32 &) const {
}

void NetChangedReplicator::InitTable() {
}

void NetworkObjectManager::Acquire(i32) {
}

void NetworkObjectManager::AddToLocalObjectList(NetworkObject *) {
}

void NetworkObjectManager::BindFilter(NOSFilter *, EdClass const *) {
}

void NetworkObjectManager::BindReplicator(NetReplicator *, EdClass const *) {
}

void NetworkObjectManager::CalcReplicatorDataSize(NetReplicator *, EdClass const *, i32 &, i32 &) {
}

void NetworkObjectManager::ChangeContext(NOSContext &) {
}

void NetworkObjectManager::ConstructObject(NetworkObject *, NetworkObjectManager::NetPeerPush *) {
}

void NetworkObjectManager::ContinuityBreak(i32, float) {
}

void NetworkObjectManager::FindNetworkObject(i32) {
}

void NetworkObjectManager::FindNetworkObject(void *) {
}

void NetworkObjectManager::FindPendingObject(NetworkObject *) {
}

void NetworkObjectManager::FlushObjects(i32) {
}

void NetworkObjectManager::GetGuid(void *) {
}

void NetworkObjectManager::GetNextGuid() {
}

void NetworkObjectManager::GetObject(i32) {
}

void NetworkObjectManager::GetPeerStatus() {
}

void NetworkObjectManager::ImportObjects() {
}

void NetworkObjectManager::Init() {
}

void NetworkObjectManager::InitClassStats() {
}

void NetworkObjectManager::IsLocal(i32) {
}

void NetworkObjectManager::IsPeerReady(NetPeer const &) const {
}

void NetworkObjectManager::IsPeerStarted(NetPeer const &) const {
}

NetworkObjectManager::NetworkObjectManager() {
}

void NetworkObjectManager::NotifyCreateObject(void *, EdClass *, void *, i32, i32, i32) {
}

void NetworkObjectManager::NotifyDestroyObject(void *, EdClass *, i32, i32) {
}

void NetworkObjectManager::ObjectCall(void *, i32, NetMessage, NetPeer const *) {
}

void NetworkObjectManager::ObjectOtherCall(void *, i32, NetMessage) {
}

void NetworkObjectManager::ObjectOwnerCall(void *, i32, NetMessage) {
}

void NetworkObjectManager::Owner(i32) {
}

void NetworkObjectManager::PeerJoined(NetPeer const &) {
}

void NetworkObjectManager::PeerLeft(NetPeer const &, ePeerLeftReason) {
}

void NetworkObjectManager::Push(NetworkObject const *, NetReplicator *, ReplicatorData &,
                                NetworkObjectManager::NetPeerPush *) {
}

void NetworkObjectManager::PushObject(NetworkObject *, NetworkObjectManager::NetPeerPush *, i32) {
}

void NetworkObjectManager::Receive(NetMessage, unsigned char, NetPeer const &) {
}

void NetworkObjectManager::ReceiveAcquireMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveAcquiredMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveAdoptedMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveConstructorMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveContinuityBreak(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveObjectCallMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveReleaseMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveRemoteCallMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveReplicaMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveStartMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveStatusMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::ReceiveStopMessage(NetMessage &, NetPeer const &) {
}

void NetworkObjectManager::Recover(NetworkObject *) {
}

void NetworkObjectManager::RegisterObject(void *, EdClass *, i32) {
}

void NetworkObjectManager::RegisterObjectCall(void (*)(void *, NetMessage &), i32) {
}

void NetworkObjectManager::RegisterRemoteCall(void (*)(NetMessage &), i32) {
}

void NetworkObjectManager::ReleaseObject(void *, EdClass *, i32) {
}

void NetworkObjectManager::RemoteCall(i32, NetMessage, NetPeer const *) {
}

void NetworkObjectManager::RemoveFromLocalObjectList(NetworkObject *) {
}

void NetworkObjectManager::RemovePendingObject(NetworkObject *) {
}

void NetworkObjectManager::Reset() {
}

void NetworkObjectManager::SendAcquireMessage(NetworkObject *) {
}

void NetworkObjectManager::SendAcquiredMessage(i16, NetPeer const &) {
}

void NetworkObjectManager::SendAdoptedMessage(i16) {
}

void NetworkObjectManager::SendPushMessage(NetMessage *, NetworkObjectManager::NetPeerPush const *, i32) {
}

void NetworkObjectManager::Start(NOSContext const &) {
}

void NetworkObjectManager::StealPendingObject() {
}

void NetworkObjectManager::Stop() {
}

void NetworkObjectManager::Term() {
}

void NetworkObjectManager::Update() {
}

void NetworkObjectManager::UpdateLocalObjectList() {
}

NetworkObjectManager::~NetworkObjectManager() {
}

void NetStats::Draw(float, float, float, float, NetSmallStats::eInfo) const {
}

void NetStats::Update() {
}

void NetSample::Max(NetSample const &) {
}

void NetSample::Reset() {
}

void NetSample::operator+=(NetSample const &) {
}

void NetSample::operator-=(NetSample const &) {
}

void NetSmallStats::Draw(float, float, float, float, NetSmallStats::eInfo) const {
}
