#include "gamelib_util_types.h"

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
}

void NetFtpManager::Send(char const *, void const *, i32, NetAddress const &) {
}

void NetFtpManager::Term() {
}

void NetFtpManager::Update() {
}

NetFtpManager::~NetFtpManager() {
}

void FtpFile::Accept() {
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
