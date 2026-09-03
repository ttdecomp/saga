#include <stddef.h>

#include "gamelib_util_types.h"

extern NetTransporter theNetwork;

void NetFtpManager::Abort(char const *, NetAddress const &, i32, i32) {
}

void NetFtpManager::FindTransfer(char const *, NetAddress const &, i32) {
}

void NetFtpManager::FindTransfer(char const *, NetAddress const &, i32) const {
}

void NetFtpManager::Get(char const *, void *, i32, NetAddress const &) {
}

void NetFtpManager::GetTransfers() const {
}

void NetFtpManager::Init() {
}

NetFtpManager::NetFtpManager() {
}

void NetFtpManager::PeerLeft(NetAddress const &, ePeerLeftReason) {
}

void NetFtpManager::Receive(NetMessage, unsigned char, NetAddress const &) {
}

void NetFtpManager::Reset() {
    FtpFile *file = files;
    FtpFile *end = file + 32;
    NetTransporter *network = &theNetwork;
    do {
        if (file->transfer != NULL) {
            network->FtpComplete(file, static_cast<i32>(0xa0200001));
            file->Term();
            if (file->transfer != NULL && file->network_object != NULL) {
                if (file->network_object->reference_count > 1) {
                    --file->network_object->reference_count;
                } else {
                    file->network_object->reference_count = 0;
                }
            }
            file->transfer = NULL;
        }
        ++file;
    } while (file != end);
}

void NetFtpManager::Send(char const *, void const *, i32, NetAddress const &) {
}

void NetFtpManager::Term() {
}

void NetFtpManager::Update() {
}

NetFtpManager::~NetFtpManager() {
}

void FtpFile::Accept(i32) {
}

void FtpFile::Accept(i32, void *) {
}

void FtpFile::Init(i32, char const *, i32, NetAddress const &, void *, i32) {
}

void FtpFile::RecvData(NetMessage &) {
}

void FtpFile::SendData() {
}

void FtpFile::Term() {
}

void FtpFile::Update() {
}
