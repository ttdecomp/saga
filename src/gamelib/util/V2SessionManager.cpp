#include "gamelib_util_types.h"

#include <stdio.h>

void V2SessionManager::Log(char *, ...) {
}

void V2SessionManager::RemoveAllPeers(ePeerLeftReason) {
}

void V2SessionManager::RemovePeer(NetPeer *, ePeerLeftReason) {
}

void V2SessionManager::Reset() {
    if (field_90 != 0) {
        puts("mHostList");
    }
    if (field_9c != 0) {
        puts("mPeerList");
    }

    field_6c = 0;
    field_68 = 0;
    field_74 = 0;
    field_04 = 0;
    field_34 = 0;
    fields_44[0] = -1;
    fields_44[1] = -1;
    fields_44[2] = -1;
    fields_44[3] = -1;
    fields_44[4] = -1;
    fields_44[5] = -1;
    fields_44[6] = -1;
    fields_44[7] = -1;
    fields_44[8] = -1;
}

void V2SessionManager::SetHostGameData(i32 *, i32) {
}

void V2SessionManager::Update() {
}

V2SessionManager::V2SessionManager(char *) {
}

void V2SessionManager::VerifyStrings(char **, char **, i32, char *) {
}
