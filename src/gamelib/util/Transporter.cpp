#include "gamelib_util_types.h"

void NetTransporter::AddListener(NetListenerInterface *, unsigned char, char *) {
}

void NetTransporter::Distribute(NetMessage const &, unsigned char, NetPeer const &) const {
}

void NetTransporter::FtpComplete(FtpFile *, i32) const {
}

void NetTransporter::FtpDownload(FtpFile *) const {
}

void NetTransporter::FtpUpload(FtpFile *) const {
}

void NetTransporter::NosAcquire(NetworkObject *, NetPeer const &) const {
}

void NetTransporter::NosAdopted(NetworkObject *, NetPeer const &) const {
}

void NetTransporter::PeerDead(NetPeer const &) const {
}

void NetTransporter::PeerJoined(NetPeer const &) const {
}

void NetTransporter::PeerLeft(NetPeer const &, ePeerLeftReason) const {
}

void NetTransporter::PeerRequest(NetPeer const &) const {
}

void NetTransporter::RemoveListener(NetListenerInterface *, unsigned char) {
}

void NetTransporter::StatsReceiveMessage(NetMessage, unsigned char) {
}

void NetTransporter::StatsSendMessage(NetMessage, unsigned char) {
}

void NetTransporter::StatsUpdate() {
}
