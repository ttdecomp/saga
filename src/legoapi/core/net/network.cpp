#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "gamelib/util/gamelib_util_types.h"

void TTNetwork::Broadcast(NetMessage, unsigned char) {
}

void TTNetwork::ClearMyHostAddress() {
}

void TTNetwork::Display(ThingRenderData *) {
}

void TTNetwork::GetMyAddress() const {
}

void TTNetwork::GetMyHostAddress() const {
}

void TTNetwork::Initialise() {
}

void TTNetwork::ProcessEvenWhenPaused(ThingProcessData *) {
}

void TTNetwork::ReliableBroadcast(NetMessage, unsigned char) {
}

void TTNetwork::ReliableSend(NetMessage, unsigned char, NetPeer &, char const *, u32) {
}

void TTNetwork::Resume() {
}

void TTNetwork::Send(NetMessage, unsigned char, NetPeer &) {
}

void TTNetwork::SetMyHostAddress(NetAddress const &) {
}

void TTNetwork::Shutdown() {
}

void TTNetwork::Suspend() {
}

TTNetwork::TTNetwork() {
}

void TTNetwork::Update() {
}

TTNetwork::~TTNetwork() {
}

static __used__ void NOSGetGuid() {
}

extern "C" {

    void WithinConnection(void) {
    }

} // extern "C"
